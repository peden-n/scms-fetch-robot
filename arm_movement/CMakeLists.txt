cmake_minimum_required(VERSION 3.7.2)
project(arm_movement)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# find_package(catkin REQUIRED COMPONENTS geometric_shapes roscpp std_msgs)

find_package(catkin REQUIRED
  COMPONENTS
    interactive_markers
    moveit_core
    moveit_visual_tools
    moveit_ros_planning
    moveit_ros_planning_interface
    moveit_ros_perception
    pluginlib
    geometric_shapes
    pcl_ros
    pcl_conversions
    rosbag
    tf2_ros
    tf2_eigen
    tf2_geometry_msgs
)

find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED system filesystem date_time thread)

# set(THIS_PACKAGE_INCLUDE_DIRS
#   doc/interactivity/include
# )

catkin_package(
  LIBRARIES
  INCLUDE_DIRS
  CATKIN_DEPENDS
    moveit_core
    moveit_visual_tools
    moveit_ros_planning_interface
    interactive_markers
    tf2_geometry_msgs
  DEPENDS
    EIGEN3
)

# include_directories(include ${catkin_INCLUDE_DIRS})


include_directories(${THIS_PACKAGE_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIRS})

# add_subdirectory(move_group_interface)

add_executable(arm_movement src/main.cpp)
target_link_libraries(arm_movement ${catkin_LIBRARIES} ${Boost_LIBRARIES})
install(TARGETS arm_movement DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

install(
  PROGRAMS
    scripts/disco.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
  DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
