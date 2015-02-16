cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS  "-O4 -Wwrite-strings -fpermissive")

# 
# To build the project, install CMake, then change into 'build' and
# call 'cmake ../'.
# 

# SEEDS Revised is built at root level as the library is a Git submodule
# already containing CMakeLists.txt.
# find_package(OpenCV REQUIRED)
# find_package(Boost REQUIRED)

include_directories(lib_seeds_revised/lib)
add_library(seeds_revised lib_seeds_revised/lib/SeedsRevised.cpp lib_seeds_revised/lib/Tools.cpp)
target_link_libraries(seeds_revised ${OpenCV_LIBRARIES})

# add_executable(seeds_revised_cli lib_seeds_revised/cli/main.cpp)
# target_link_libraries(seeds_revised_cli ${Boost_LIBRARIES} ${OpenCV_LIBRARIES} seeds_revised)

# Constant Intensity Superpixels/Compact Superpixels
# add_subdirectory(lib_cis)
# add_subdirectory(cis_cli)

# Contour Relaxed Superpixels
add_subdirectory(lib_crs)
add_subdirectory(crs_cli)

# Felzenswalb & Huttenlocher
add_subdirectory(lib_fh)
add_subdirectory(fh_cli)

# Pseudo Boolean Superpixels
add_subdirectory(lib_pb)
add_subdirectory(pb_cli)

# SEEDS
add_subdirectory(lib_seeds)
add_subdirectory(seeds_cli)

# SLIC
add_subdirectory(lib_slic)
add_subdirectory(slic_cli)

# VLFeat SLIC
add_subdirectory(lib_vlfeat)
add_subdirectory(vlfeat_slic_cli)
