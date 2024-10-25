cmake_minimum_required(VERSION 3.10)

project(TestMCAPDataSim VERSION 1.0 LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find required packages and libraries
find_package(Protobuf REQUIRED)

find_library(LZ4_LIBRARY lz4 REQUIRED)
find_library(ZSTD_LIBRARY zstd REQUIRED)
find_library(ABSL_LOG_INTERNAL_CHECK_OP absl_log_internal_check_op REQUIRED)
find_library(ABSL_LOG_SEVERITY absl_log_severity REQUIRED)
find_library(ABSL_LOG_INTERNAL_MESSAGE absl_log_internal_message REQUIRED)
find_library(ABSL_BASE absl_base REQUIRED)
find_library(ABSL_STRINGS absl_strings REQUIRED)


# Add libraries
add_library(socket SHARED src/socket.cpp)
add_library(helper SHARED src/helper.cpp)

# Specify include directories for socket and helper
target_include_directories(socket PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(helper PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Add executable for test_mcap_data_sim
add_executable(test_mcap_data_sim src/test_mcap_data_sim.cpp)

# Link libraries and directories for test_mcap_data_sim
target_include_directories(test_mcap_data_sim PRIVATE ${CMAKE_SOURCE_DIR}/mcap)
target_include_directories(test_mcap_data_sim PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link external and internal libraries
target_link_libraries(test_mcap_data_sim PUBLIC
    protobuf::libprotobuf
    socket
    helper
    ${LZ4_LIBRARY}
    ${ZSTD_LIBRARY}
)

target_link_libraries(test_mcap_data_sim PRIVATE
    ${ABSL_LOG_INTERNAL_CHECK_OP}
    ${ABSL_LOG_SEVERITY}
    ${ABSL_LOG_INTERNAL_MESSAGE}
    ${ABSL_BASE}
    ${ABSL_STRINGS}
)

# Add compile definitions
target_compile_definitions(test_mcap_data_sim PRIVATE MCAP_IMPLEMENTATION)

# Install test_mcap_data_sim binary
install(TARGETS test_mcap_data_sim DESTINATION bin)

# Add executable for server
add_executable(server src/server.cpp)

# Link libraries and directories for server
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/mcap)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(server PUBLIC socket helper protobuf::libprotobuf)

# Install server binary
install(TARGETS server DESTINATION bin)