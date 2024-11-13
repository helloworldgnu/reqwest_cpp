if(NOT DEFINED ENV{QT_HOME} )
    message(FATAL_ERROR "FATAL: You need define QT_HOME with qt in environment.")
elseif(ENV{QT_HOME} STREQUAL "" )
    message(FATAL_ERROR "FATAL: You need set QT_HOME with qt in environment.")
endif()
set(QT_PATH "$ENV{QT_HOME}")

message(STATUS "QT_PATH: ${QT_PATH}")

if(CMAKE_PLATFORM_WINDOWS)
    STRING(REPLACE "\\" "/" QT_PATH ${QT_PATH})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QT_PATH}" )
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT_PATH}" )
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# check dir is exist
function(check_dir_exist dir)
    if(NOT EXISTS "${dir}" OR NOT IS_DIRECTORY "${dir}")
        message(FATAL_ERROR "Qt path [${dir}] not exist")
    endif()
endfunction()
check_dir_exist("${QT_PATH}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CLIENT_LIB_DIR ${CMAKE_BINARY_DIR}/client)

set(SOURCE main.cpp
        main_window.cpp main_window.hpp
        ${LIB_HEADERS}
)
add_executable(gui ${SOURCE})
add_dependencies(gui client)

target_link_libraries(gui
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        crab_http::crab_http
)

if (WIN32)
    target_link_libraries(gui ${CLIENT_LIB_DIR}/crab_http.dll.lib)
elseif (APPLE)
    target_link_libraries(gui ${CLIENT_LIB_DIR}/libcrab_http.dylib)
else ()
    target_link_libraries(gui ${CLIENT_LIB_DIR}/libcrab_http.so)
endif ()

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