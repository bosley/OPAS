add_executable(NetTcpThreadedExample
        example.cpp
)
target_link_libraries(NetTcpThreadedExample
    ${LIBRARY_NAME}
    ${BSLCORE_LIBRARIES}
)