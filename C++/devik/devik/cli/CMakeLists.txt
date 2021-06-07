
add_executable(devik_cli
        ${PROJECT_SOURCE_DIR}/apps/cli/main.cpp
        )


target_link_libraries(devik_cli
        PRIVATE
        devik
        )

target_compile_options(devik_cli PRIVATE -Werror)
target_compile_features(devik_cli PRIVATE cxx_std_17)