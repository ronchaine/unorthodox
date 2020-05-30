#ifndef UNORTHODOX_ERROR_CODES_HPP
#define UNORTHODOX_ERROR_CODES_HPP

#include <errno.h>
#include <iostream>

#include "deps/expected.hpp"

namespace unorthodox
{
    enum class error_domain : uint16_t
    {
        generic_error = 0,
        network_error = 1,
    };

    // to be split to error_value and error_code
    struct error_code
    {
        using err_value_type = uint16_t;

        constexpr static err_value_type success                 = 0x0000;

        // general
        constexpr static err_value_type undefined_error         = 0xe000;
        constexpr static err_value_type access_denied           = 0xe001;
        constexpr static err_value_type resource_busy           = 0xe002;
        constexpr static err_value_type connection_reset        = 0xe003;
        constexpr static err_value_type uninitialised_value     = 0xe004;
        constexpr static err_value_type unimplemented_feature   = 0xe005;

        // network
        constexpr static err_value_type setsockopt_failed       = 0xe010;
        constexpr static err_value_type socket_already_open     = 0xe011;
        constexpr static err_value_type cannot_open_socket      = 0xe012;
        constexpr static err_value_type could_not_listen        = 0xe013;
        constexpr static err_value_type no_active_socket        = 0xe014;
        constexpr static err_value_type failed_to_send_data     = 0xe015;
        constexpr static err_value_type wrong_socket_type       = 0xe016;

        // platform-specific
        constexpr static err_value_type poll_error              = 0xe100;

        constexpr error_code() noexcept = default;

        constexpr error_code(error_domain domain, err_value_type err) noexcept
            : domain(domain), code(err) {}

        constexpr error_code(err_value_type err) noexcept
            : domain(error_domain::generic_error), code(err) {}

        constexpr static err_value_type from_errno() noexcept;

        constexpr operator bool() const noexcept { return !(!code); }

        error_domain    domain;
        err_value_type  code;
    };

    // futureproofing...
    using error_value = error_code;

    //constexpr static error success;

    constexpr error_code::err_value_type error_code::from_errno() noexcept
    {
        return error_value::undefined_error;
    }

    /*
    inline error_code translate_error(int posix_errno)
    {
        if (posix_errno == EACCES)       return error::access_denied;
        if (posix_errno == EBUSY)        return error::resource_busy;
        if (posix_errno == ECONNRESET)   return error::connection_reset;

        std::cerr << "unhandled error value: " << posix_errno << "\n";
        return error::undefined_error;
    }

    inline error_code from_errno(domain, int posix_errno)
    {
        return translate_error(posix_errno);
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
    */
};

#endif
