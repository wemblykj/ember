#pragma once

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace ember::core {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& instance();

    void log(LogLevel level, std::string_view message);

    // Compile-time-checked format string (preferred for literals)
    template<typename... Args>
    void log(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
    {
        log(level, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void warning(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Warning, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void fatal(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Fatal, fmt, std::forward<Args>(args)...); }

    void setLevel(LogLevel level) { minLevel_ = level; }

private:
    Logger() = default;
    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(LogLevel level) const;
    std::string getCurrentTime() const;

    LogLevel minLevel_ = LogLevel::Debug;
};

}  // namespace ember::core

// Convenience macros (variadic; call with one or more arguments)
#define EMBER_LOG_DEBUG(...)  ember::core::Logger::instance().debug(__VA_ARGS__)
#define EMBER_LOG_INFO(...)   ember::core::Logger::instance().info(__VA_ARGS__)
#define EMBER_LOG_WARN(...)   ember::core::Logger::instance().warning(__VA_ARGS__)
#define EMBER_LOG_ERROR(...)  ember::core::Logger::instance().error(__VA_ARGS__)
#define EMBER_LOG_FATAL(...)  ember::core::Logger::instance().fatal(__VA_ARGS__)