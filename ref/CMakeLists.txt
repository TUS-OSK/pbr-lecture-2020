add_executable(rtao rtao.cpp)
target_compile_features(rtao PRIVATE cxx_std_17)
target_include_directories(rtao PRIVATE src)
target_link_libraries(rtao PRIVATE OpenMP::OpenMP_CXX)

add_executable(pt pt.cpp)
target_compile_features(pt PRIVATE cxx_std_17)
target_include_directories(pt PRIVATE src)
target_link_libraries(pt PRIVATE OpenMP::OpenMP_CXX)