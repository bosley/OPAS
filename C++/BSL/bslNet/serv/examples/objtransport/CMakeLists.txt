add_executable(ServObjTransport
        example.cpp
)
target_link_libraries(ServObjTransport
    ${LIBRARY_NAME}
    ${BSLCORE_LIBRARIES}
)