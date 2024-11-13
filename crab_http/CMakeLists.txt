project(crab_http)

set(SOURCES
        client.cpp
        client_builder.cpp
        header_map.cpp
        http_exception.cpp
        proxy.cpp
        r_string.cpp
        request.cpp
        request_builder.cpp
        resp_body.cpp
        response.cpp
)

set(HEADERS
        client.h
        client_builder.h
        crab_http.h
        crab_http_c.h
        header_map.h
        http_exception.h
        proxy.h
        r_string.h
        request.h
        request_builder.h
        resp_body.h
        response.h
)

add_library(crab_http STATIC ${SOURCES} ${HEADERS})
add_library(crab_http::crab_http ALIAS crab_http)

target_include_directories(crab_http PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
