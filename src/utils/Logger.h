//
// Created by HuyN on 25/12/2025.
//

#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#pragma once

#include <mutex>
#include <utility>
#include <fmt/core.h>
#include <fmt/color.h>

namespace engine::utils::Logger {

    inline std::mutex& getMutex() {
        static std::mutex m;
        return m;
    }

    template<typename... Args>
    inline void log(fmt::color color, std::string_view level, fmt::format_string<Args...> format, Args&&... args) {
        std::lock_guard<std::mutex> lock(getMutex());
        fmt::print(fg(color) | fmt::emphasis::bold, "[{:^9}] ", level);
        fmt::print(format, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template<typename... Args>
    inline void info(fmt::format_string<Args...> fmt, Args&&... args) {
        log(fmt::color::cyan, "INFO", fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void success(fmt::format_string<Args...> fmt, Args&&... args) {
        log(fmt::color::green, "SUCCESS", fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        log(fmt::color::yellow, "WARN", fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void error(fmt::format_string<Args...> fmt, Args&&... args) {
        log(fmt::color::red, "ERROR", fmt, std::forward<Args>(args)...);
    }
}

#endif //ENGINE_LOGGER_H