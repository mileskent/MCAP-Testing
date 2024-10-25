# cmake
cmake_minimum_required(VERSION 3.10)
project(TestMCAPDataSim VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(Protobuf REQUIRED)

# dependency of mcap
find_library(LZ4_LIBRARY lz4 REQUIRED)
find_library(ZSTD_LIBRARY zstd REQUIRED)
find_library(ABSL_LOG_INTERNAL_CHECK_OP absl_log_internal_check_op REQUIRED)
find_library(ABSL_LOG_SEVERITY absl_log_severity REQUIRED)
find_library(ABSL_LOG_INTERNAL_MESSAGE absl_log_internal_message REQUIRED)
find_library(ABSL_BASE absl_base REQUIRED)
find_library(ABSL_STRINGS absl_strings REQUIRED)

# my headers
add_library(socket src/socket.cpp)
add_library(helper src/helper.cpp)
target_include_directories(socket PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(helper PUBLIC ${CMAKE_SOURCE_DIR}/include)

# client
add_executable(client src/client.cpp)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/mcap)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(client PUBLIC
    protobuf::libprotobuf
    socket
    helper
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
)
target_link_libraries(client PRIVATE
    ${ABSL_LOG_INTERNAL_CHECK_OP}
    ${ABSL_LOG_SEVERITY}
    ${ABSL_LOG_INTERNAL_MESSAGE}
    ${ABSL_BASE}
    ${ABSL_STRINGS}
)
target_compile_definitions(client PRIVATE MCAP_IMPLEMENTATION)
install(TARGETS client DESTINATION bin)

# server
add_executable(server src/server.cpp)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/mcap)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(server PUBLIC
    protobuf::libprotobuf
    socket
    helper
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
)
target_link_libraries(server PRIVATE
    ${ABSL_LOG_INTERNAL_CHECK_OP}
    ${ABSL_LOG_SEVERITY}
    ${ABSL_LOG_INTERNAL_MESSAGE}
    ${ABSL_BASE}
    ${ABSL_STRINGS}
)
target_compile_definitions(server PRIVATE MCAP_IMPLEMENTATION)
install(TARGETS server DESTINATION bin)