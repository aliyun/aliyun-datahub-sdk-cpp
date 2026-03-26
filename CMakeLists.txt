############################################################
# datahub-cpp-sdk
############################################################

cmake_minimum_required(VERSION 3.5)

#system processor will set by project, so set first
execute_process(
    COMMAND bash -c "uname -m"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_C_COMPILER ${CMAKE_SOURCE_DIR}/thirdparty/${CMAKE_SYSTEM_PROCESSOR}/gcc/bin/gcc)
set(CMAKE_CXX_COMPILER ${CMAKE_SOURCE_DIR}/thirdparty/${CMAKE_SYSTEM_PROCESSOR}/gcc/bin/g++)

project(datahub-cpp-sdk)
enable_testing()

############################################################
# Globale Variables
############################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

############################################################
# Compile & Link Options
############################################################

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

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    add_compile_options(-march=armv8-a+crc)
else()
    add_compile_options(-m64)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

execute_process(
    COMMAND bash -c "ld -v | sed 's#[^2]*2\\.#2\\.#' | awk -F . '{printf(\"%02d%02d\", $1,$2)}'"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE LD_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

############################################################
# paths
############################################################
set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/${CMAKE_SYSTEM_PROCESSOR}/ CACHE STRING "")

if (${LD_VERSION} GREATER 0221)
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,--copy-dt-needed-entries -Wl,-rpath ${THIRD_PARTY_PATH}")
else()
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath-link ${THIRD_PARTY_PATH}")
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-Wl,--gc-sections -Wl,-z,defs -Wl,--exclude-libs,ALL -Wl,-export-dynamic")
else()
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-Wl,--gc-sections -Wl,-z,defs -Wl,--exclude-libs,ALL -Wl,-export-dynamic -m64")
endif()

############################################################
set(PACKAGE_ROOT_PATH ${CMAKE_SOURCE_DIR}/package)
set(PROTO_ROOT_PATH ${CMAKE_SOURCE_DIR})
set(PROTOBUF_INCLUDE_DIR ${THIRD_PARTY_PATH}/include)
set(PROTOBUF_PROTOC_EXECUTABLE ${THIRD_PARTY_PATH}/bin/protoc)

############################################################
# Functions
############################################################
function(PROTOBUF_GENERATE_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
        return()
    endif()

    if(NOT PROTO_ROOT_PATH)
        message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() PROTO_ROOT_PATH is not set")
        return()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        get_filename_component(FIL_DIR ${FIL} DIRECTORY)

        file(RELATIVE_PATH REL ${PROTO_ROOT_PATH} ${FIL_DIR})

        list(APPEND ${SRCS} "${CMAKE_BINARY_DIR}/${REL}/${FIL_WE}.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_BINARY_DIR}/${REL}/${FIL_WE}.pb.h")

        add_custom_command(
            OUTPUT "${CMAKE_BINARY_DIR}/${REL}/${FIL_WE}.pb.cc"
                   "${CMAKE_BINARY_DIR}/${REL}/${FIL_WE}.pb.h"
            COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
            ARGS --cpp_out  ${CMAKE_BINARY_DIR} -I${PROTO_ROOT_PATH} ${ABS_FIL}
            DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE}
            COMMENT "Running C++ protocol buffer compiler on ${FIL}"
            VERBATIM )
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

