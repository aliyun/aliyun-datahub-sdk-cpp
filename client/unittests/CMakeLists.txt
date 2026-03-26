############################################################
# datahub_client_unittest
############################################################

file(GLOB DATAHUB_CLIENT_UNITTEST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/consumer/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/producer/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ./gmock/)

add_executable(unit_tests ${DATAHUB_CLIENT_UNITTEST_SOURCE})

target_link_libraries(unit_tests
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
    gmock
    pthread
    rt
    dl)