set(NET_SRC
	Acceptor.cpp
	Buffer.cpp
	Channel.cpp
	Connector.cpp
	EventLoop.cpp
	InetAddress.cpp
	Poller.cpp
	Socket.cpp
	TcpClient.cpp
	TcpConnection.cpp
	TcpServer.cpp
)
file(GLOB HEADERS "*.h")

# set(HEADERS
	# Acceptor.h
	# Buffer.h
	# Callback.h
	# Channel.h
	# EventLoop.h
	# InetAddress.h
	# Poller.h
	# Socket.h
	# TcpConnection.h
	# TcpServer.h
# )
# install(FILES ${HEADERS} DESTINATION include/muduo/net)

# message("net headers = " ${HEADERS})

add_library(thsrv_net ${NET_SRC})
target_link_libraries(thsrv_net thsrv_base)