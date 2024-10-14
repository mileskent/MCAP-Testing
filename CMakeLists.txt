cmake_minimum_required(VERSION 3.10)

project(TestMCAPDataSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protobuf REQUIRED)
# find_library(LZ4_LIBRARY lz4 REQUIRED)
# find_library(ZSTD_LIBRARY zstd REQUIRED)


# add_executable(test_mcap_data_sim src/test_mcap_data_sim.cpp)

# target_include_directories(test_mcap_data_sim PRIVATE ${CMAKE_SOURCE_DIR}/mcap)

# # Manually specify where Abseil libraries are located (usually installed system-wide in /usr/lib)
# find_library(ABSL_LOG_INTERNAL_CHECK_OP absl_log_internal_check_op REQUIRED)
# find_library(ABSL_LOG_SEVERITY absl_log_severity REQUIRED)
# find_library(ABSL_LOG_INTERNAL_MESSAGE absl_log_internal_message REQUIRED)
# find_library(ABSL_BASE absl_base REQUIRED)
# find_library(ABSL_STRINGS absl_strings REQUIRED)

# target_link_libraries(test_mcap_data_sim PUBLIC
#     protobuf::libprotobuf
# )

# target_link_libraries(test_mcap_data_sim PRIVATE ${ABSL_LOG_INTERNAL_CHECK_OP} ${ABSL_LOG_SEVERITY} ${ABSL_LOG_INTERNAL_MESSAGE} ${ABSL_BASE} ${ABSL_STRINGS})

# target_link_libraries(test_mcap_data_sim PRIVATE ${LZ4_LIBRARY} ${ZSTD_LIBRARY})

# target_compile_definitions(test_mcap_data_sim PRIVATE MCAP_IMPLEMENTATION)

# install(TARGETS test_mcap_data_sim DESTINATION bin)


# Proto stuff that works when only proto
add_library(socket src/socket.cpp)
add_library(data STATIC src/data.pb.cc)

target_include_directories(socket PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
target_include_directories(data PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

#Link the necessary targets 
target_link_libraries(data PUBLIC protobuf::libprotobuf)
target_link_libraries(socket PUBLIC data)

# Add the server executable DONT DO THIS use targets include
add_executable(server app/server.cpp)
target_link_libraries(server PUBLIC socket)

# Add the client executable
add_executable(client app/client.cpp )
target_link_libraries(client PUBLIC socket)

#install targets!!
install(TARGETS server client DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)