include_directories(../lib_pb ../lib_seeds_revised/)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${EIGEN_INCLUDE_DIRS})

add_executable(pb_cli main.cpp)
target_link_libraries(pb_cli ${Boost_LIBRARIES} ${OpenCV_LIBS} seeds_revised pb_elimination pb_maxflow)