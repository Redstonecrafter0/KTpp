#include "Logger/Logger.hh"
#include "stdint.h"
#include <iostream>
#include <sstream>

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

  void log(LogLevel level, const std::string &message,
           size_t line_offset) override {
    if (level < min_level)
      return;
    try {
      std::stringstream logOutput;
      if (colors) {
        logOutput << log_level_color(level);
      }
      std::stringstream header;
      header << "[" << level << "] ";
      logOutput << header.str();
      int header_length = header.str().length() + line_offset;
      std::string messages = message;
      std::string spaces = std::string(header_length, ' ');
      size_t pos = 0;
      std::vector<std::string> lines;
      while ((pos = messages.find('\n')) != std::string::npos) {
        lines.push_back(messages.substr(0, pos));
        messages.erase(0, pos + 1);
      }
      for (const auto &line : lines) {
        logOutput << line << "\n" << spaces;
      }
      logOutput << messages;
      if (colors) {
        logOutput << log_level_reset();
      }
      *ostream << logOutput.str() << std::endl;
    } catch (std::exception e) {
      std::cerr << "Error while logging: " << e.what() << std::endl;
    }
  }

  void log(LogLevel level, const std::string &source,
           const std::string &message, size_t line_offset) override {
    if (level < min_level)
      return;
    log(level, source + ": " + message, line_offset + source.length() + 2);
  }

  ~LogWriter() override { ostream->flush(); }
};

class CombinedLogger : public Logger {
private:
  std::vector<Logger *> loggers;

public:
  CombinedLogger(std::vector<Logger *> loggers) : loggers(loggers) {}
  void log(LogLevel level, const std::string &message,
           size_t line_offset) override {
    for (auto &logger : loggers) {
      logger->log(level, message, line_offset);
    }
  }

  void log(LogLevel level, const std::string &source,
           const std::string &message, size_t line_offset) override {
    for (auto &logger : loggers) {
      logger->log(level, source, message, line_offset);
    }
  }

  ~CombinedLogger() override {
    for (auto &logger : loggers) {
      delete logger;
    }
  }
};

class PrefixedLogger : public Logger {
private:
  std::unique_ptr<Logger> logger;
  std::string prefix;

public:
  PrefixedLogger(std::unique_ptr<Logger> logger, std::string prefix)
      : logger(std::move(logger)), prefix(prefix) {}

  void log(LogLevel level, const std::string &message,
           size_t line_offset) override {
    logger->log(level, prefix, message, line_offset);
  }

  void log(LogLevel level, const std::string &source,
           const std::string &message, size_t line_offset) override {
    logger->log(level, prefix + "." + source, message, line_offset);
  }
};

std::ostream &operator<<(std::ostream &os, const LogLevel &level) {
  switch (level) {
  case LogLevel::Debug:
    return os << "Debug ";
  case LogLevel::Info:
    return os << "Info  ";
  case LogLevel::Warn:
    return os << "Warn  ";
  case LogLevel::Error:
    return os << "Error ";
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

std::unique_ptr<Logger> sourced_logger(std::unique_ptr<Logger> logger,
                                       const std::string &prefix) {
  return std::make_unique<PrefixedLogger>(std::move(logger), prefix);
}
} // namespace ktpp::logger