add_executable(Compressor)
set_target_properties(Compressor PROPERTIES OUTPUT_NAME "compressor")

target_sources(Compressor
  PRIVATE
    src/compressor.cpp
)

find_package(spdlog REQUIRED)

target_link_libraries(Compressor 
  PRIVATE 
    LibCompressor
    spdlog::spdlog
)

install(TARGETS Compressor
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
