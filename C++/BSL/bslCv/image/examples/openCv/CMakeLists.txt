add_executable(OpenCvBasic
        example.cpp
)

target_link_libraries(OpenCvBasic
    PRIVATE 
        ${OpenCV_LIBS}
)