# cargo 设置
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_CMD cargo build)
    set(TARGET_DIR "debug")
else ()
    set(CARGO_CMD cargo build --release)
    set(TARGET_DIR "release")
endif ()

# 根据平台设置动态库后缀
if (WIN32)
    set(CLIENT_LIB "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/client.dll")
elseif (APPLE)
    set(CLIENT_LIB "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/libclient.dylib")
else ()
    set(CLIENT_LIB "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/libclient.so")
endif ()

message(STATUS "client lib path:${CLIENT_LIB} current binary dir:${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(client ALL
        COMMENT "Compiling client module"
        COMMAND CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} ${CARGO_CMD}
        COMMAND ${CMAKE_COMMAND} -E copy ${CLIENT_LIB} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 添加测试
add_test(NAME client_test
        COMMAND cargo test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})