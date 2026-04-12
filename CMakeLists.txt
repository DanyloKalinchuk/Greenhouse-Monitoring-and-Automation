cmake_minimum_required(VERSION 3.24)
project(MASTER)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SCADA_SRC CONFIGURE_DEPENDS "src/master/scada/*.cpp")
list(FILTER SCADA_SRC EXCLUDE REGEX ".*/tests/.*")

add_executable(scada_process ${SCADA_SRC})
target_link_libraries(scada_process rf24 pthread)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SCADA_TEST_SRC CONFIGURE_DEPENDS "src/master/scada/*.cpp")
list(FILTER SCADA_TEST_SRC EXCLUDE REGEX ".*/scada.cpp*")

enable_testing()
add_executable(scada_tests ${SCADA_TEST_SRC})
target_compile_definitions(scada_tests PRIVATE 
  SAVE_PATH="test_sensors.bin"
  RADIO_LOGS_PATH="test_logs.log"
)
target_link_libraries(scada_tests gtest rf24 pthread)
add_test(NAME ScadaTests COMMAND scada_tests)
