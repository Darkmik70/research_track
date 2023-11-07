First Assignment
================================

This project is meant as a solution to an assignment for Research Track I corse. It is based on a simulator developed by developed by [Student Robotics](https://studentrobotics.org).

# The Goal
The task for the first assignment was to write a python node that controls the robot to put all the golden boxes together.

# Installing and running
This simulator requires a Python 3  to be run correctly. For more information regarding installing, running, troubleshooting etc. please refer to the [original project](
https://github.com/CarmineD8/python_simulator/tree/assignment23_python3)

# Behavior
The scripts makes robot to look for boxes in two modes. Depending on the `bool group_mode` robot is either searching for the group of boxes when `True` or the nearest box which is not in the group when `False`. In the first iteration the robot is saving the first spotted, **nearest** box as the one where it wants to put the rest of them and then begins searching for the rest of boxes.

When robot obtains the orientation and distance from itself to the box it rotates to be aligned with the box and drives towards its targed. When it is close it slows down a bit to make sure the `R.grab()` will be succesful and then goes back to the grouped boxes. 

While it drives back to the group, it considers the nearest location *any* of the boxes in the group, not just the first one.

When all of the boxes are grouped, it drives back and starts searching for another box. This is considered to be the reaction to *accomplished task*.

# Code Description
This section explains sections of the code of the assignment

## 1. Constants
Global constants were used for threasholds for robots orientation and distance, depending whether the robot is trying to grab the object or realease it.
```python
GRAB_THRESHOLD = 0.4
RELEASE_THRESHOLD = GRAB_THRESHOLD * 1.5
ANGLE_THRESHOLD = 2.0
```
## Functions
Here are some functions, with `find_marker()` being excluded of the logic are presented 

1. `drive(speed, seconds)` - This function provides robot to drive forwards. A variation, `drive_back(speed, seconds)` drives robot backwards 
```python
def drive(speed, seconds):
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0
```

2. `turn_cws(speed, seconds)` - allows robot to perform clockwise rotation. Variation `turn_cnt_cws(speed,seconds)` performs counter clockwise rotation

```python
def turn_cws(speed, seconds):
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = -speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0
```
### `find_marker(group_mode)`
This function looks for boxes and identifies the target based on the robot's mode. The pseudocode presenting its operation:
```python
function find_marker(group_mode):
  initialize distance to 100
  for each marker seen by the robot:
    if in group_mode:
      check if the marker is part of the group and update distance and box_id
    else:
      check for the closest uncollected marker and update distance and marker info
  if no relevant marker is found:
    return -1, -1, -1
  else:
    return distance, rotation, marker_id
```


### Main loop 
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
    turn counterclockwise and continue
  else if distance to marker is greater than dist_threshold:
    if rotation to marker is greater than angle_threshold:
      turn clockwise slightly and continue
    else if rotation to marker is less than -angle_threshold:
      turn counterclockwise slightly and continue
    else:
      drive forward based on distance to marker and continue
  else:
    if not in group_mode and robot successfully grabs the box:
      add box to box_captured, switch to group mode, and increase dist_threshold to releasing_threshold
    else if in group mode:
      release the box, switch off group mode, and reset dist_threshold
    else:
      drive backward and attempt to grab again
```
# Further improvements
--------------------
In author's opinion these could be additional changes to improve the project:
- [x] In the beginning of the main loop if statement can append box_id = -1, add proper handling
- [ ] Pass `box_captured`  as an argument to `find_marker()` instead of using it as a global variable that is changed in various places
- [ ] Refactor hardcoded values for speed and velocity with constants and local variables
