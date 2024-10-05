cmake_minimum_required(VERSION 3.10)

# Project name
project(TestMCAPDataSim VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable (your .cpp file)
add_executable(test_mcap_data_sim test_mcap_data_sim.cpp)

# Add include directories for your local "mcap" folder
target_include_directories(test_mcap_data_sim PRIVATE ${CMAKE_SOURCE_DIR}/mcap)

# Find and link necessary libraries
find_package(Protobuf REQUIRED)
find_library(LZ4_LIBRARY lz4 REQUIRED)
find_library(ZSTD_LIBRARY zstd REQUIRED)

# Manually specify where Abseil libraries are located (usually installed system-wide in /usr/lib)
find_library(ABSL_LOG_INTERNAL_CHECK_OP absl_log_internal_check_op REQUIRED)
find_library(ABSL_LOG_SEVERITY absl_log_severity REQUIRED)
find_library(ABSL_LOG_INTERNAL_MESSAGE absl_log_internal_message REQUIRED)
find_library(ABSL_BASE absl_base REQUIRED)
find_library(ABSL_STRINGS absl_strings REQUIRED)

# Link Protocol Buffers
target_link_libraries(test_mcap_data_sim PRIVATE ${PROTOBUF_LIBRARIES})

# Link Abseil libraries
target_link_libraries(test_mcap_data_sim PRIVATE ${ABSL_LOG_INTERNAL_CHECK_OP} ${ABSL_LOG_SEVERITY} ${ABSL_LOG_INTERNAL_MESSAGE} ${ABSL_BASE} ${ABSL_STRINGS})

# Link LZ4 and Zstd
target_link_libraries(test_mcap_data_sim PRIVATE ${LZ4_LIBRARY} ${ZSTD_LIBRARY})

# Add definitions (if needed, for example MCAP_IMPLEMENTATION)
target_compile_definitions(test_mcap_data_sim PRIVATE MCAP_IMPLEMENTATION)
