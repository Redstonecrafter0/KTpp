#pragma once
#include <unordered_map>

#include "Logger/Logger.hh"

namespace ktpp::diagnostics {
class TextSpan {
 public:
  const std::string filePath;
  const size_t line;
  const size_t line_start;
  const size_t start;
  const size_t end;
  TextSpan(std::string filePath, size_t line, size_t line_start, size_t start,
           size_t end)
      : filePath(filePath),
        line(line),
        line_start(line_start),
        start(start),
        end(end) {}
  size_t length() const { return end - start; }
};

class Diagnostics {
 public:
  Diagnostics(logger::Logger* logger);
  void log(const logger::LogLevel level, const std::string& message) const;
  void log(const logger::LogLevel level, const std::string& source,
           const std::string& message) const;
  void emit(const logger::LogLevel level, const std::string& message,
            const TextSpan& span) const;
  void emit(const logger::LogLevel level, const std::string& source,
            const std::string& message, const TextSpan& span) const;
  std::string highlight(const TextSpan& span) const;
  void addFile(std::string filePath, std::string source);

 private:
  logger::Logger* logger;
  std::unordered_map<std::string, std::string> sources;
};
}  // namespace ktpp::diagnostics