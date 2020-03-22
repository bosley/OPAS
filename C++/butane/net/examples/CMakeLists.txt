################################################
#   NETTcpServerExample
################################################

add_executable(NETTcpServerExample
        TcpServerExample.cpp
)

target_link_libraries(NETTcpServerExample
    PUBLIC 
        butane
)

################################################
#   NETUdpServerExample
################################################

add_executable(NETUdpServerExample
        UdpServerExample.cpp
)

target_link_libraries(NETUdpServerExample
    PUBLIC 
        butane
)

################################################
#   NETWriter
################################################

add_executable(NETWriter
        WriterExample.cpp
)

target_link_libraries(NETWriter
    PUBLIC 
        butane
)