function(CREATE_PACKAGE TARGET_NAME PACKAGE_NAME)
    if(NOT PACKAGE_ROOT_PATH)
        message(SEND_ERROR "Error: CREATE_PACKAGE() PACKAGE_ROOT_PATH is not set")
        return()
    endif()

    set(get_link "grep -E \"thirdparty\" | grep \".so\" | awk '{print $3}'")
    set(depends ${EXT_PACKAGE_DEPENDS})
    set(STATE "flag")
    foreach(ARG ${ARGN})
        if(${STATE} STREQUAL "flag")
            if(${ARG} STREQUAL "TARGETS" OR ${ARG} STREQUAL "RTARGETS"
                OR ${ARG} STREQUAL "LINKLIB" OR ${ARG} STREQUAL "FILE_LINK"
                OR ${ARG} STREQUAL "FILE_PATH" OR ${ARG} STREQUAL "DIR_PATH")
                set(FLAG ${ARG})
            else()
                message(SEND_ERROR "Error: CREATE_PACKAGE invalid flag ${ARG}")
                return()
            endif()
            set(STATE "src")
        elseif(${STATE} STREQUAL "src")
            set(SRC ${ARG})
            if (NOT ${FLAG} STREQUAL "DIR_PATH")
                set(depends ${depends} ${ARG})
            endif()
            set(STATE "dst")
        else()
            set(STATE "flag")
            set(IN_PACKAGE_PATH ${ARG})
            set(DEST_PATH ${PACKAGE_ROOT_PATH}/tmp/${PACKAGE_NAME}/${IN_PACKAGE_PATH})
            if(${FLAG} STREQUAL "TARGETS")
                set(DEBUG_EXT ".debug")
                get_filename_component(FILE_NAME ${SRC} NAME)
                set(dump_debug_info "${CMAKE_OBJCOPY} --only-keep-debug ${FILE_NAME} ${FILE_NAME}${DEBUG_EXT}")
                set(strip_bin_debug "${CMAKE_OBJCOPY} --strip-debug ${FILE_NAME}")
                set(link_debug_info "${CMAKE_OBJCOPY} --add-gnu-debuglink=${FILE_NAME}${DEBUG_EXT} ${FILE_NAME}")
                set(COPY_TARGETS ${COPY_TARGETS} COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SRC}> ${DEST_PATH}/$<TARGET_FILE_NAME:${SRC}>)
                set(COPY_TARGETS ${COPY_TARGETS} COMMAND sh -c "cd ${DEST_PATH} && ${dump_debug_info} && ${strip_bin_debug} && ${link_debug_info}")
            elseif(${FLAG} STREQUAL "RTARGETS")
                set(COPY_RTARGETS ${COPY_RTARGETS} COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SRC}> ${DEST_PATH}/$<TARGET_FILE_NAME:${SRC}>)
            elseif(${FLAG} STREQUAL "LINKLIB")
                set(COPY_LINKS ${COPY_LINKS} COMMAND sh -c "mkdir -p ${DEST_PATH} && ldd $<TARGET_FILE:${SRC}> | ${get_link} | xargs -i cp -u {} ${DEST_PATH}")
            elseif(${FLAG} STREQUAL "FILE_PATH")
                get_filename_component(FILE_NAME ${SRC} NAME)
                set(COPY_FILES ${COPY_FILES} COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DEST_PATH}/${FILE_NAME})
            elseif(${FLAG} STREQUAL "FILE_LINK")
                get_filename_component(FILE_NAME ${SRC} NAME)
                set(COPY_FILES ${COPY_FILES} COMMAND ${CMAKE_COMMAND} -E create_symlink ${FILE_NAME} ${DEST_PATH})
            else()
                get_filename_component(DIR_NAME ${SRC} NAME)
                set(COPY_DIRS ${COPY_DIRS} COMMAND ${CMAKE_COMMAND} -E copy_directory ${SRC} ${DEST_PATH}/${DIR_NAME})
            endif()
        endif()
    endforeach()

    set(INPUT_DIR ${PACKAGE_ROOT_PATH}/tmp/${PACKAGE_NAME})
    set(OUTPUT_PATH ${PACKAGE_ROOT_PATH}/${PACKAGE_NAME}.tar.gz)

    add_custom_target(
        ${TARGET_NAME}
        ${COPY_TARGETS}
        ${COPY_RTARGETS}
        ${COPY_LINKS}
        ${COPY_FILES}
        ${COPY_DIRS}
        COMMAND ${CMAKE_COMMAND} -E chdir "${INPUT_DIR}" tar "czvf" "${OUTPUT_PATH}" "."
        DEPENDS ${depends}
        COMMENT "Creating package ${PACKAGE_NAME}"
        VERBATIM)
endfunction()

############################################################
# datahub-cpp-sdk
############################################################

include_directories(
    ./
    ${THIRD_PARTY_PATH}/include/
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_BINARY_DIR}/
    ${CMAKE_CURRENT_BINARY_DIR}/include
    )

link_directories(
    ${THIRD_PARTY_PATH}/lib/
    )

