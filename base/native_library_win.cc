// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2020 The Base Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/native_library.h"

#include <windows.h>

#include "absl/strings/str_format.h"
#include "absl/strings/substitute.h"
#include "base/files/file_util.h"
#include "base/scoped_native_library.h"
#include "base/strings/string_util.h"

namespace base {

namespace {

// forward declare
HMODULE AddDllDirectory(PCSTR new_directory);

// A helper method to check if AddDllDirectory method is available, thus
// LOAD_LIBRARY_SEARCH_* flags are available on systems.
bool AreSearchFlagsAvailable() {
  // The LOAD_LIBRARY_SEARCH_* flags are available on systems that have
  // KB2533623 installed. To determine whether the flags are available, use
  // GetProcAddress to get the address of the AddDllDirectory,
  // RemoveDllDirectory, or SetDefaultDllDirectories function. If GetProcAddress
  // succeeds, the LOAD_LIBRARY_SEARCH_* flags can be used with LoadLibraryEx.
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx
  // The LOAD_LIBRARY_SEARCH_* flags are used in the LoadNativeLibraryHelper
  // method.
  static const auto add_dll_dir_func =
      reinterpret_cast<decltype(AddDllDirectory)*>(
          GetProcAddress(GetModuleHandle("kernel32.dll"), "AddDllDirectory"));
  return !!add_dll_dir_func;
}

NativeLibrary LoadNativeLibraryHelper(const FilePath& library_path,
                                      NativeLibraryLoadError* error) {
  HMODULE module = nullptr;

  bool are_search_flags_available = AreSearchFlagsAvailable();
  if (are_search_flags_available) {
    // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR flag is needed to search the library
    // directory as the library may have dependencies on DLLs in this
    // directory.
    module = ::LoadLibraryEx(
        library_path.value().c_str(), nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    // If LoadLibraryEx succeeds, log this metric and return.
    if (module) {
      return module;
    }
    // GetLastError() needs to be called immediately after
    // LoadLibraryEx call.
    if (error) error->code = ::GetLastError();
  }

  // If LoadLibraryEx API/flags are unavailable or API call fails, try
  // LoadLibrary API.

  // Switch the current directory to the library directory as the library
  // may have dependencies on DLLs in this directory.
  bool restore_directory = false;
  FilePath current_directory;
  if (GetCurrentDirectory(&current_directory)) {
    FilePath plugin_path = library_path.DirName();
    if (!plugin_path.empty()) {
      SetCurrentDirectory(plugin_path);
      restore_directory = true;
    }
  }
  module = ::LoadLibrary(library_path.value().c_str());

  // GetLastError() needs to be called immediately after LoadLibraryW call.
  if (!module && error) error->code = ::GetLastError();

  if (restore_directory) SetCurrentDirectory(current_directory);

  return module;
}

NativeLibrary LoadSystemLibraryHelper(const FilePath& library_path,
                                      NativeLibraryLoadError* error) {
  NativeLibrary module;
  BOOL module_found =
      ::GetModuleHandleEx(0, library_path.value().c_str(), &module);
  if (!module_found) {
    bool are_search_flags_available = AreSearchFlagsAvailable();
    // Prefer LOAD_LIBRARY_SEARCH_SYSTEM32 to avoid DLL preloading attacks.
    DWORD flags = are_search_flags_available ? LOAD_LIBRARY_SEARCH_SYSTEM32
                                             : LOAD_WITH_ALTERED_SEARCH_PATH;
    module = ::LoadLibraryEx(library_path.value().c_str(), nullptr, flags);

    if (!module && error) error->code = ::GetLastError();
  }

  return module;
}

FilePath GetSystemLibraryName(absl::string_view name) {
  char system_buffer[MAX_PATH];
  // Use an absolute path to load the DLL to avoid DLL preloading attacks.
  if (GetSystemDirectory(system_buffer, MAX_PATH)) {
    FilePath library_path(system_buffer);
    return library_path.Append(name);
  }
  return FilePath();
}

}  // namespace

std::string NativeLibraryLoadError::ToString() const {
  return absl::StrFormat("%lu", code);
}

NativeLibrary LoadNativeLibraryWithOptions(const FilePath& library_path,
                                           const NativeLibraryOptions& options,
                                           NativeLibraryLoadError* error) {
  return LoadNativeLibraryHelper(library_path, error);
}

void UnloadNativeLibrary(NativeLibrary library) { FreeLibrary(library); }

void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
                                          absl::string_view name) {
  return reinterpret_cast<void*>(GetProcAddress(library, name.data()));
}

std::string GetNativeLibraryName(absl::string_view name) {
  DCHECK(IsStringASCII(name));
  return absl::Substitute("$0.dll", name);
}

NativeLibrary LoadSystemLibrary(absl::string_view name,
                                NativeLibraryLoadError* error) {
  FilePath library_path = GetSystemLibraryName(name);
  if (library_path.empty()) {
    if (error) error->code = ERROR_NOT_FOUND;
    return nullptr;
  }
  return LoadSystemLibraryHelper(library_path, error);
}

NativeLibrary PinSystemLibrary(absl::string_view name,
                               NativeLibraryLoadError* error) {
  FilePath library_path = GetSystemLibraryName(name);
  if (library_path.empty()) {
    if (error) error->code = ERROR_NOT_FOUND;
    return nullptr;
  }

  // GetModuleHandleEx acquires the LoaderLock, hence must not be called from
  // Dllmain.
  ScopedNativeLibrary module;
  if (::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN,
                          library_path.value().c_str(),
                          ScopedNativeLibrary::Receiver(module).get())) {
    return module.release();
  }

  // Load and pin the library since it wasn't already loaded.
  module = ScopedNativeLibrary(LoadSystemLibraryHelper(library_path, error));
  if (!module.is_valid()) return nullptr;

  ScopedNativeLibrary temp;
  if (::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN,
                          library_path.value().c_str(),
                          ScopedNativeLibrary::Receiver(temp).get())) {
    return module.release();
  }

  if (error) error->code = ::GetLastError();
  // Return nullptr since we failed to pin the module.
  return nullptr;
}

}  // namespace base
