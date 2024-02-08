Second Assignment
================================

This repo contains a project for second assignment for Research Track I course.

# The Goal
The task was to create a ROS package, with three nodes:
1. A node that implements an action client, allowing the user to set a target (x, y) or to cancel it. 
    - The node also publishes the robot position and velocity as a custom message (x,y, vel_x, vel_z), by relying on the values published on the topic
2. Service node that, when called, should return the coordinates of the last target sent by the user
3. Another service node that gets to the robot's current position and average speed.


# Installing and running
## 1. Installing

In order to use this project, package [assignment_2_2023](https://github.com/CarmineD8/assignment_2_2023) is required.

## 2. Run
To successfully run the project in predefine scenario, the author advises to use `roslaunch` command.

```
roslaunch rt_assignment_2 assignment_2.launch
```

In the .launch file there is an arg `av_window_size` (the window size for average speed calculation) which can be additionally set by the user while running eg:

```
roslaunch rt_assignment_2 test.launch av_window_size:=123
```
If the argument is not set the parameter takes the default value of 10.

The user may also run all the nodes seperately using `rosrun`.

# Project overview and structure

## 1. Controls

According to the tasks of the assignment, the user may set or cancel goal for action server.
Additionally to that, there are services that, when called return the latest robot target, or the distance to that target and average speed in both x,y directions

### Set new target
In order to set the new target, the user should publish new message onto `/robot_target` topic. The type of message to be published is `rt_assignment_2/RobotTarget`

```
rostopic pub /robot_target rt_assignment_2/RobotTarget "{x: 3.5, y: 3}"
```

### Cancel the goal
Similarly to the previous example, in order to cancel the goal the user should publish new message onto `/robot_cancel_goal` topic. The type is `rt_assignment_2/RobotCancelGoal`, but the message is empty.

```
rostopic pub /robot_cancel_goal rt_assignment_2/RobotCancelGoal
```

### Get the last target (service)
To get the latest target set by the user, the service `/get_last_target` needs to be called.
If there was no target yet set. The node will log a warning and return 0 in both x,y coordinates.

```
rosservice call /get_last_target 
```


### Get target distance
To get the distance to the target, the user needs to call the `/get_target_distance` service.

```
rosservice call /get_target_distance 
```
This service returns 5 values. 
- dist - distance to the target
- dist_x, dist_y - distances in both x and y
- av_speed_x, av_speed_y - robot speed in both x and y

## 2. The structure
Project was built using both C++ and Python scripts. In `src/` there are two files: `set_target_node.cpp` and `main.cpp`. The first one provides definitions of member functions of a class *SetTargetNode*, which is declared in `include/rt_assignment_2/set_target_node.hpp`.

Main function is used only to initialize the most important components and spin ros node. 

```
Main Function:
  Initialize ROS node
  Create ROS node handle
  Create Action Client to PlanningAction
  Try:
    Create SetTargetNode object with Action Client and Node Handle
    Log "set_target_node ready"
    Spin ROS
  Catch exception:
    Log exception message
  Log "Shutting down"
  Shutdown ROS
  Return 0
```

While most of the logic is hidden inside the constructor of *SetTargetNode* .

```
SetTargetNode Class:
  Constructor:
    Store Action Client and Node Handle
    Log "Waiting for server to connect"
    Waitfor Action Server to connect
    Subscribe to "/robot_target" with setNewTargetCallback
    Subscribe to "/odom" with robotStateCallback
    Subscribe to "/robot_cancel_goal" with cancelGoalCallback
    Advertise "/robot_state" to publish robot state
    Create timer to actualPoseCallback with 0.5 seconds interval

```

The logic based on callbacks to subscribers, publisher, and a timer, to get various information

```
actualPoseCallback Function:
    If Action Client state is ACTIVE:
      Log current robot position (feedback_pos_x_, feedback_pos_y_)

  cancelGoalCallback Function:
    Cancel the current goal using Action Client
    Log "Robot's goal is canceled"

  setNewTargetCallback Function:
    Log "New Target arrived"
    Create PlanningGoal with target coordinates from received message
    Send the goal using Action Client
    Log "New Goal has been sent"

  robotStateCallback Function:
    Create RobotState message from received Odometry message
    Publish the RobotState message to "/robot_state"

  doneCallback Function:
    Log the goal finishing state and result
    Log "Select new target by publishing into /robot_target"

```

# Further improvements
--------------------
In author's opinion these could be additional changes to improve the project:
- [ ] Change the rostopic pub messages into rosservices. Current state is a bit clunky and doesnt come in hande to simply cancel the goal or set new target. Use of services would be much more compact here. 
- [ ] In some cases, it would be better to use standard messages instead of custom ones (eg. /robot_cancel_goal)

