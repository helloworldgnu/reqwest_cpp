set(CMAKE_PREFIX_PATH "${QT_HOME}/lib/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CLIENT_LIB_DIR ${CMAKE_BINARY_DIR}/client)
set(CLIENT_HEADER_SRC "${CMAKE_BINARY_DIR}/client/client.hpp")
set(CLIENT_HEADER_DST "${CMAKE_CURRENT_SOURCE_DIR}/client.hpp")

# 添加一个自定义命令，将client.hpp文件拷贝到gui目录
add_custom_command(
        OUTPUT ${CLIENT_HEADER_DST}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CLIENT_HEADER_SRC} ${CLIENT_HEADER_DST}
        DEPENDS ${CLIENT_HEADER_SRC}
        COMMENT "Copying client.hpp to gui directory"
)

set(SOURCE main.cpp
        main_window.cpp main_window.hpp
        wrappers.cpp wrappers.hpp
        client.hpp
)

add_executable(gui ${SOURCE})

target_link_libraries(gui
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
)

if (WIN32)
    target_link_libraries(gui ${CLIENT_LIB_DIR}/client.dll)
elseif (APPLE)
    target_link_libraries(gui ${CLIENT_LIB_DIR}/libclient.dylib)
else ()
    target_link_libraries(gui ${CLIENT_LIB_DIR}/libclient.so)
endif ()
add_dependencies(gui client)

# 指定在编译gui之前先执行复制任务
add_custom_target(copy_client_header ALL DEPENDS ${CLIENT_HEADER_DST})

# 确保gui依赖于header文件的拷贝
add_dependencies(gui copy_client_header)

# 生成so库
#set(SRC wrappers.cpp wrappers.hpp ffi.hpp)
#add_library(reqwest_cpp SHARED ${SRC})
#target_link_libraries(reqwest_cpp ${CLIENT_DIR}/libclient.dylib)
#add_dependencies(reqwest_cpp client)

# 测试静态链接生成的so库
#set(SRC2 main_window.cpp main_window.hpp wrappers.cpp wrappers.hpp ffi.hpp main.cpp)
#add_executable(test_static ${SRC2})
#target_link_libraries(test_static Qt5::Widgets)
#add_dependencies(test_static reqwest_cpp)
#target_link_libraries(test_static reqwest_cpp)
#add_dependencies(test_static client)
#target_link_libraries(test_static ${CLIENT_DIR}/libclient.a)