cmake_minimum_required(VERSION 3.0.0)

set(JSON_Install OFF CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)

if(CMAKE_CONFIGURATION_TYPES) # Multi-config generator (Visual Studio)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CONFIG}")
endforeach()
endif()

project(
  ArkhamHorror 
  VERSION 0.1.0 
  LANGUAGES C CXX
)

add_compile_options(/MD)

add_subdirectory(external)

add_library(
  ArkhamHorror_Lib
  OBJECT
  src/card.cpp
)
target_include_directories(
	ArkhamHorror_Lib 
	PRIVATE 
	${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(
  ArkhamHorror_Lib
  PRIVATE
  nlohmann_json::nlohmann_json
  spdlog::spdlog
)

add_executable(
  ArkhamHorror 
  src/main.cpp
)
add_custom_command(
  TARGET ArkhamHorror
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
)

enable_testing()
add_executable(
	ArkhamHorror_Test 
	"tests/test_card.cpp"
	"tests/test_deck.cpp"
	"tests/test_string_to_enum.cpp"
	"tests/test_skill_test.cpp"
)
target_include_directories(
	ArkhamHorror_Test 
	PRIVATE 
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/tests/mock
)
target_link_libraries(
	ArkhamHorror_Test 
	PRIVATE 
	ArkhamHorror_Lib
	gtest_main 
	gmock 
	nlohmann_json::nlohmann_json
  spdlog::spdlog
)
add_test(NAME ArkhamHorror_Test COMMAND ArkhamHorror_Test)