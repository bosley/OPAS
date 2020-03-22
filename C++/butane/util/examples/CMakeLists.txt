################################################
#   UTILFuzzyBoolean
################################################

add_executable(UTILFuzzyBoolean
        FuzzyBoolean.cpp
)

target_link_libraries(UTILFuzzyBoolean
    PUBLIC 
        butane
)
