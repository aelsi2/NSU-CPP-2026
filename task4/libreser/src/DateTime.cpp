#include "LibReser/DateTime.hpp"

#include <chrono>
#include <sstream>

using namespace reser;

std::string DateTime::toString() const {
  using namespace std::chrono;

  switch (type) {
    case Type::OffsetDateTime:
      if (offset == minutes::zero()) {
        return std::format("{:%FT%T}Z", local_time<milliseconds>(local_days(date) + time));
      } else {
        char sign = offset > minutes::zero() ? '+' : '-';
        return std::format("{:%FT%T}{}{:%R}", local_time<milliseconds>(local_days(date) + time), sign, abs(offset));
      }
    case Type::LocalDateTime:
      return std::format("{:%FT%T}", local_time<milliseconds>(local_days(date) + time));
    case Type::LocalDate:
      return std::format("{:%F}", date);
      break;
    case Type::LocalTime:
      return std::format("{:%T}", time);
    default:
      throw std::logic_error("Invalid DateTime::Type");
  }
}

void DateTime::parseType(std::string_view string) {
  const auto monthPart = string.find('-');
  if (monthPart == std::string_view::npos) {
    type = Type::LocalTime;
    return;
  }
  const auto datePart = string.find('-', monthPart + 1);
  if (datePart == std::string_view::npos) {
    type = Type::LocalTime;
    return;
  }
  const auto timePart = string.find(':', datePart + 1);
  if (timePart == std::string_view::npos) {
    type = DateTime::Type::LocalDate;
    return;
  }
  const auto zonePart = string.find_first_of("Zz+-", timePart + 1);
  if (zonePart == std::string_view::npos) {
    type = DateTime::Type::LocalDateTime;
    return;
  }
  type = DateTime::Type::OffsetDateTime;
}

bool DateTime::tryParseAnyChar(std::istringstream &stream, std::string_view chars) {
  if (stream.peek() == std::char_traits<char>::eof()) {
    return false;
  }
  if (chars.find(stream.peek()) != chars.npos) {
    stream.get();
    return true;
  }
  return false;
}

bool DateTime::tryParseDate(std::istringstream &stream) {
  const auto start = stream.tellg();

  if (stream >> std::chrono::parse("%F", date) && date.ok()) {
    return true;
  }

  stream.clear();
  stream.seekg(start);

  return false;
}

bool DateTime::tryParseTime(std::istringstream &stream, bool hasFraction) {
  const auto start = stream.tellg();

  if (stream >> std::chrono::parse("%T", time) && time < std::chrono::hours(24)) {
    if (hasFraction) {
      while (stream.peek() != std::char_traits<char>::eof() && std::isdigit(stream.peek(), std::locale::classic())) {
        stream.get();
      }
    }
    return true;
  }

  stream.clear();
  stream.seekg(start);

  if (stream >> std::chrono::parse("%R", time) && time < std::chrono::hours(24)) {
    return true;
  }

  stream.clear();
  stream.seekg(start);

  return false;
}

bool DateTime::tryParseZone(std::istringstream &stream) {
  int sign;
  switch (stream.peek()) {
    case 'Z':
    case 'z':
      stream.get();
      offset = std::chrono::minutes::zero();
      return true;
    case '+':
      sign = 1;
      break;
    case '-':
      sign = -1;
      break;
    default:
      return false;
  }

  const auto start = stream.tellg();
  stream.get();

  if (stream >> std::chrono::parse("%R", offset) && offset < std::chrono::hours(24)) {
    offset *= sign;
    return true;
  }

  stream.clear();
  stream.seekg(start);

  return false;
}

static bool hasSecondsFraction(std::string_view string) { return string.find('.') != string.npos; }

bool DateTime::tryFromString(std::string_view string, DateTime &result) {
  DateTime dateTime;

  std::istringstream stream((std::string(string)));
  stream.imbue(std::locale::classic());
  dateTime.parseType(string);

  bool success;
  // clang-format off
  switch (dateTime.type) {
    case Type::OffsetDateTime:
      success = dateTime.tryParseDate(stream) && 
                dateTime.tryParseAnyChar(stream, "tT ") &&
                dateTime.tryParseTime(stream, hasSecondsFraction(string)) && 
                dateTime.tryParseZone(stream) && 
                stream.peek() == std::char_traits<char>::eof();
      break;
    case Type::LocalDateTime:
      success = dateTime.tryParseDate(stream) && 
                dateTime.tryParseAnyChar(stream, "tT ") &&
                dateTime.tryParseTime(stream, hasSecondsFraction(string)) && 
                stream.peek() == std::char_traits<char>::eof();
      break;
    case Type::LocalDate:
      success = dateTime.tryParseDate(stream) && 
                stream.peek() == std::char_traits<char>::eof();
      break;
    case Type::LocalTime:
      success = dateTime.tryParseTime(stream, hasSecondsFraction(string)) && 
                stream.peek() == std::char_traits<char>::eof();
      break;
    default:
      success = false;
      break;
  }
  // clang-format on
  if (success) {
    result = std::move(dateTime);
  }
  return success;
}
