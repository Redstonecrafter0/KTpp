#include "Diagnostics.hh"

namespace ktpp::diagnostics {
Diagnostics::Diagnostics(logger::Logger* logger) : logger(logger) {}
void Diagnostics::log(const logger::LogLevel level,
                      const std::string& message) const {
  logger->log(level, message);
}
void Diagnostics::log(const logger::LogLevel level, const std::string& source,
                      const std::string& message) const {
  logger->log(level, source, message);
}

std::string Diagnostics::highlight(const TextSpan& span) const {
  size_t column = span.start - span.line_start;
  std::string position = span.filePath + ":" + std::to_string(span.line) + ":" +
                         std::to_string(column);
  if (!sources.contains(span.filePath)) {
    return position;
  }
  const std::string& source = sources.at(span.filePath);
  size_t lineLength = span.end - span.line_start;
  std::string squiggles = std::string(span.length(), '^');
  std::string spaces = std::string(lineLength - span.length(), ' ');
  std::string code = source.substr(span.line_start, lineLength);
  std::string textHighlight = "\n" + spaces + squiggles;
  return code + textHighlight + "\n" + "[" + position + "] ";
}

void Diagnostics::emit(const logger::LogLevel level, const std::string& message,
                       const TextSpan& span) const {
  const std::string& source = sources.at(span.filePath);
  logger->log(level, highlight(span) + message);
}

void Diagnostics::emit(const logger::LogLevel level, const std::string& source,
                       const std::string& message, const TextSpan& span) const {
  const std::string& filePath = sources.at(span.filePath);
  logger->log(level, source, highlight(span) + message);
}

void Diagnostics::addFile(std::string filePath, std::string source) {
  sources.insert({filePath, source});
}
}  // namespace ktpp::diagnostics