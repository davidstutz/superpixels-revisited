include_directories(../lib_cis/ ../lib_cis/vlib/include/)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${EIGEN_INCLUDE_DIRS})

add_executable(cis_cli main.cpp)
target_link_libraries(cis_cli ${Boost_LIBRARIES} ${OpenCV_LIBS} ${PCL_LIBRARIES} seeds_revised cis vlib)
