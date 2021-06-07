add_executable(NetTcpExample
        example.cpp
)
target_link_libraries(NetTcpExample
    ${LIBRARY_NAME}
    ${BSLCORE_LIBRARIES}
)