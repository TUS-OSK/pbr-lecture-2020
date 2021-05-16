add_library(renderer INTERFACE)
target_include_directories(renderer INTERFACE "src")
target_compile_features(renderer INTERFACE cxx_std_17)
target_link_libraries(renderer INTERFACE OpenMP::OpenMP_CXX)
target_compile_options(renderer INTERFACE -march=native)

add_executable(pt "pt.cpp")
target_link_libraries(pt PRIVATE renderer)

add_executable(cornell-box "cornell-box.cpp")
target_link_libraries(cornell-box PRIVATE renderer)