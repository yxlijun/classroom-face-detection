project(CFD)
cmake_minimum_required(VERSION 2.8)

find_package(OpenCV REQUIRED)
include(mini-caffe/mini-caffe.cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/json)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/curl)
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib/)




file(GLOB INC include/*.h include/*.hpp)
file(GLOB SRC src/*.c src/*.cc src/*.cpp )

file(GLOB INS include/json/*.h include/json/*.hpp)
file(GLOB SRS src/lib_json/*.c src/lib_json/*.cc src/lib_json/*.cpp)


source_group(include FILES ${INC})
source_group(src FILES ${SRC})
source_group(jsonlib FILES ${INS})
source_group(jsoncpp FILES ${SRS})


add_executable(cfd ${INC} ${SRC} ${INS} ${SRS})
target_link_libraries(cfd caffe ${OpenCV_LIBS})
