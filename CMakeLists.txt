cmake_minimum_required(VERSION 3.5)
project(pr4test)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# MAkefile includes following compile flags: CFLAGS := -Wall --std=gnu99 -g3 -Wno-format-security -Werror -fsanitize=address -fno-omit-frame-pointer
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --std=gnu99 -g3 -Wno-format-security -Werror ")

set(CLIENT_SOURCE_FILES
        clientmain.c
        shm_channel.c
        )

set(SERVER_SOURCE_FILES
        servermain.c)

#add_executable(pr3 ${SOURCE_FILES})
# I looked at the targets in the MakeFile and reproduced the content here.
# We have 2 exes to build : webproxy and simplecached
add_executable(clientmain ${CLIENT_SOURCE_FILES})
add_executable(servermain ${SERVER_SOURCE_FILES})

# Need to link in pthread explicitly - we do this by using target_link_libraries
#target_link_libraries(webproxy pthread curl)

# The following copies the resulting binaries to the local directory
# CMake builds everything in a temporary folder.
add_custom_command(TARGET clientmain
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:clientmain> ~/ClionProjects/pr4test)

add_custom_command(TARGET servermain
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:servermain> ~/ClionProjects/pr4test)
