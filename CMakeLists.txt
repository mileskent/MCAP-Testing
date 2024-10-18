cmake_minimum_required(VERSION 3.10)

project(TestMCAPDataSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protobuf REQUIRED)

find_library(LZ4_LIBRARY lz4 REQUIRED)
find_library(ZSTD_LIBRARY zstd REQUIRED)

add_executable(test_mcap_data_sim src/test_mcap_data_sim.cpp)

target_include_directories(test_mcap_data_sim PRIVATE ${CMAKE_SOURCE_DIR}/mcap)

find_library(ABSL_LOG_INTERNAL_CHECK_OP absl_log_internal_check_op REQUIRED)
find_library(ABSL_LOG_SEVERITY absl_log_severity REQUIRED)
find_library(ABSL_LOG_INTERNAL_MESSAGE absl_log_internal_message REQUIRED)
find_library(ABSL_BASE absl_base REQUIRED)
find_library(ABSL_STRINGS absl_strings REQUIRED)

target_link_libraries(test_mcap_data_sim PUBLIC
    protobuf::libprotobuf
)

target_link_libraries(test_mcap_data_sim PRIVATE ${ABSL_LOG_INTERNAL_CHECK_OP} ${ABSL_LOG_SEVERITY} ${ABSL_LOG_INTERNAL_MESSAGE} ${ABSL_BASE} ${ABSL_STRINGS})

target_link_libraries(test_mcap_data_sim PRIVATE ${LZ4_LIBRARY} ${ZSTD_LIBRARY})
target_link_libraries(test_mcap_data_sim PUBLIC socket)

target_compile_definitions(test_mcap_data_sim PRIVATE MCAP_IMPLEMENTATION)

install(TARGETS test_mcap_data_sim DESTINATION bin)

add_library(socket src/socket.cpp)

target_include_directories(socket PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

add_executable(server src/server.cpp)
target_link_libraries(server PUBLIC socket protobuf::libprotobuf) 

install(TARGETS server DESTINATION bin)
