add_library(renderer INTERFACE)
target_include_directories(renderer INTERFACE "src")
target_compile_features(renderer INTERFACE cxx_std_17)
target_link_libraries(renderer INTERFACE OpenMP::OpenMP_CXX)
target_compile_options(renderer INTERFACE -march=native)

add_executable(spheres "spheres.cpp")
target_link_libraries(spheres PRIVATE renderer)

add_executable(cornell-box "cornell-box.cpp")
target_link_libraries(cornell-box PRIVATE renderer)

add_executable(cornell-box2 "cornell-box2.cpp")
target_link_libraries(cornell-box2 PRIVATE renderer)