file(GLOB DATAHUB_TEST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/client/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests ${DATAHUB_TEST_SOURCE})

target_link_libraries(tests
    datahub-sdk-static
    protobuf.a
    curl.a
    ssl.a
    crypto.a
    lz4.a
    z.a
    gtest
    pthread
    rt
    dl)
