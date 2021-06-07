add_executable(ViewerDemo
        example.cpp
)

target_link_libraries(ViewerDemo
    PRIVATE 
        ${OpenCV_LIBS}
        ${BSLCORE_LIBRARIES}
        ${LIBRARY_NAME}
)