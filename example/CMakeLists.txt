############################################################
# General
############################################################
cmake_minimum_required(VERSION 3.5)

set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99")

file(GLOB EXAMPLE_SOURCES *.cpp *.c)
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(TARGET_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${TARGET_NAME} ${EXAMPLE_SOURCE})
    target_link_libraries(${TARGET_NAME}
        datahub-sdk-static
        protobuf.a
        curl.a
        ssl.a
        crypto.a
        z.a
        lz4.a
        pthread
        rt
        dl
    )
endforeach()
