add_executable(RegexMatcher)
set_target_properties(RegexMatcher PROPERTIES OUTPUT_NAME "regex-matcher")

target_sources(RegexMatcher
  PRIVATE
    src/matcher.cpp
)

target_link_libraries(RegexMatcher 
  PRIVATE 
    LibRegex
)
