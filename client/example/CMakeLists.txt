############################################################
# General
############################################################
cmake_minimum_required(VERSION 3.5)

set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ ${CMAKE_EXE_LINKER_FLAGS}")

file(GLOB EXAMPLE_SOURCES *.cpp *.c)
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(TARGET_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${TARGET_NAME} ${EXAMPLE_SOURCE})
    target_link_libraries(${TARGET_NAME}
        datahub-client-static
        datahub-sdk-static
        liblog4cpp.a
        libprotobuf.a
        libcurl.a
        libssl.a
        libcrypto.a
        libz.a
        lz4.a
        pthread
        rt
        dl
    )
endforeach()
