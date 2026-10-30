#ifndef __LIBRESER_DATETTIME_HPP__
#define __LIBRESER_DATETTIME_HPP__

#include <chrono>
#include <string>

namespace reser {

struct DateTime {
 public:
  enum class Type {
    OffsetDateTime,
    LocalDateTime,
    LocalDate,
    LocalTime,
  };

  std::string toString() const;
  static bool tryFromString(std::string_view string, DateTime &dateTime);

 private:
  void parseType(std::string_view string);
  bool tryParseAnyChar(std::istringstream &stream, std::string_view chars);
  bool tryParseDate(std::istringstream &stream);
  bool tryParseTime(std::istringstream &stream, bool hasFraction);
  bool tryParseZone(std::istringstream &stream);

  std::chrono::year_month_day date{};
  std::chrono::milliseconds time{};
  std::chrono::minutes offset{};

  Type type{Type::LocalTime};
};

};  // namespace reser

#endif  // __LIBRESER_DATETIME_HPP__
