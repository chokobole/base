#include <iostream>

#include "base/event_loop/event_loop.h"
#include "base/io_buffer.h"
#include "base/logging.h"
#include "base/socket/socket_errors.h"
#include "base/socket/unix_domain_client_socket_posix.h"
#include "base/socket/unix_domain_server_socket_posix.h"

const char* kMessage = "hello world";

bool CheckCredentials(
    const base::UnixDomainServerSocket::Credentials& credentials) {
  std::cout << "Connected with ";
#if defined(OS_LINUX) || defined(OS_ANDROID)
  std::cout << "pid: " << credentials.process_id << ", ";
#endif
  std::cout << "uid: " << credentials.user_id
            << ", gid: " << credentials.group_id << std::endl;
  return true;
}

class EchoServer {
 public:
  explicit EchoServer(base::ServerSocket* server_socket)
      : server_socket_(server_socket) {}

  void Start() {
    server_socket_->Accept(&accepted_socket_, [this](int result) {
      if (result != base::OK) {
        LOG(ERROR) << base::ErrorToShortString(result);
        return;
      }
      Read(strlen(kMessage));
    });
  }

 private:
  void Read(size_t len) {
    read_buffer_ = std::make_shared<base::GrowableIOBuffer>();
    read_buffer_->SetCapacity(len);
    size_t to_read = read_buffer_->capacity();
    while (to_read > 0) {
      int rv = accepted_socket_->Read(read_buffer_, to_read,
                                      [this](int result) { OnRead(result); });

      if (rv == base::ERR_IO_PENDING) {
        break;
      }

      if (rv > 0) {
        read_buffer_->SetOffset(read_buffer_->offset() + rv);
        to_read -= rv;
      }

      if (to_read == 0 || rv <= 0) {
        OnRead(rv);
        break;
      }
    }
  }

  void OnRead(int result) {
    if (result < 0) {
      LOG(ERROR) << base::ErrorToShortString(result);
      return;
    }
    std::cout << "Received: "
              << std::string(read_buffer_->StartOfBuffer(),
                             read_buffer_->offset())
              << std::endl;
  }

  base::ServerSocket* server_socket_;
  std::unique_ptr<base::StreamSocket> accepted_socket_;
  std::shared_ptr<base::GrowableIOBuffer> read_buffer_;
};

class EchoClient {
 public:
  explicit EchoClient(base::StreamSocket* stream_socket)
      : stream_socket_(stream_socket) {}

  void ConnectAndWrite(const std::string& text) {
    text_ = text;
    int result =
        stream_socket_->Connect([this](int result) { OnConnect(result); });
    if (result != base::ERR_IO_PENDING) {
      OnConnect(result);
    }
  }

 private:
  void OnConnect(int result) {
    if (result != base::OK) {
      LOG(ERROR) << base::ErrorToShortString(result);
      return;
    }
    Write();
  }

  void Write() {
    std::shared_ptr<base::StringIOBuffer> buffer(
        new base::StringIOBuffer(text_));
    std::shared_ptr<base::DrainableIOBuffer> write_buffer(
        new base::DrainableIOBuffer(buffer, buffer->size()));

    while (write_buffer->BytesRemaining() > 0) {
      int rv =
          stream_socket_->Write(write_buffer, write_buffer->BytesRemaining(),
                                [this](int result) { OnWrite(result); });

      if (rv == base::ERR_IO_PENDING) break;

      if (rv > 0) {
        write_buffer->DidConsume(rv);
      }

      if (write_buffer->BytesRemaining() == 0 || rv <= 0) {
        OnWrite(rv);
        break;
      }
    }
  }

  void OnWrite(int result) {
    if (result < 0) {
      LOG(ERROR) << base::ErrorToShortString(result);
      return;
    }
  }

  std::string text_;
  base::StreamSocket* stream_socket_;
};

class Worker : public base::EventLoop::Delegate {
 public:
  Worker() {}

  bool DoIdleWork() override { return false; }
};

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  base::EventLoop event_loop;
  std::string socket_path("uds_socket");
  base::UnixDomainClientSocket client_socket(socket_path, false);
  base::UnixDomainServerSocket server_socket(&CheckCredentials, false);
  int rv = server_socket.BindAndListen(socket_path, 5);
  if (rv != 0) {
    LOG(ERROR) << base::ErrorToShortString(rv);
    return 1;
  }
  EchoServer echo_server(&server_socket);
  echo_server.Start();
  EchoClient echo_client(&client_socket);
  echo_client.ConnectAndWrite(kMessage);

  Worker* worker = new Worker();
  event_loop.Run(worker);

  return 0;
}