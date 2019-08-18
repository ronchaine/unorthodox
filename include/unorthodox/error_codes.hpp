#ifndef UNORTHODOX_ERROR_CODES_HPP
#define UNORTHODOX_ERROR_CODES_HPP

#include <errno.h>
#include <iostream>

namespace unorthodox
{
    using error_code = int;

    class error
    {
        public:
            // general
            constexpr static error_code undefined_error         = -0xe000;
            constexpr static error_code access_denied           = -0xe001;
            constexpr static error_code resource_busy           = -0xe002;
            constexpr static error_code connection_reset        = -0xe003;
            constexpr static error_code uninitialised_value     = -0xe004;
            constexpr static error_code unimplemented_feature   = -0xe005;

            // network
            constexpr static error_code setsockopt_failed       = -0xe010;
            constexpr static error_code socket_already_open     = -0xe011;
            constexpr static error_code cannot_open_socket      = -0xe012;
            constexpr static error_code could_not_listen        = -0xe013;
            constexpr static error_code no_active_socket        = -0xe014;
            constexpr static error_code failed_to_send_data     = -0xe015;
            constexpr static error_code wrong_socket_type       = -0xe016;

            constexpr error(int ecode) { code = ecode < 0 ? ecode : 0; }

            // platform-specific
            constexpr static error_code epoll_error             = -0xe100;


        private:
            int code = 0;
    };

    inline error_code translate_error(int unix_error)
    {
        if (unix_error == EACCES)       return error::access_denied;
        if (unix_error == EBUSY)        return error::resource_busy;
        if (unix_error == ECONNRESET)   return error::connection_reset;

        std::cerr << "unhandled error value: " << unix_error << "\n";
        return error::undefined_error;
    }

    constexpr inline const char* error_string(const error_code code)
    {
        if (code >= 0)                          return "success";
        if (code == error::access_denied)       return "access denied";
        if (code == error::resource_busy)       return "resource busy";
        if (code == error::connection_reset)    return "connection reset";
        if (code == error::uninitialised_value) return "uninitialised value";

        return "unknown error";
    }
};

#endif
