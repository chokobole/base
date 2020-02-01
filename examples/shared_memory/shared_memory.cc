#include <iostream>

#include "base/logging.h"
#include "base/memory/read_only_shared_memory_region.h"

const char* kMessage = "hello world";

class ReadOnlySharedBuffer {
 public:
  explicit ReadOnlySharedBuffer(size_t size) {
    base::MappedReadOnlyRegion mapped_region =
        base::ReadOnlySharedMemoryRegion::Create(size);
    CHECK(mapped_region.IsValid());
    region_ = std::move(mapped_region.region);
    mapping_ = std::move(mapped_region.mapping);
  }

  void Write(const std::string& text) {
    memcpy(mapping_.memory(), text.c_str(), text.length());
  }

  void* data() { return mapping_.memory(); }

 private:
  base::WritableSharedMemoryMapping mapping_;
  base::ReadOnlySharedMemoryRegion region_;
};

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  ReadOnlySharedBuffer buffer(strlen(kMessage));
  buffer.Write(kMessage);

  std::cout << std::string(reinterpret_cast<const char*>(buffer.data()),
                           strlen(kMessage))
            << std::endl;

  return 0;
}