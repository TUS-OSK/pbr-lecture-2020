cmake_minimum_required(VERSION 3.12)

project(pbr-lecture-2020 LANGUAGES CXX)

option(BUILD_REFERENCE "build reference raytracer" ON)

# openMP
find_package(OpenMP)

# main
add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_include_directories(main PRIVATE src)

if(OpenMP_CXX_FOUND)
  target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
else()
  message(WARNING "failed to find OpenMP")
endif()

# reference
if(BUILD_REFERENCE)
  add_subdirectory(ref)
endif()
