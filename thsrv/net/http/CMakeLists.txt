set(
    HTTP_SRC
    HttpServer.cpp
    HttpParse.cpp
    HttpResponse.cpp
)

file(GLOB HEADERS ".h")

add_library(httpserver ${HTTP_SRC})
target_link_libraries(httpserver thsrv_net thsrv_base pthread)

