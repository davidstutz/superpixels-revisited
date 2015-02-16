include_directories(../lib_seeds_revised/ ../lib_vlfeat/vl/)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${EIGEN_INCLUDE_DIRS})

add_executable(vlfeat_slic_cli main.cpp)
target_link_libraries(vlfeat_slic_cli ${Boost_LIBRARIES} ${OpenCV_LIBS} vlfeat seeds_revised)
