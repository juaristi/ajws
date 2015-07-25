cmake_minimum_required(VERSION 2.8)

# In CMake 3.0 and above, this call should be:
#    project(ajws LANGUAGES C)
project(ajws C)
set(srcs main.c log.c pcap.c)

add_executable(ajws ${srcs})
target_include_directories(ajws PUBLIC .)
target_link_libraries(ajws "pcap")

