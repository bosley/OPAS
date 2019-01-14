
#####################
# Find CPPUTEST
#####################
if(DEFINED ENV{CPPUTEST_HOME})
    message(STATUS "Using CppUTest home: $ENV{CPPUTEST_HOME}")
    set(CPPUTEST_INCLUDE_DIRS $ENV{CPPUTEST_HOME}/include)
    set(CPPUTEST_LIBRARIES $ENV{CPPUTEST_HOME}/lib)
    set(CPPUTEST_LDFLAGS CppUTest CppUTestExt)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(CPPUTEST REQUIRED cpputest>=3.8)
    message(STATUS "Found CppUTest version ${CPPUTEST_VERSION}")
endif()

#####################
# Add unit test sources
#####################
set(TEST_APP_NAME ${APP_NAME}_tests)
set(TEST_SOURCES
     ${PROJECT_SOURCE_DIR}/CBBus.cpp
     ${PROJECT_SOURCE_DIR}/implementations/tsgp/TSGPBusImpl.cpp
    BusCreationTest.cpp
    TSGPIfTest.cpp
    main.cpp
)

#####################
# Include application libraries to test
#####################
include_directories(${CPPUTEST_INCLUDE_DIRS} 
                    ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/implementations/tsgp
)

#####################
# Link CPPUTEST
#####################
link_directories(${CPPUTEST_LIBRARIES})

#####################
# Add test sources to test
#####################
add_executable(${TEST_APP_NAME} 
               ${TEST_SOURCES}
)

#####################
# Link tests
#####################
target_link_libraries(${TEST_APP_NAME} ${APP_LIB_NAME} ${CPPUTEST_LDFLAGS})

#####################
# Run tests
#####################
add_custom_command(TARGET ${TEST_APP_NAME} COMMAND ./${TEST_APP_NAME} POST_BUILD)
