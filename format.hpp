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

#ifndef HORONI_FORMAT_HPP
#define HORONI_FORMAT_HPP

#include <array>
#include <stdexcept>
#include <type_traits>

namespace horoni::fmt {
    namespace {
        inline thread_local std::array<char, 1024> fmt_buffer;
    
        template<class T>
        constexpr decltype(auto) fmt_arg(T&& arg) noexcept {
            if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string> ||
                          std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string_view>) {
                return arg.data();
            } else {
                return std::forward<T>(arg);
            }
        }
    }

    template<class... Ts>
    inline size_t fmt_size(const char* format, Ts&&... args) {
        const size_t req_siz = std::snprintf(
            nullptr, 0,
            format, fmt_arg(std::forward<Ts>(args))...
        );
        return req_siz;
    }
    
    template<class... Ts>
    inline size_t fmt_to(char *buf, size_t bufsiz, const char* format, Ts&&... args) {
        const size_t req_siz = std::snprintf(
            buf, bufsiz,
            format, fmt_arg(std::forward<Ts>(args))...
        );

        if (req_siz < 0) {
            throw std::runtime_error("horoni::fmt::fmt_to(): encoding error.");
        }
        return req_siz;
    }

    template<class... Ts>
    auto vfmt(const char* format, Ts&&... args) -> std::string {
        const size_t req_siz = std::snprintf(
            fmt_buffer.data(), fmt_buffer.size(),
            format, fmt_arg(std::forward<Ts>(args))...
        );

        if (req_siz < fmt_buffer.size()) {
            return {fmt_buffer.data(), req_siz};
        } else if (req_siz < 0) {
            throw std::runtime_error("horoni::fmt::vfmt(): encoding error.");
        }

        std::string str;
        str.resize(req_siz + 1);
        const size_t fin_siz = std::snprintf(
            str.data(), str.size(),
            format, fmt_arg(std::forward<Ts>(args))...
        );

        if (fin_siz < 0 || fin_siz > str.size()) {
            throw std::runtime_error("horoni::fmt::vfmt(): failed.");
        }

        return str;
    }

    template<class... Ts>
    auto fmt(const char* format, Ts&&... args) -> std::string {
        // TODO(): Add format string constexpr checker
        return vfmt(format, std::forward<Ts>(args)...);
    }
}

#endif // HORONI_FORMAT_HPP
