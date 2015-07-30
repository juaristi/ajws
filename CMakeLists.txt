cmake_minimum_required (VERSION 2.8)

# In CMake 3.0 and above, this call should be:
#    project(ajws LANGUAGES C)
project (ajws C)

if (NOT DEFINED AJWS_HANDLER)
    # The default AJWS_HANDLER is libpcap
    set (AJWS_HANDLER "pcap")
endif ()

message (STATUS "Using handler '${AJWS_HANDLER}'.")
message (STATUS "System: ${CMAKE_SYSTEM_NAME} on ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "Compiler flags: ${CMAKE_C_FLAGS}")

set (srcs main.c log.c dev.c)
if (AJWS_HANDLER STREQUAL "pcap")
    list (APPEND srcs pcap.c)
elseif (AJWS_HANDLER STREQUAL "pfring")
    list (APPEND srcs pfring.c)
endif ()

add_executable (ajws ${srcs})
target_include_directories (ajws PUBLIC ${CMAKE_SOURCE_DIR})

if (AJWS_HANDLER STREQUAL "pcap")
    target_link_libraries (ajws "pcap")
elseif (AJWS_HANDLER STREQUAL "pfring")
    target_link_libraries (ajws "pfring")
endif ()

