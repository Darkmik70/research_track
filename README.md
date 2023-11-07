First Assignment
================================

This project is meant as a solution to an assignment for Research Track I corse. It is based on a simulator developed by developed by [Student Robotics](https://studentrobotics.org).

The Goal
--------
The task for the first assignment was to write a python node that controls the robot to put all the golden boxes together.

Installing and running
----------------------

This simulator requires a Python 3  to be run correctly. For more information regarding installing, running, troubleshooting etc. please refer to the [original project](
https://github.com/CarmineD8/python_simulator/tree/assignment23_python3)

Code Description
================
This section explains sections of the code of the assignment

`1.` Constants
Global constants were used for threasholds for robots orientation and distance, depending whether the robot is trying to grab the object or realease it.
```python
GRAB_THRESHOLD = 0.4
RELEASE_THRESHOLD = GRAB_THRESHOLD * 1.5
ANGLE_THRESHOLD = 2.0
```
Functions
---------
Here are some functions, with `find_marker()` being excluded of the logic are presented 

`*` `drive(speed, seconds)` - This function provides robot to drive forwards. A variation, `drive_back(speed, seconds)` drives robot backwards 
```python
def drive(speed, seconds):
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0
```

`*` `turn_cws(speed, seconds)` - allows robot to perform clockwise rotation. Variation `turn_cnt_cws(speed,seconds)` performs counter clockwise rotation

```python
def turn_cws(speed, seconds):
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = -speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0
```

`2.` `find_marker(group_mode)`
This function looks for boxes and identifies the target based on the robot's mode. The pseudocode presenting its operation:
```python
function find_marker(group_mode):
  Initialize distance to 100
  for each marker seen by the robot:
    if in group_mode:
      Check if the marker is part of the group and update distance and box_id
    else:
      Check for the closest uncollected marker and update distance and marker info
  if no relevant marker is found:
    return -1, -1, -1
  else:
    return distance, rotation, marker_id
```


`3.` Main loop 
This part will provides the pseudocode of the main loop operation.

```python
declare:
  angle_threshold = ANGLE_THRESHOLD
  dist_threshold = GRAB_THRESHOLD
  box_captured =  empty list
  group_mode = False

while True:
  dist, rot_y, box_id = find_marker(group_mode)

  if box_captured is empty and box_id is not -1:
    Add box_id to box_captured

  if no marker found:
    Turn counterclockwise and continue
  else if distance to marker is greater than dist_threshold:
    if rotation to marker is greater than angle_threshold:
      Turn clockwise slightly and continue
    else if rotation to marker is less than -angle_threshold:
      Turn counterclockwise slightly and continue
    else:
      Drive forward based on distance to marker and continue
  else:
    if not in group_mode and robot successfully grabs the box:
      Add box to box_captured, switch to group mode, and increase dist_threshold to releasing_threshold
    else if in group mode:
      Release the box, switch off group mode, and reset dist_threshold
    ELSE:
      Drive backward and attempt to grab again
```
Further improvements
--------------------
In author's opinion these could be additional changes to improve the project:



