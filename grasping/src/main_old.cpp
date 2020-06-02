// #include <moveit/move_group_interface/move_group_interface.h>
// #include <moveit/planning_scene_interface/planning_scene_interface.h>

// #include <moveit_msgs/DisplayRobotState.h>
// #include <moveit_msgs/DisplayTrajectory.h>

// #include <moveit_msgs/AttachedCollisionObject.h>
// #include <moveit_msgs/CollisionObject.h>

// #include <moveit_visual_tools/moveit_visual_tools.h>

/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2012, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Ioan Sucan, Ridhwan Luthra*/

// ROS
#include <ros/ros.h>

// MoveIt!
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit/move_group_interface/move_group_interface.h>

// TF2
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

void openGripper(trajectory_msgs::JointTrajectory &posture)
{
    // BEGIN_SUB_TUTORIAL open_gripper
    /* Add both finger joints of panda robot. */
    posture.joint_names.resize(1);
    posture.joint_names[0] = "gripper";
    //posture.joint_names[1] = "panda_finger_joint2";

    /* Set them as open, wide enough for the object to fit. */
    posture.points.resize(1);
    posture.points[0].positions.resize(1);
    posture.points[0].positions[0] = 0.04;
    //posture.points[0].positions[1] = 0.04;
    posture.points[0].time_from_start = ros::Duration(0.5);
    // END_SUB_TUTORIAL
}

void closedGripper(trajectory_msgs::JointTrajectory &posture)
{
    // BEGIN_SUB_TUTORIAL closed_gripper
    /* Add both finger joints of panda robot. */
    posture.joint_names.resize(1);
    posture.joint_names[0] = "gripper";
    //posture.joint_names[1] = "panda_finger_joint2";

    /* Set them as closed. */
    posture.points.resize(1);
    posture.points[0].positions.resize(1);
    posture.points[0].positions[0] = 0.00;
    //posture.points[0].positions[1] = 0.00;
    posture.points[0].time_from_start = ros::Duration(0.5);
    // END_SUB_TUTORIAL
}

void pick(moveit::planning_interface::MoveGroupInterface &move_group)
{
    // BEGIN_SUB_TUTORIAL pick1
    // Create a vector of grasps to be attempted, currently only creating single grasp.
    // This is essentially useful when using a grasp generator to generate and test multiple grasps.
    std::vector<moveit_msgs::Grasp> grasps;
    grasps.resize(1);

    // Setting grasp pose
    // ++++++++++++++++++++++
    // This is the pose of panda_link8. |br|
    // From panda_link8 to the palm of the eef the distance is 0.058, the cube starts 0.01 before 5.0 (half of the length
    // of the cube). |br|
    // Therefore, the position for panda_link8 = 5 - (length of cube/2 - distance b/w panda_link8 and palm of eef - some
    // extra padding)
    grasps[0].grasp_pose.header.frame_id = "gripper";
    tf2::Quaternion orientation;
    orientation.setRPY(-M_PI / 2, -M_PI / 4, -M_PI / 2);
    grasps[0].grasp_pose.pose.orientation = tf2::toMsg(orientation);
    grasps[0].grasp_pose.pose.position.x = 0.415;
    grasps[0].grasp_pose.pose.position.y = 0;
    grasps[0].grasp_pose.pose.position.z = 0.5;

    // Setting pre-grasp approach
    // ++++++++++++++++++++++++++
    /* Defined with respect to frame_id */
    grasps[0].pre_grasp_approach.direction.header.frame_id = "gripper";
    /* Direction is set as positive x axis */
    grasps[0].pre_grasp_approach.direction.vector.x = 1.0;
    grasps[0].pre_grasp_approach.min_distance = 0.095;
    grasps[0].pre_grasp_approach.desired_distance = 0.115;

    // Setting post-grasp retreat
    // ++++++++++++++++++++++++++
    /* Defined with respect to frame_id */
    grasps[0].post_grasp_retreat.direction.header.frame_id = "gripper";
    /* Direction is set as positive z axis */
    grasps[0].post_grasp_retreat.direction.vector.z = 1.0;
    grasps[0].post_grasp_retreat.min_distance = 0.1;
    grasps[0].post_grasp_retreat.desired_distance = 0.25;

    // Setting posture of eef before grasp
    // +++++++++++++++++++++++++++++++++++
    openGripper(grasps[0].pre_grasp_posture);
    // END_SUB_TUTORIAL

    // BEGIN_SUB_TUTORIAL pick2
    // Setting posture of eef during grasp
    // +++++++++++++++++++++++++++++++++++
    closedGripper(grasps[0].grasp_posture);
    // END_SUB_TUTORIAL

    // BEGIN_SUB_TUTORIAL pick3
    // Set support surface as table1.
    //move_group.setSupportSurfaceName("table1");
    // Call pick to pick up the object using the grasps given
    move_group.pick("object", grasps);
    // END_SUB_TUTORIAL
}

void place(moveit::planning_interface::MoveGroupInterface &group)
{
    // BEGIN_SUB_TUTORIAL place
    // TODO(@ridhwanluthra) - Calling place function may lead to "All supplied place locations failed. Retrying last
    // location in
    // verbose mode." This is a known issue and we are working on fixing it. |br|
    // Create a vector of placings to be attempted, currently only creating single place location.
    std::vector<moveit_msgs::PlaceLocation> place_location;
    place_location.resize(1);

    // Setting place location pose
    // +++++++++++++++++++++++++++
    place_location[0].place_pose.header.frame_id = "gripper";
    tf2::Quaternion orientation;
    orientation.setRPY(0, 0, M_PI / 2);
    place_location[0].place_pose.pose.orientation = tf2::toMsg(orientation);

    /* While placing it is the exact location of the center of the object. */
    place_location[0].place_pose.pose.position.x = 0;
    place_location[0].place_pose.pose.position.y = 0.5;
    place_location[0].place_pose.pose.position.z = 0.5;

    // Setting pre-place approach
    // ++++++++++++++++++++++++++
    /* Defined with respect to frame_id */
    place_location[0].pre_place_approach.direction.header.frame_id = "gripper";
    /* Direction is set as negative z axis */
    place_location[0].pre_place_approach.direction.vector.z = -1.0;
    place_location[0].pre_place_approach.min_distance = 0.095;
    place_location[0].pre_place_approach.desired_distance = 0.115;

    // Setting post-grasp retreat
    // ++++++++++++++++++++++++++
    /* Defined with respect to frame_id */
    place_location[0].post_place_retreat.direction.header.frame_id = "gripper";
    /* Direction is set as negative y axis */
    place_location[0].post_place_retreat.direction.vector.y = -1.0;
    place_location[0].post_place_retreat.min_distance = 0.1;
    place_location[0].post_place_retreat.desired_distance = 0.25;

    // Setting posture of eef after placing object
    // +++++++++++++++++++++++++++++++++++++++++++
    /* Similar to the pick case */
    openGripper(place_location[0].post_place_posture);

    // Set support surface as table2.
    group.setSupportSurfaceName("table2");
    // Call place to place the object using the place locations given.
    group.place("object", place_location);
    // END_SUB_TUTORIAL
}

void addCollisionObjects(moveit::planning_interface::PlanningSceneInterface &planning_scene_interface)
{
    // BEGIN_SUB_TUTORIAL table1
    //
    // Creating Environment
    // ^^^^^^^^^^^^^^^^^^^^
    // Create vector to hold 3 collision objects.
    std::vector<moveit_msgs::CollisionObject> collision_objects;
    collision_objects.resize(3);

    // Add the first table where the cube will originally be kept.
    collision_objects[0].id = "table1";
    collision_objects[0].header.frame_id = "gripper";

    /* Define the primitive and its dimensions. */
    collision_objects[0].primitives.resize(1);
    collision_objects[0].primitives[0].type = collision_objects[0].primitives[0].BOX;
    collision_objects[0].primitives[0].dimensions.resize(3);
    collision_objects[0].primitives[0].dimensions[0] = 0.2;
    collision_objects[0].primitives[0].dimensions[1] = 0.4;
    collision_objects[0].primitives[0].dimensions[2] = 0.4;

    /* Define the pose of the table. */
    collision_objects[0].primitive_poses.resize(1);
    collision_objects[0].primitive_poses[0].position.x = 0.5;
    collision_objects[0].primitive_poses[0].position.y = 0;
    collision_objects[0].primitive_poses[0].position.z = 0.2;
    // END_SUB_TUTORIAL

    collision_objects[0].operation = collision_objects[0].ADD;

    // BEGIN_SUB_TUTORIAL table2
    // Add the second table where we will be placing the cube.
    collision_objects[1].id = "table2";
    collision_objects[1].header.frame_id = "gripper";

    /* Define the primitive and its dimensions. */
    collision_objects[1].primitives.resize(1);
    collision_objects[1].primitives[0].type = collision_objects[1].primitives[0].BOX;
    collision_objects[1].primitives[0].dimensions.resize(3);
    collision_objects[1].primitives[0].dimensions[0] = 0.4;
    collision_objects[1].primitives[0].dimensions[1] = 0.2;
    collision_objects[1].primitives[0].dimensions[2] = 0.4;

    /* Define the pose of the table. */
    collision_objects[1].primitive_poses.resize(1);
    collision_objects[1].primitive_poses[0].position.x = 0;
    collision_objects[1].primitive_poses[0].position.y = 0.5;
    collision_objects[1].primitive_poses[0].position.z = 0.2;
    // END_SUB_TUTORIAL

    collision_objects[1].operation = collision_objects[1].ADD;

    // BEGIN_SUB_TUTORIAL object
    // Define the object that we will be manipulating
    collision_objects[2].header.frame_id = "gripper";
    collision_objects[2].id = "object";

    /* Define the primitive and its dimensions. */
    collision_objects[2].primitives.resize(1);
    collision_objects[2].primitives[0].type = collision_objects[1].primitives[0].BOX;
    collision_objects[2].primitives[0].dimensions.resize(3);
    collision_objects[2].primitives[0].dimensions[0] = 0.02;
    collision_objects[2].primitives[0].dimensions[1] = 0.02;
    collision_objects[2].primitives[0].dimensions[2] = 0.2;

    /* Define the pose of the object. */
    collision_objects[2].primitive_poses.resize(1);
    collision_objects[2].primitive_poses[0].position.x = 0.5;
    collision_objects[2].primitive_poses[0].position.y = 0;
    collision_objects[2].primitive_poses[0].position.z = 0.5;
    // END_SUB_TUTORIAL

    collision_objects[2].operation = collision_objects[2].ADD;

    planning_scene_interface.applyCollisionObjects(collision_objects);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "gripper");
    ros::NodeHandle nh;
    ros::AsyncSpinner spinner(1);
    spinner.start();

    ros::WallDuration(1.0).sleep();
    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
    moveit::planning_interface::MoveGroupInterface group("gripper");
    trajectory_msgs::JointTrajectory posture; //moveit::planning_interface::posture posture;
    group.setPlanningTime(45.0);

    //   addCollisionObjects(planning_scene_interface);

    //   // Wait a bit for ROS things to initialize
    //   ros::WallDuration(1.0).sleep();

    //   pick(group);

    //   ros::WallDuration(1.0).sleep();

    //   place(group);

    closedGripper(posture);

    ros::waitForShutdown();
    return 0;
}

// BEGIN_TUTORIAL
// CALL_SUB_TUTORIAL table1
// CALL_SUB_TUTORIAL table2
// CALL_SUB_TUTORIAL object
//
// Pick Pipeline
// ^^^^^^^^^^^^^
// CALL_SUB_TUTORIAL pick1
// openGripper function
// """"""""""""""""""""
// CALL_SUB_TUTORIAL open_gripper
// CALL_SUB_TUTORIAL pick2
// closedGripper function
// """"""""""""""""""""""
// CALL_SUB_TUTORIAL closed_gripper
// CALL_SUB_TUTORIAL pick3
//
// Place Pipeline
// ^^^^^^^^^^^^^^
// CALL_SUB_TUTORIAL place
// END_TUTORIAL
// © 2020 GitHub, Inc.
// Terms
// Privacy
// Security
// Status
// Help
// Contact GitHub
// Pricing
// API
// Training
// Blog
// About

//}

// int main(int argc, char **argv)
// {
//     ros::init(argc, argv, "move_group_interface_tutorial");
//     ros::NodeHandle node_handle;
//     ros::AsyncSpinner spinner(1);
//     spinner.start();

//     // BEGIN_TUTORIAL
//     //
//     // Setup
//     // ^^^^^
//     //
//     // MoveIt operates on sets of joints called "planning groups" and stores them in an object called
//     // the `JointModelGroup`. Throughout MoveIt the terms "planning group" and "joint model group"
//     // are used interchangably.
//     static const std::string PLANNING_GROUP = "panda_arm";

//     // The :move_group_interface:`MoveGroupInterface` class can be easily
//     // setup using just the name of the planning group you would like to control and plan for.
//     moveit::planning_interface::MoveGroupInterface move_group(PLANNING_GROUP);

//     // We will use the :planning_scene_interface:`PlanningSceneInterface`
//     // class to add and remove collision objects in our "virtual world" scene
//     moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

//     // Raw pointers are frequently used to refer to the planning group for improved performance.
//     const robot_state::JointModelGroup *joint_model_group =
//         move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);

//     // Visualization
//     // ^^^^^^^^^^^^^
//     //
//     // The package MoveItVisualTools provides many capabilties for visualizing objects, robots,
//     // and trajectories in RViz as well as debugging tools such as step-by-step introspection of a script
//     namespace rvt = rviz_visual_tools;
//     moveit_visual_tools::MoveItVisualTools visual_tools("panda_link0");
//     visual_tools.deleteAllMarkers();

//     // Remote control is an introspection tool that allows users to step through a high level script
//     // via buttons and keyboard shortcuts in RViz
//     visual_tools.loadRemoteControl();

//     // RViz provides many types of markers, in this demo we will use text, cylinders, and spheres
//     Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
//     text_pose.translation().z() = 1.75;
//     visual_tools.publishText(text_pose, "MoveGroupInterface Demo", rvt::WHITE, rvt::XLARGE);

//     // Batch publishing is used to reduce the number of messages being sent to RViz for large visualizations
//     visual_tools.trigger();

//     // Getting Basic Information
//     // ^^^^^^^^^^^^^^^^^^^^^^^^^
//     //
//     // We can print the name of the reference frame for this robot.
//     ROS_INFO_NAMED("tutorial", "Planning frame: %s", move_group.getPlanningFrame().c_str());

//     // We can also print the name of the end-effector link for this group.
//     ROS_INFO_NAMED("tutorial", "End effector link: %s", move_group.getEndEffectorLink().c_str());

//     // We can get a list of all the groups in the robot:
//     ROS_INFO_NAMED("tutorial", "Available Planning Groups:");
//     std::copy(move_group.getJointModelGroupNames().begin(), move_group.getJointModelGroupNames().end(),
//               std::ostream_iterator<std::string>(std::cout, ", "));

//     // Start the demo
//     // ^^^^^^^^^^^^^^^^^^^^^^^^^
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo");

//     // Planning to a Pose goal
//     // ^^^^^^^^^^^^^^^^^^^^^^^
//     // We can plan a motion for this group to a desired pose for the
//     // end-effector.
//     geometry_msgs::Pose target_pose1;
//     target_pose1.orientation.w = 1.0;
//     target_pose1.position.x = 0.28;
//     target_pose1.position.y = -0.2;
//     target_pose1.position.z = 0.5;
//     move_group.setPoseTarget(target_pose1);

//     // Now, we call the planner to compute the plan and visualize it.
//     // Note that we are just planning, not asking move_group
//     // to actually move the robot.
//     moveit::planning_interface::MoveGroupInterface::Plan my_plan;

//     bool success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);

//     ROS_INFO_NAMED("tutorial", "Visualizing plan 1 (pose goal) %s", success ? "" : "FAILED");

//     // Visualizing plans
//     // ^^^^^^^^^^^^^^^^^
//     // We can also visualize the plan as a line with markers in RViz.
//     ROS_INFO_NAMED("tutorial", "Visualizing plan 1 as trajectory line");
//     visual_tools.publishAxisLabeled(target_pose1, "pose1");
//     visual_tools.publishText(text_pose, "Pose Goal", rvt::WHITE, rvt::XLARGE);
//     visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
//     visual_tools.trigger();
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");

//     // Moving to a pose goal
//     // ^^^^^^^^^^^^^^^^^^^^^
//     //
//     // Moving to a pose goal is similar to the step above
//     // except we now use the move() function. Note that
//     // the pose goal we had set earlier is still active
//     // and so the robot will try to move to that goal. We will
//     // not use that function in this tutorial since it is
//     // a blocking function and requires a controller to be active
//     // and report success on execution of a trajectory.

//     /* Uncomment below line when working with a real robot */
//     /* move_group.move(); */

//     // Planning to a joint-space goal
//     // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//     //
//     // Let's set a joint space goal and move towards it.  This will replace the
//     // pose target we set above.
//     //
//     // To start, we'll create an pointer that references the current robot's state.
//     // RobotState is the object that contains all the current position/velocity/acceleration data.
//     moveit::core::RobotStatePtr current_state = move_group.getCurrentState();
//     //
//     // Next get the current set of joint values for the group.
//     std::vector<double> joint_group_positions;
//     current_state->copyJointGroupPositions(joint_model_group, joint_group_positions);

//     // Now, let's modify one of the joints, plan to the new joint space goal and visualize the plan.
//     joint_group_positions[0] = -1.0; // radians
//     move_group.setJointValueTarget(joint_group_positions);

//     success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
//     ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");

//     // Visualize the plan in RViz
//     visual_tools.deleteAllMarkers();
//     visual_tools.publishText(text_pose, "Joint Space Goal", rvt::WHITE, rvt::XLARGE);
//     visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
//     visual_tools.trigger();
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");

//     // Planning with Path Constraints
//     // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//     //
//     // Path constraints can easily be specified for a link on the robot.
//     // Let's specify a path constraint and a pose goal for our group.
//     // First define the path constraint.
//     moveit_msgs::OrientationConstraint ocm;
//     ocm.link_name = "panda_link7";
//     ocm.header.frame_id = "panda_link0";
//     ocm.orientation.w = 1.0;
//     ocm.absolute_x_axis_tolerance = 0.1;
//     ocm.absolute_y_axis_tolerance = 0.1;
//     ocm.absolute_z_axis_tolerance = 0.1;
//     ocm.weight = 1.0;

//     // Now, set it as the path constraint for the group.
//     moveit_msgs::Constraints test_constraints;
//     test_constraints.orientation_constraints.push_back(ocm);
//     move_group.setPathConstraints(test_constraints);

//     // We will reuse the old goal that we had and plan to it.
//     // Note that this will only work if the current state already
//     // satisfies the path constraints. So, we need to set the start
//     // state to a new pose.
//     robot_state::RobotState start_state(*move_group.getCurrentState());
//     geometry_msgs::Pose start_pose2;
//     start_pose2.orientation.w = 1.0;
//     start_pose2.position.x = 0.55;
//     start_pose2.position.y = -0.05;
//     start_pose2.position.z = 0.8;
//     start_state.setFromIK(joint_model_group, start_pose2);
//     move_group.setStartState(start_state);

