#pragma once

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
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

    // Base sink: accepts already-formatted message or simple runtime messages.
    void log(LogLevel level, std::string_view message);

    // Runtime-only format overload (std::string / std::string_view friendly).
    // This single approach avoids overload ambiguities and supports passing std::string
    // as the format. Arguments are materialized into a tuple so std::make_format_args
    // receives lvalue references (avoids MSVC binding errors).
    template<typename... Args>
    void log(LogLevel level, std::string_view fmt, Args&&... args)
    {
        if constexpr (sizeof...(Args) == 0) {
            // No format args: treat fmt as the final message (avoid vformat)
            log(level, fmt);
            return;
        }

        // Materialize arguments into a tuple so we can pass them as lvalues
        // to std::make_format_args (which expects lvalue refs).
        auto tpl = std::make_tuple(std::forward<Args>(args)...);

        auto formatted = std::apply(
            [&](auto &... a) {
                return std::vformat(fmt, std::make_format_args(a...));
            },
            tpl
        );

        log(level, formatted);
    }

    // Convenience forwarders
    template <typename... Args>
    void debug(std::string_view fmt, Args&&... args) { log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void info(std::string_view fmt, Args&&... args) { log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void warning(std::string_view fmt, Args&&... args) { log(LogLevel::Warning, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void error(std::string_view fmt, Args&&... args) { log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
    template <typename... Args>
    void fatal(std::string_view fmt, Args&&... args) { log(LogLevel::Fatal, fmt, std::forward<Args>(args)...); }

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