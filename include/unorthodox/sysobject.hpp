#ifndef UNORTHODOX_SYSOBJECT_HPP
#define UNORTHODOX_SYSOBJECT_HPP

#ifndef _WIN32
#define UNORTHODOX_SO_LOADER_DLOPEN
#else
#define UNORTHODOX_SO_LOADER_WINDOWS
#endif

#ifdef UNORTHODOX_SO_LOADER_DLOPEN
#include <dlfcn.h>
#endif

#ifdef UNORTHODOX_SO_LOADER_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "file.hpp"

namespace unorthodox
{
    class system_library
    {
        public:
            system_library(const std::string& library) noexpect;
           ~system_library();

            system_library(const system_library&) = delete;

            const std::string search(const std::string& library) const noexpect;

        private:
            void* handle = nullptr;
    };
}

namespace unorthodox
{
#ifdef UNORTHODOX_SO_LOADER_DLOPEN
    system_library::system_library(const std::string& library) noexpect
    {
    }

    const std::string search(const std::string& library) const noexcept
    {
    }

    system_library::~system_library()
    {
    }
#endif

#ifdef UNORTHODOX_SO_LOADER_WINDOWS
    system_library::system_library()
    {
    }
    system_library::~system_library()
    {
    }
#endif
}

#endif