file(GLOB SDK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model/*.cpp)

file(GLOB PROTO_FILES
    ${CMAKE_SOURCE_DIR}/proto/*.proto)

PROTOBUF_GENERATE_CPP(PROTO_SOURCES PROTO_HEADERS ${PROTO_FILES})

add_library(datahub-sdk SHARED ${SDK_SOURCES} ${PROTO_SOURCES})
add_library(datahub-sdk-static STATIC ${SDK_SOURCES} ${PROTO_SOURCES})

# The version number.
set(SDK_VERSION_MAJOR 2)
set(SDK_VERSION_MINOR 25)
set(SDK_VERSION_PATCH 0)
set(SDK_VERSION ${SDK_VERSION_MAJOR}.${SDK_VERSION_MINOR}.${SDK_VERSION_PATCH})

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/include/datahub/sdk_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/datahub/sdk_version.h
)

set_target_properties(
    datahub-sdk PROPERTIES
    VERSION   ${SDK_VERSION}
    SOVERSION ${SDK_VERSION_MAJOR}
)

target_link_libraries(datahub-sdk
    protobuf
    curl
    ssl
    crypto
    lz4
    z
    pthread
    )

############################################################
# datahub-cpp-client
############################################################

file(GLOB CLIENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/client/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/client/src/*.cpp)

include_directories(client/example client/include client/src)

add_library(datahub-client SHARED ${CLIENT_SOURCES} ${PROTO_SOURCES})
add_library(datahub-client-static STATIC ${CLIENT_SOURCES} ${PROTO_SOURCES})

add_dependencies(datahub-client datahub-sdk)
add_dependencies(datahub-client-static datahub-sdk-static)

set_target_properties(
    datahub-client PROPERTIES
    VERSION   ${SDK_VERSION}
    SOVERSION ${SDK_VERSION_MAJOR}
)

target_link_libraries(datahub-client datahub-sdk log4cpp)

add_subdirectory(example)
add_subdirectory(tests)
add_subdirectory(client)

############################################################
# create sdk package
############################################################

set(PACKAGE_FILES
    RTARGETS datahub-sdk-static                                                             lib
    RTARGETS datahub-sdk                                                                    lib
    LINKLIB  datahub-sdk                                                                    lib
    RTARGETS datahub-client-static                                                          lib
    RTARGETS datahub-client                                                                 lib
    LINKLIB  datahub-client                                                                 lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/libcurl.a                                             lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/liblz4.a                                              lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/libz.a                                                lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/libssl.a                                              lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/libcrypto.a                                           lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/libprotobuf.a                                         lib
    FILE_PATH ${THIRD_PARTY_PATH}/lib/liblog4cpp.a                                          lib
    FILE_PATH ${CMAKE_SOURCE_DIR}/example/readme                                            example
    FILE_PATH ${CMAKE_SOURCE_DIR}/example/example_c.c                                       example/datahub
    FILE_PATH ${CMAKE_SOURCE_DIR}/example/example_cpp.cpp                                   example/datahub
    FILE_PATH ${CMAKE_SOURCE_DIR}/example/example_blob.cpp                                  example/datahub
    FILE_PATH ${CMAKE_SOURCE_DIR}/example/example_pub_sub.cpp                               example/datahub
    FILE_PATH ${CMAKE_SOURCE_DIR}/client/example/example_general_produce.cpp                example/client
    FILE_PATH ${CMAKE_SOURCE_DIR}/client/example/example_async_produce.cpp                  example/client
    FILE_PATH ${CMAKE_SOURCE_DIR}/client/example/example_collaborative_consume.cpp          example/client
    FILE_PATH ${CMAKE_SOURCE_DIR}/makefile/CMakeLists.txt                                   .
    DIR_PATH  ${CMAKE_SOURCE_DIR}/include                                                   .
    DIR_PATH  ${CMAKE_SOURCE_DIR}/client/include                                            .
    DIR_PATH  ${THIRD_PARTY_PATH}/include/rapidjson                                         include
    FILE_PATH ${CMAKE_CURRENT_BINARY_DIR}/include/datahub/sdk_version.h                     include/datahub
    FILE_LINK ${CMAKE_BINARY_DIR}/libdatahub-sdk.so.${SDK_VERSION_MAJOR}                    lib/libdatahub-sdk.so
    FILE_LINK ${CMAKE_BINARY_DIR}/libdatahub-sdk.so.${SDK_VERSION}                          lib/libdatahub-sdk.so.${SDK_VERSION_MAJOR}
    FILE_LINK ${CMAKE_BINARY_DIR}/libdatahub-client.so.${SDK_VERSION_MAJOR}                 lib/libdatahub-client.so
    FILE_LINK ${CMAKE_BINARY_DIR}/libdatahub-client.so.${SDK_VERSION}                       lib/libdatahub-client.so.${SDK_VERSION_MAJOR}
    )

set(EXT_PACKAGE_DEPENDS)
create_package("sdk" "datahub_cpp_sdk_release_64" ${PACKAGE_FILES})
