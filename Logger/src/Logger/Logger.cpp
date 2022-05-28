#include "Logger/Logger.hh"
#include <iostream>

namespace ktpp::logger {

std::string log_level_color(LogLevel level) {
  switch (level) {
  case LogLevel::Debug:
    return "\033[34m";
  case LogLevel::Info:
    return "\033[32m";
  case LogLevel::Warn:
    return "\033[33m";
  case LogLevel::Error:
    return "\033[1m\033[31m";
  case LogLevel::Severe:
    return "\033[1m\033[35m";
  }
}

std::string log_level_reset() { return "\033[0m"; }

class LogWriter : public Logger {
private:
  std::ostream *ostream;
  LogLevel min_level;
  bool colors;

public:
  LogWriter(std::ostream *ostream, LogLevel min_level, bool colors)
      : ostream(ostream), min_level(min_level), colors(colors) {}

  void Log(LogLevel level, const std::string &message) override {
    if (level < min_level)
      return;
    try {
      if (colors) {
        *ostream << log_level_color(level);
      }
      *ostream << "[" << level << "] " << message;
      if (colors) {
        *ostream << log_level_reset();
      }
      *ostream << std::endl;
    } catch (std::exception e) {
      std::cerr << "Error while logging: " << e.what() << std::endl;
    }
  }

  ~LogWriter() override { ostream->flush(); }
};

class CombinedLogger : public Logger {
private:
  std::vector<Logger *> loggers;

public:
  CombinedLogger(std::vector<Logger *> loggers) : loggers(loggers) {}
  void Log(LogLevel level, const std::string &message) override {
    for (auto &logger : loggers) {
      logger->Log(level, message);
    }
  }

  ~CombinedLogger() override {
    for (auto &logger : loggers) {
      delete logger;
    }
  }
};

std::ostream &operator<<(std::ostream &os, const LogLevel &level) {
  switch (level) {
  case LogLevel::Debug:
    return os << "Debug";
  case LogLevel::Info:
    return os << "Info";
  case LogLevel::Warn:
    return os << "Warn";
  case LogLevel::Error:
    return os << "Error";
  case LogLevel::Severe:
    return os << "Severe";
  }
};

std::unique_ptr<Logger> create_logger(std::ostream *ostream, LogLevel min_level,
                                      bool colors) {
  return std::make_unique<LogWriter>(ostream, min_level, colors);
};

std::unique_ptr<Logger> combine_loggers(std::vector<Logger *> loggers) {
  return std::make_unique<CombinedLogger>(loggers);
}
} // namespace ktpp::logger