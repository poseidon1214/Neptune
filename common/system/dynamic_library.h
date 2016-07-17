// Copyright (c) 2010, Tencent Inc. All rights reserved.
// Author: Chen Feng <phongchen@tencent.com>

#ifndef COMMON_SYSTEM_DYNAMIC_LIBRARY_H
#define COMMON_SYSTEM_DYNAMIC_LIBRARY_H

#include <stddef.h>
#include <dlfcn.h>

#  define DLL_IMPORT __attribute__ ((visibility("default")))
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#  define DLL_LOCAL  __attribute__ ((visibility("hidden")))

// Generic helper definitions above to define FOO_API and FOO_LOCAL.
// FOO_API is used for the public API symbols. It either DLL imports or DLL
// exports (or does nothing for static build)
// FOO_LOCAL is used for non-api symbols.

// #ifdef FOO_DLL // defined if FOO is compiled as a DLL
//   // defined if we are building the FOO DLL (instead of using it)
//   #ifdef FOO_DLL_EXPORTS
//     #define FOO_API DLL_EXPORT
//   #else
//     #define FOO_API DLL_IMPORT
//   #endif // FOO_DLL_EXPORTS
//   #define FOO_LOCAL DLL_LOCAL
// #else // FOO_DLL is not defined: this means FOO is a static lib.
//   #define FX_API
//   #define FOO_LOCAL
// #endif // FOO_DLL

// namespace common {

/// dynamic library encapsulation
class DynamicLibrary
{
public:
    DynamicLibrary();
    ~DynamicLibrary();

    /// Loads the library filename
    // RTLD_NOW: resolve all symbols on load
    // RTLD_LOCAL: don't resolve symbols for other libraries
    bool Load(const char* filename, int flag = RTLD_NOW | RTLD_LOCAL);

    /// Unload the current library
    bool Unload();

    /// Gets a pointer to the symbol of 'name' by getting it from the
    /// shared object
    void* GetSymbol(const char* name) const;

    /// type safed version of GetSymbol
    template <typename T>
    bool GetSymbol(const char* name, T** address) const
    {
        *address = reinterpret_cast<T*>(GetSymbol(name));
        return *address != NULL;
    }

    /// Returns true if the library is loaded
    bool IsLoaded() const
    {
        return m_library != 0;
    }

private:
    DynamicLibrary(const DynamicLibrary&);
    DynamicLibrary& operator=(const DynamicLibrary&);

private:
    /// Library handle
    void* m_library;
};

// } // namespace common

#endif // COMMON_SYSTEM_DYNAMIC_LIBRARY_H
