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

#include <array>
#include <ctime>
#include <iostream>
#include <string_view>

namespace horoni::log {
    enum class Level : size_t {
        Dbg,
        Info,
        Warn,
        Err,
    };
    
    namespace detail {
        inline auto get_time_str() -> std::string_view {
            thread_local static std::array<char, 20> buf;
            thread_local static std::time_t prev_time = 0;
            std::time_t time = std::time(nullptr);
            if (time != prev_time) {
                std::tm loc{};

                #if defined(_WIN32)
                localtime_s(&local_time, &time);
                #elif defined(__linux__)
                localtime_r(&time, &loc);
                #endif

                std::strftime(buf.data(), buf.size(), "%F %T", &loc);
            }
            prev_time = time;
            return {buf.data(), buf.size()};
        }
    
        template<class... Ts>
        void log_(Level log_level, std::string_view msg) {
            static constexpr std::array<const char *, 4> level_prefixes= {
                "\033[37m[DBG  ",
                "\033[36m[INFO ",
                "\033[33m[WARN ",
                "\033[31m[ERR  "
            };
            const char *level_prefix = level_prefixes[static_cast<size_t>(log_level)];
            std::cout << level_prefix
                << get_time_str() << "]\033[0m\t"
                << msg << "\n";
        }
    } // namespace detail
    inline Level g_level = Level::Dbg;

    template<class... Ts>
    void log(std::string_view  msg) { detail::log_(g_level, msg); }
    template<class... Ts>
    void dbg(std::string_view  msg) { detail::log_(Level::Dbg,  msg); }
    template<class... Ts>
    void info(std::string_view msg) { detail::log_(Level::Info, msg); }
    template<class... Ts>
    void warn(std::string_view msg) { detail::log_(Level::Warn, msg); }
    template<class... Ts>
    void err(std::string_view  msg) { detail::log_(Level::Err,  msg); }
} // namespace horoni::log

#endif // HORONI_LOG_HPP
