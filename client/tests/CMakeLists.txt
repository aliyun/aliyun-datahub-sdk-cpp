file(GLOB DATAHUB_CLIENT_TEST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/consumer/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/producer/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests_client ${DATAHUB_CLIENT_TEST_SOURCE})

target_link_libraries(tests_client
    datahub-client-static
    datahub-sdk-static
    protobuf.a
    log4cpp.a
    curl.a
    ssl.a
    crypto.a
    lz4.a
    z.a
    gtest
    pthread
    rt
    dl)
