/*
 * Copyright (c) 2025, horoni. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef HORONI_LOG_HPP
#define HORONI_LOG_HPP

#include "format.hpp"

#include <array>
#include <ctime>
#include <iostream>

enum class LogLevel : size_t {
    Dbg,
    Info,
    Warn,
    Err,
};

namespace {
    inline auto get_time_str() -> std::string_view {
        thread_local static std::array<char, 20> buf;
        std::time_t time = std::time(nullptr);
        std::tm local_time{};

        #if defined(_WIN32)
        localtime_s(&local_time, &time);
        #elif defined(__linux__)
        localtime_r(&time, &local_time);
        #endif

        const size_t len = std::strftime(buf.data(), buf.size(), "%F %T", &local_time);
        return {buf.data(), len};
    }
    
    template<class... Ts>
    void log_(LogLevel log_level, const char *format, Ts... args) {
        static constexpr std::array<const char *, 4> level_prefixes= {
            "[DBG  ",
            "\033[36m[INFO ",
            "\033[33m[WARN ",
            "\033[31m[ERR  "
        };
        const char *level_prefix = level_prefixes[static_cast<size_t>(log_level)];
        std::cout << level_prefix
            << get_time_str() << "]\033[0m\t"
            << horoni::fmt::fmt(format, std::forward<Ts>(args)...) << "\n";
    }
}

namespace Log {
    inline LogLevel g_level = LogLevel::Dbg;

    template<class... Ts>
    void log(const char *format, Ts... args) {
        ::log_(g_level, format, std::forward<Ts>(args)...);
    }
    
    template<class... Ts>
    void dbg(const char *format, Ts... args) {
        ::log_(LogLevel::Dbg, format, std::forward<Ts>(args)...);
    }

    template<class... Ts>
    void info(const char *format, Ts... args) {
        ::log_(LogLevel::Info, format, std::forward<Ts>(args)...);
    }
    
    template<class... Ts>
    void warn(const char *format, Ts... args) {
        ::log_(LogLevel::Warn, format, std::forward<Ts>(args)...);
    }
    
    template<class... Ts>
    void err(const char *format, Ts... args) {
        ::log_(LogLevel::Err, format, std::forward<Ts>(args)...);
    }

};

#endif // HORONI_LOG_HPP
