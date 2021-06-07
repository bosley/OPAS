################################################ 
#   Add examples 
################################################

add_subdirectory(tcp)

if(BUILD_THREADED)
    add_subdirectory(threadedTcp)
endif()

add_subdirectory(udp)
add_subdirectory(writer)