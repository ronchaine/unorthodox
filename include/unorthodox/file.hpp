#ifndef UNORTHODOX_FILE_HPP
#define UNORTHODOX_FILE_HPP

#include <unistd.h>
#include <fcntl.h>

#include "util.hpp"
#include "error_codes.hpp"

namespace unorthodox
{
// todo: translate win32 commands
#ifdef _WIN32
#else
#endif
}

namespace unorthodox
{
    class file
    {
        public:
            template <bool is_const> class iterator_type;

            using value_type        = std::byte;
            using size_type         = std::size_t;
            using difference_type   = std::ptrdiff_t;
            using reference         = std::byte&;
            using const_reference   = const std::byte&;
            using pointer           = std::byte*;
            using const_pointer     = const std::byte*;

            using iterator          = iterator_type<false>;
            using const_iterator    = iterator_type<true>;
        /*
        _O_APPEND                       O_APPEND
                                        O_ASYNC
        _O_BINARY
                                        O_CLOEXEC
        _O_CREAT (requires pmode)       O_CREAT
            _O_TEMPORARY                O_TMPFILE
            _O_SHORT_LIVED
                                        O_DIRECT
                                        O_DIRECTORY
                                        O_DSYNC
        _O_EXCL                         O_EXCL
                                        O_LARGEFILE
                                        O_NOATIME
                                        O_NOCTTY
                                        O_NOFOLLOW
                                        O_NONBLOCK / O_NDELAY
        _O_NONINHERIT
                                        O_PATH
        _O_RANDOM
        _O_RDONLY                       O_RDONLY
        _O_RDWR                         O_RDWR
        _O_SEQUENTIAL
                                        O_SYNC
        _O_TEXT
        _O_TRUNC                        O_TRUNC
        _O_WRONLY                       O_WRONLY
        _O_U16TEXT
        _O_U8TEXT
        _O_WTEXT

        pmode
         _S_IREAD
        _S_IWRITE
        */
        public:
            using flagset = uint8_t;

            constexpr static flagset APPEND     = 0x01;
            constexpr static flagset READ       = 0x02;
            constexpr static flagset WRITE      = 0x04;
            constexpr static flagset MMAP       = 0x08;
            constexpr static flagset TRUNCATE   = 0x10;
            
            constexpr static flagset RW         = READ | WRITE;

            // use these to get actual file instances
            static file open(const char* file, flagset flags = READ | WRITE) noexcept;
            static file create(const char* file, flagset flags) noexcept;

           ~file();

            void close();

            // check if we're valid
            operator bool() { return fd >= 0; }

            // retrieve error if something has gone wrong
            error_code error() { return fd < 0 ? fd : 0; }

        private:
            static inline int translate_flags(flagset flags);

            // do not allow straight constructing
            file() = default;

            int fd: 31 = error::uninitialised_value;
            bool mmapped : 1 = false;
    };
}

namespace unorthodox
{
    inline int file::translate_flags(flagset flags)
    {
        int rval = 0;
        #ifdef _WIN32
        #else
        // POSIX
        if ((flags & READ) && (flags & WRITE)) rval |= O_RDWR;
        else if (flags & WRITE)  rval |= O_WRONLY;
        else rval |= O_RDONLY;

        if (flags & APPEND) rval |= O_APPEND;

        #endif
        return rval;
    }

    file file::open(const char* filename, flagset flags) noexcept
    {
        file rval;

        int filed = ::open(filename, translate_flags(flags));
        if (filed == -1)
            rval.fd = translate_error(errno);
        else
            rval.fd = filed;

        return rval;
    }

    file file::create(const char* filename, flagset flags) noexcept
    {
        return file();
    }

    file::~file()
    {
        close();
    }

    void file::close()
    {
        if (fd >= 0)
            ::close(fd);
        fd = -1;
    }
}

#endif
