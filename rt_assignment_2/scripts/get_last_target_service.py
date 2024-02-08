#!/usr/bin/env python

import rospy

from rt_assignment_2.srv import LastTarget, LastTargetResponse
from rt_assignment_2.msg import RobotTarget


class LastTargetServer:
    """ROS service server for handling the latest target sent by user to the robot"""
    def __init__(self, name: str) -> None:
        rospy.init_node(name)
        self.last_target = None
        rospy.Subscriber("/robot_target", RobotTarget, self.robot_target_callback)
        self.service = rospy.Service('get_last_target', LastTarget, self.handle_get_last_target)
        rospy.loginfo("Service ready to provide the last target coordinates.")
    
    def robot_target_callback(self, data):
        """Calback to latest robots position set by user"""
        self.last_target = data

    def handle_get_last_target(self, req):
        """Handles the 'get_last_target' service"""
        response = LastTargetResponse()
        if self.last_target:
            response.x = self.last_target.x
            response.y = self.last_target.y
        else:
            rospy.logwarn("No target received yet.")
        return response
    
def main():
    last_target_server_node = LastTargetServer('get_last_target')
    rospy.spin()

if __name__ == "__main__":
    try:
        main()
    except rospy.ROSInterruptException:
        pass