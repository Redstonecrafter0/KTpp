#pragma once
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace ktpp::logger {
enum LogLevel { Debug, Info, Warn, Error, Severe };
class Logger {
public:
  virtual void Log(LogLevel level, const std::string &message){};
  virtual void Log(LogLevel level, const std::string &source,
                   const std::string &message){};
  virtual ~Logger() = default;
};

std::unique_ptr<Logger> create_logger(std::ostream *ostream,
                                      LogLevel min_level = LogLevel::Info,
                                      bool colors = true);

std::unique_ptr<Logger> combine_loggers(std::vector<Logger *> loggers);

std::unique_ptr<Logger> sourced_logger(std::unique_ptr<Logger> logger,
                                       const std::string &prefix);

std::string to_string(const LogLevel &level);
std::ostream &operator<<(std::ostream &os, const LogLevel &level);
} // namespace ktpp::logger