#ifndef DYNAMICLIBRARYHELPER_H
#define DYNAMICLIBRARYHELPER_H

#if defined(_MSC_VER)
#   define _DYNAMICLIBRARY_WINDOWS
#   define _DYNAMICLIBRARY_OSNAME "Windows"
#endif

#ifdef __linux__
#   define _DYNAMICLIBRARY_LINUX
#   define _DYNAMICLIBRARY_OSNAME "Linux"
#endif

#include <iostream>
#include <string>
#include "deps/encoding.h"

#ifdef _DYNAMICLIBRARY_LINUX
#include <dlfcn.h>
#include <sys/types.h>
#endif
#ifdef _DYNAMICLIBRARY_WINDOWS
#include <windows.h>
#include <direct.h>
#endif

//****************************************************************************//
//****************************************************************************//
//**************                                                **************//
//**************                                                **************//
//**************  STRUCTS AND METHODS FOR CALLING APPLICATIONS  **************//
//**************                                                **************//
//**************                                                **************//
//****************************************************************************//
//****************************************************************************//

#ifdef _DYNAMICLIBRARY_LINUX
#define LibHandle void *
#endif
#ifdef _DYNAMICLIBRARY_WINDOWS
#define LibHandle HINSTANCE
#endif

struct LibraryInfo {
  std::string linux_lib_path;// { "../genericlib/genericlib.so" };
  bool linux_open_as_rtld_lazy{ true };
  std::string windows_lib_path;// { "../x64/Release/genericlib.dll" };
};

inline
bool OpenDynLibPlugIn(std::string const & lib_path,
                      LibHandle * lib_handle_ptr,
                      bool linux_open_as_rtld_lazy = true) {
#ifdef _DYNAMICLIBRARY_LINUX
  (*lib_handle_ptr) = dlopen(lib_path.c_str(),
    (linux_open_as_rtld_lazy) ? RTLD_LAZY : RTLD_NOW);

  if ((*lib_handle_ptr) == NULL) {
    std::cerr << "Cannot load library: " <<
      lib_path << std::endl;
    fprintf(stderr, "%s\n", dlerror());
    return false;
  }
#endif

#ifdef _DYNAMICLIBRARY_WINDOWS
  bool conv_ok;
#ifdef UNICODE
  std::u16string utf16_lib_path =
    EncodingLib_Conv_UTF8N_UTF16W::ToUTF16W(lib_path,
                                              &conv_ok);
  if (!conv_ok)
    return false;

  (*lib_handle_ptr) = LoadLibrary((const wchar_t *)utf16_lib_path.c_str());
#else
  (*lib_handle_ptr) = LoadLibrary((const char *)lib_path.c_str());
#endif
  if ((*lib_handle_ptr) == NULL) {
    std::cerr << "Cannot load library: " <<
      lib_path << std::endl;
    return false;
  }

#endif

  return true;
}


inline
bool OpenDynLibPlugIn(LibraryInfo const & library_info,
                      LibHandle * lib_handle_ptr) {
#ifdef _DYNAMICLIBRARY_LINUX
  (*lib_handle_ptr) = dlopen(library_info.linux_lib_path.c_str(),
    (library_info.linux_open_as_rtld_lazy) ? RTLD_LAZY : RTLD_NOW);

  if ((*lib_handle_ptr) == NULL) {
    std::cerr << "Cannot load library: " <<
      library_info.linux_lib_path << std::endl;
    fprintf(stderr, "%s\n", dlerror());
    return false;
  }
#endif

#ifdef _DYNAMICLIBRARY_WINDOWS
  bool conv_ok;
#ifdef UNICODE
  std::u16string utf16_lib_path =
    EncodingLib_Conv_UTF8N_UTF16W::ToUTF16W(library_info.windows_lib_path,
                                            &conv_ok);
  if (!conv_ok)
    return false;

  (*lib_handle_ptr) = LoadLibrary((const wchar_t *)utf16_lib_path.c_str());
#else
  (*lib_handle_ptr) = LoadLibrary((const char *)library_info.windows_lib_path.c_str());
#endif
  if ((*lib_handle_ptr) == NULL) {
    std::cerr << "Cannot load library: " <<
      library_info.windows_lib_path << std::endl;
    return false;
  }

#endif

  return true;
}

//Returns address to the function which has been loaded with the shared library.
inline
bool GetDynLibFunctionPointer(const std::string & function_name,
                              LibHandle lib_handle,
                              void ** function_ptr) {
#ifdef _DYNAMICLIBRARY_LINUX

  dlerror();    /* Clear any existing error */

  (*function_ptr) = dlsym(lib_handle, function_name.c_str());

  const char* dlsym_error = dlerror();
  if (dlsym_error != NULL) {
    std::cerr << "Cannot load symbol " << function_name <<
      ": " << dlsym_error << std::endl;
    return false;
  }
#endif

#ifdef _DYNAMICLIBRARY_WINDOWS
  (*function_ptr) = GetProcAddress(lib_handle, function_name.c_str());
  if (!(*function_ptr)) {
    std::cerr << "Cannot load symbol " << function_name <<
      ": " << GetLastError() << std::endl;
    return false;
  }


#endif

  return true;
}

inline
bool CloseDynLibPlugIn(LibHandle lib_handle) {
#ifdef _DYNAMICLIBRARY_LINUX
  int retval = dlclose(lib_handle);
  return (retval == 0) ? true : false;
#endif

#ifdef _DYNAMICLIBRARY_WINDOWS
  FreeLibrary(lib_handle);
#endif

  return true;
}


#endif //DYNAMICLIBRARYHELPER_H
