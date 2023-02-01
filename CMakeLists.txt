cmake_minimum_required(VERSION 3.22)
project(cocotools)

set(CMAKE_CXX_STANDARD 17)
#add_compile_options(-Wall -Werror -O3)
add_compile_options(-Wall -Werror -O3)
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json EXCLUDE_FROM_ALL)
add_subdirectory(fmt EXCLUDE_FROM_ALL)

include_directories(src)
add_executable(cocotools main.cpp src/coco.cpp src/cocoeval.cpp)
target_link_libraries(cocotools PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(cocotools PRIVATE fmt::fmt)