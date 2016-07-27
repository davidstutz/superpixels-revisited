cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS  "-O4 -fpermissive")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 
# To build the project, install CMake, then change into 'build' and
# call 'cmake ../'.
# 

include_directories(lib_seeds_revised/lib)

# SEEDS Revised
add_subdirectory(lib_seeds_revised)

# Constant Intensity Superpixels/Compact Superpixels
# Remove comments after installing the library as described in
# lib_cli/README.md!
add_subdirectory(lib_cis)
add_subdirectory(cis_cli)

# Entropy Rate Superpixels
add_subdirectory(lib_ers)
add_subdirectory(ers_cli)

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
add_subdirectory(vlfeat_slic_cli)
