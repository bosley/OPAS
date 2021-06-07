add_executable(NetUdpExample
        example.cpp
)
target_link_libraries(NetUdpExample
        ${LIBRARY_NAME}
        ${BSLCORE_LIBRARIES}
)