add_subdirectory(vlib)

include_directories(vlib/include/)
add_library(cis maxflow.cpp graph.cpp superpixels.cpp)
target_link_libraries(cis vlib)