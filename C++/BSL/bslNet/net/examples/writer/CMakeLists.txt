add_executable(NetWriterExample
        example.cpp
)
target_link_libraries(NetWriterExample
        ${LIBRARY_NAME}
        ${BSLCORE_LIBRARIES}
)