//     // Now we will plan to the earlier pose target from the new
//     // start state that we have just created.
//     move_group.setPoseTarget(target_pose1);

//     // Planning with constraints can be slow because every sample must call an inverse kinematics solver.
//     // Lets increase the planning time from the default 5 seconds to be sure the planner has enough time to succeed.
//     move_group.setPlanningTime(10.0);

//     success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
//     ROS_INFO_NAMED("tutorial", "Visualizing plan 3 (constraints) %s", success ? "" : "FAILED");

//     // Visualize the plan in RViz
//     visual_tools.deleteAllMarkers();
//     visual_tools.publishAxisLabeled(start_pose2, "start");
//     visual_tools.publishAxisLabeled(target_pose1, "goal");
//     visual_tools.publishText(text_pose, "Constrained Goal", rvt::WHITE, rvt::XLARGE);
//     visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
//     visual_tools.trigger();
//     visual_tools.prompt("next step");

//     // When done with the path constraint be sure to clear it.
//     move_group.clearPathConstraints();

//     // Cartesian Paths
//     // ^^^^^^^^^^^^^^^
//     // You can plan a Cartesian path directly by specifying a list of waypoints
//     // for the end-effector to go through. Note that we are starting
//     // from the new start state above.  The initial pose (start state) does not
//     // need to be added to the waypoint list but adding it can help with visualizations
//     std::vector<geometry_msgs::Pose> waypoints;
//     waypoints.push_back(start_pose2);

//     geometry_msgs::Pose target_pose3 = start_pose2;

//     target_pose3.position.z -= 0.2;
//     waypoints.push_back(target_pose3); // down

//     target_pose3.position.y -= 0.2;
//     waypoints.push_back(target_pose3); // right

//     target_pose3.position.z += 0.2;
//     target_pose3.position.y += 0.2;
//     target_pose3.position.x -= 0.2;
//     waypoints.push_back(target_pose3); // up and left

//     // Cartesian motions are frequently needed to be slower for actions such as approach and retreat
//     // grasp motions. Here we demonstrate how to reduce the speed of the robot arm via a scaling factor
//     // of the maxiumum speed of each joint. Note this is not the speed of the end effector point.
//     move_group.setMaxVelocityScalingFactor(0.1);

//     // We want the Cartesian path to be interpolated at a resolution of 1 cm
//     // which is why we will specify 0.01 as the max step in Cartesian
//     // translation.  We will specify the jump threshold as 0.0, effectively disabling it.
//     // Warning - disabling the jump threshold while operating real hardware can cause
//     // large unpredictable motions of redundant joints and could be a safety issue
//     moveit_msgs::RobotTrajectory trajectory;
//     const double jump_threshold = 0.0;
//     const double eef_step = 0.01;
//     double fraction = move_group.computeCartesianPath(waypoints, eef_step, jump_threshold, trajectory);
//     ROS_INFO_NAMED("tutorial", "Visualizing plan 4 (Cartesian path) (%.2f%% acheived)", fraction * 100.0);

//     // Visualize the plan in RViz
//     visual_tools.deleteAllMarkers();
//     visual_tools.publishText(text_pose, "Joint Space Goal", rvt::WHITE, rvt::XLARGE);
//     visual_tools.publishPath(waypoints, rvt::LIME_GREEN, rvt::SMALL);
//     for (std::size_t i = 0; i < waypoints.size(); ++i)
//         visual_tools.publishAxisLabeled(waypoints[i], "pt" + std::to_string(i), rvt::SMALL);
//     visual_tools.trigger();
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");

//     // Adding/Removing Objects and Attaching/Detaching Objects
//     // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//     //
//     // Define a collision object ROS message.
//     moveit_msgs::CollisionObject collision_object;
//     collision_object.header.frame_id = move_group.getPlanningFrame();

