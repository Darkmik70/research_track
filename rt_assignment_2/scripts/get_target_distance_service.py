#!/usr/bin/env python

import rospy
from math import hypot

from rt_assignment_2.msg import RobotState
from rt_assignment_2.msg import RobotTarget
from rt_assignment_2.srv import TargetDistance, TargetDistanceResponse


class DistanceToTargetServer:
    """
    ROS service server for handling distance and average speed calculations based on the robot's target.

    Attributes:
    - robot_speed_list: List of robot's speeds for calculating average speed.
    - av_speed_window_limit: Size of the list for calculating average speed. Set by a parameter in launch/assignment_2.launch
    - robot_current: Robot's current position as a RobotState message
    - target_position: Robot's target set by the user as RobotTarget message.
    - is_target_set: Flag indicating whether the target has been set, for the first time
    
    Subscribers:
    - "/robot_target": Subscribes to the robot's target position updates.
    - "/robot_state": Subscribes to the robot's current state updates.

    Service:
    - "get_target_distance": Provides distance and average speed information based on the robot's state and target.

    Methods:
    - robot_target_callback: Callback for the "/robot_target" subscriber, stores the latest robot's target.
    - robot_state_callback: Callback for the "/robot_state" subscriber, collects current position and values for average speed.
    - handle_get_target_distance: Handles the "get_target_distance" service, calculates distance and average speed based on the robot's state and target.
    """

    def __init__(self, name: str) -> None:
        rospy.init_node(name, anonymous=False)  
        
        self.robot_speed_list = []                                                     
        self.av_speed_window_limit = rospy.get_param('av_window_size', default=10)      
        self.robot_current = RobotState()                                               
        self.target_position = RobotTarget()                                            
        self.is_target_set = False      

        rospy.Subscriber("/robot_target", RobotTarget, self.robot_target_callback)     
        rospy.Subscriber("/robot_state", RobotState, self.robot_state_callback)
        self.service = rospy.Service("get_target_distance", TargetDistance, \
                                      self.handle_get_target_distance)
                                      

    def robot_target_callback(self, data):
        """Callback to robot_target subscriber, gets the target coordinates"""
        # store the latest robot's target
        self.target_position.x = data.x
        self.target_position.y = data.y
        # When first target arrives, set to True
        if not self.is_target_set:

            self.is_target_set = True

    def robot_state_callback(self, data):
        """Callback to robot_state subscriber, collects current position and values for av_speed"""
        self.robot_current.x_pos = data.x_pos
        self.robot_current.y_pos = data.y_pos
        # fill the values for av_speed window
        if len(self.robot_speed_list) < self.av_speed_window_limit:
            self.robot_speed_list.append((data.x_vel, data.y_vel))
        elif len(self.robot_speed_list) == self.av_speed_window_limit:
            # First remove the first element
            self.robot_speed_list.pop(0)
            # Add data at the end of the list
            self.robot_speed_list.append((data.x_vel, data.y_vel))
        # else:
        #     # This should never happen
        #     rospy.logerr("The list size is goes over the limit.")
        #     rospy.logerr("List size = %d, Threshold is = %d", len(self.robot_speed_list), self.av_speed_window_limit)

    def handle_get_target_distance(self, req):
        """
        Handles the 'get_target_distance' service

        Returns:
        - GetDistanceToTargetResponse object, the response message containing calculated distance
        and average speed.

        If the target position is set, calculates the distance in x and y directions, the overall
        distance as a line, and the average speed in x and y directions based on the robot's speed
        history. 
        If the target position is not set, returns zero values for distance, but returns the values
        for average speed.
        """
        response = TargetDistanceResponse()
        if self.is_target_set:
            # Calculate distance
            response.dist_x = self.target_position.x - self.robot_current.x_pos
            response.dist_y = self.target_position.y - self.robot_current.y_pos
            rospy.loginfo("target_x = %d target_y %d", self.target_position.x, self.target_position.y)
            # distance as a line
            response.dist = hypot((self.target_position.x - self.robot_current.x_pos), \
                                  (self.target_position.y - self.robot_current.y_pos))
        else:
            # There is no target, set distance to zero
            response.dist_x = 0.0
            response.dist_y = 0.0
        # calculate average speed
        response.av_speed_x = sum( x[0] for x in self.robot_speed_list) / len(self.robot_speed_list)
        response.av_speed_y = sum( y[1] for y in self.robot_speed_list) / len(self.robot_speed_list)
        return response


def main():
    last_target_server_node = DistanceToTargetServer('get_target_distance')
    rospy.spin()


if __name__ == "__main__":
    try:
        main()
    except rospy.ROSInterruptException:
        pass