#####################
# Add example sources
#####################
add_compile_options(-std=c++11 -Wall -Werror -pthread)
set(EXAMPLE_APP_NAME ${APP_NAME}_examples)
set(EXAMPLE_SOURCES
     ${PROJECT_SOURCE_DIR}/CBBus.cpp
     ${PROJECT_SOURCE_DIR}/implementations/tsgp/TSGPBusImpl.cpp
     TSGP_example.cpp
)

#####################
# Include application libraries to test
#####################
include_directories(${CPPUEXAMPLE_INCLUDE_DIRS} 
                    ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/implementations/tsgp
)

link_directories(${CPPUEXAMPLE_LIBRARIES})

#####################
# Add example sources
#####################
add_executable(${EXAMPLE_APP_NAME} 
               ${EXAMPLE_SOURCES}
)

#####################
# Link tests
#####################
target_link_libraries(${EXAMPLE_APP_NAME} ${APP_LIB_NAME})
