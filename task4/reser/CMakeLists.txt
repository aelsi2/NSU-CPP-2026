add_executable(Reserializer)
set_target_properties(Reserializer PROPERTIES OUTPUT_NAME "reser")

target_sources(Reserializer
  PRIVATE
    src/main.cpp
)

target_link_libraries(Reserializer 
  PRIVATE 
    LibReser
)
