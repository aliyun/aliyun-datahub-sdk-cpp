cmake_minimum_required(VERSION 3.0)

# using gcc4.9.2
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_compile_options(-pthread)
add_compile_options(-D_FILE_OFFSET_BITS=64)
add_compile_options(-g3)
add_compile_options(-fno-strict-aliasing)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-D__STDC_LIMIT_MACROS)
add_compile_options(-D__USE_XOPEN2K8)
add_compile_options(-fPIC)
add_compile_options(-ffunction-sections -fdata-sections)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB EXAMPLE_SOURCES
    ${CMAKE_SOURCE_DIR}/example/client/*.cpp
    ${CMAKE_SOURCE_DIR}/example/datahub/*.cpp
)

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
