if (MSVC)
    set(LIBS ${LIBS} ws2_32.lib)
else()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
endif(MSVC)