//     // The id of the object is used to identify it.
//     collision_object.id = "box1";

//     // Define a box to add to the world.
//     shape_msgs::SolidPrimitive primitive;
//     primitive.type = primitive.BOX;
//     primitive.dimensions.resize(3);
//     primitive.dimensions[0] = 0.4;
//     primitive.dimensions[1] = 0.1;
//     primitive.dimensions[2] = 0.4;

//     // Define a pose for the box (specified relative to frame_id)
//     geometry_msgs::Pose box_pose;
//     box_pose.orientation.w = 1.0;
//     box_pose.position.x = 0.4;
//     box_pose.position.y = -0.2;
//     box_pose.position.z = 1.0;

//     collision_object.primitives.push_back(primitive);
//     collision_object.primitive_poses.push_back(box_pose);
//     collision_object.operation = collision_object.ADD;

//     std::vector<moveit_msgs::CollisionObject> collision_objects;
//     collision_objects.push_back(collision_object);

//     // Now, let's add the collision object into the world
//     ROS_INFO_NAMED("tutorial", "Add an object into the world");
//     planning_scene_interface.addCollisionObjects(collision_objects);

//     // Show text in RViz of status
//     visual_tools.publishText(text_pose, "Add object", rvt::WHITE, rvt::XLARGE);
//     visual_tools.trigger();

//     // Wait for MoveGroup to recieve and process the collision object message
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to once the collision object appears in RViz");

//     // Now when we plan a trajectory it will avoid the obstacle
//     move_group.setStartState(*move_group.getCurrentState());
//     geometry_msgs::Pose another_pose;
//     another_pose.orientation.w = 1.0;
//     another_pose.position.x = 0.4;
//     another_pose.position.y = -0.4;
//     another_pose.position.z = 0.9;
//     move_group.setPoseTarget(another_pose);

//     success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
//     ROS_INFO_NAMED("tutorial", "Visualizing plan 5 (pose goal move around cuboid) %s", success ? "" : "FAILED");

//     // Visualize the plan in RViz
//     visual_tools.deleteAllMarkers();
//     visual_tools.publishText(text_pose, "Obstacle Goal", rvt::WHITE, rvt::XLARGE);
//     visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
//     visual_tools.trigger();
//     visual_tools.prompt("next step");

//     // Now, let's attach the collision object to the robot.
//     ROS_INFO_NAMED("tutorial", "Attach the object to the robot");
//     move_group.attachObject(collision_object.id);

//     // Show text in RViz of status
//     visual_tools.publishText(text_pose, "Object attached to robot", rvt::WHITE, rvt::XLARGE);
//     visual_tools.trigger();

//     /* Wait for MoveGroup to recieve and process the attached collision object message */
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to once the collision object attaches to the "
//                         "robot");

//     // Now, let's detach the collision object from the robot.
//     ROS_INFO_NAMED("tutorial", "Detach the object from the robot");
//     move_group.detachObject(collision_object.id);

//     // Show text in RViz of status
//     visual_tools.publishText(text_pose, "Object dettached from robot", rvt::WHITE, rvt::XLARGE);
//     visual_tools.trigger();

//     /* Wait for MoveGroup to recieve and process the attached collision object message */
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to once the collision object detaches to the "
//                         "robot");

//     // Now, let's remove the collision object from the world.
//     ROS_INFO_NAMED("tutorial", "Remove the object from the world");
//     std::vector<std::string> object_ids;
//     object_ids.push_back(collision_object.id);
//     planning_scene_interface.removeCollisionObjects(object_ids);

//     // Show text in RViz of status
//     visual_tools.publishText(text_pose, "Object removed", rvt::WHITE, rvt::XLARGE);
//     visual_tools.trigger();

//     /* Wait for MoveGroup to recieve and process the attached collision object message */
//     visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to once the collision object disapears");

//     // END_TUTORIAL

//     ros::shutdown();
//     return 0;
// }