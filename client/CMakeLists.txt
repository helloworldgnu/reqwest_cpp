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
    file(GLOB CLIENT_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/target/${TARGET_DIR}/crab_http.*")
elseif (APPLE)
    set(CLIENT_LIB "${CMAKE_CURRENT_SOURCE_DIR}/target/${TARGET_DIR}/libcrab_http.dylib")
else ()
    set(CLIENT_LIB "${CMAKE_CURRENT_SOURCE_DIR}/target/${TARGET_DIR}/libcrab_http.so")
endif ()

message(STATUS "client lib path:${CLIENT_LIB} current binary dir:${CMAKE_CURRENT_BINARY_DIR}")

if (WIN32)
    add_custom_target(client ALL
            COMMENT "Compiling client module"
            COMMAND ${CARGO_CMD}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    foreach (CLIENT_LIB ${CLIENT_LIBS})
        add_custom_command(
                TARGET client POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${CLIENT_LIB} ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Copying ${CLIENT_LIB} to ${CMAKE_CURRENT_BINARY_DIR}"
        )
    endforeach ()
else ()
    add_custom_target(client ALL
            COMMENT "Compiling client module"
            COMMAND ${CARGO_CMD}
            COMMAND ${CMAKE_COMMAND} -E copy ${CLIENT_LIB} ${CMAKE_CURRENT_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
#set(CLIENT_HEADER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/target/crab_http_c.h")
#set(CLIENT_HEADER_DST "${CMAKE_SOURCE_DIR}/gui/include/crab_http_c.h")
#
## 添加一个自定义命令，将client.hpp文件拷贝到gui目录
#add_custom_command(
#        OUTPUT ${CLIENT_HEADER_DST}
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CLIENT_HEADER_SRC} ${CLIENT_HEADER_DST}
#        DEPENDS ${CLIENT_HEADER_SRC}
#        COMMENT "Copying client.hpp to gui directory"
#)

# 添加测试
add_test(NAME client_test
        COMMAND cargo test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})