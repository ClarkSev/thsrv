
set(BASE_SRC
	Condition.cpp
	Logger.cpp
	LogStream.cpp
	CurrentThread.cpp
	Thread.cpp
	ThreadPool.cpp
	TimeStamp.cpp
)
file(GLOB HEADERS "*.h")

add_library(thsrv_base ${BASE_SRC})
target_link_libraries(thsrv_base rt pthread)

# install(FILES ${HEADERS} DESTINATION thsrv/base)

# message("base-cmake = " ${HEADERS})
