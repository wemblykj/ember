#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

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

    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message) { log(LogLevel::Debug, message); }
    void info(const std::string& message) { log(LogLevel::Info, message); }
    void warning(const std::string& message) { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error, message); }
    void fatal(const std::string& message) { log(LogLevel::Fatal, message); }

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

// Convenience macros
#define EMBER_LOG_DEBUG(msg) ember::core::Logger::instance().debug(msg)
#define EMBER_LOG_INFO(msg) ember::core::Logger::instance().info(msg)
#define EMBER_LOG_WARN(msg) ember::core::Logger::instance().warning(msg)
#define EMBER_LOG_ERROR(msg) ember::core::Logger::instance().error(msg)
#define EMBER_LOG_FATAL(msg) ember::core::Logger::instance().fatal(msg)
