
# add_executable(thread_test thread_test.cpp)
# target_link_libraries(thread_test thsrv_base pthread)

add_executable(queue_thread_safe_test queue_thread_safe_test.cpp)
target_link_libraries(queue_thread_safe_test thsrv_base pthread)