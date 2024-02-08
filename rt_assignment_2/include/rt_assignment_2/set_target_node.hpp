#ifndef RT_ASSIGNMENT_2_SET_TARGET_HPP
#define RT_ASSIGNMENT_2_SET_TARGET_HPP


#include <ros/ros.h>

#include <rt_assignment_2/RobotTarget.h>
#include <rt_assignment_2/RobotCancelGoal.h>
#include <rt_assignment_2/RobotState.h>

#include <actionlib/client/simple_action_client.h>
#include <nav_msgs/Odometry.h>

#include <assignment_2_2023/PlanningAction.h>



namespace rt_assignment_2
{
    /**
     * @class SetTargetNode
     * @brief ROS node for setting/cancelling robot targets.
     *
     * Initializes a ROS node responsible for setting and managing robot targets.
     * It communicates with the Action Server '/reaching_goal' using an action client,
     * subscribes to '/robot_target' , '/odom' ,and '/robot_cancel_goal' topics,
     * and publishes the '/robot_state'.
     * Additionally, it sets up a timer to periodically update the robot's actual pose.
     */
    class SetTargetNode
    {
    public:
        /**
        * @brief Constructor for SetTargetNode.
        * 
        * Initializes all the subscribers, publishers and timers for communication
        * 
        * @param ac Action Client for communicating with the Action Server.
        * @param nh Shared pointer to the ROS NodeHandle.
        * 
        */
        SetTargetNode(
            const std::shared_ptr<actionlib::SimpleActionClient<assignment_2_2023::PlanningAction>> &ac,
            const std::shared_ptr<ros::NodeHandle> &nh);

    private:

        /**
         * Callback function for updating the robot's current position.
         * @param event TimerEvent that triggers the callback
         */
        void actualPoseCallback(const ros::TimerEvent &event);

        /**
         * Callback function to cancel the current goal
         * @param msg Const reference to received cancel message
        */
        void cancelGoalCallback(const rt_assignment_2::RobotCancelGoal::ConstPtr &msg);

        /**
         * @brief callback function for setting new target position
         * @param msg Const reference to received cancel message
        */
        void setNewTargetCallback(const rt_assignment_2::RobotTarget::ConstPtr &msg);
        
        /**
         * @brief Callback function for receiving robot state updates.
         * 
         *  Gets the values from /odom and publishes to RobotState.
         * 
         * @param msg const reference to received message from '/odom'
        */
        void robotStateCallback(const nav_msgs::Odometry::ConstPtr &msg);

        /**
         * Callback function called when the goal is successfully completed or preempted.
        */
        void doneCallback(const actionlib::SimpleClientGoalState &state, const assignment_2_2023::PlanningResultConstPtr &result);
        void activeCallback();
        
        /**
        * Callback function called during the execution of feedback
        * stores the current robots position
        */
       void feedbackCallback(const assignment_2_2023::PlanningFeedbackConstPtr &feedback);


        std::shared_ptr<actionlib::SimpleActionClient<assignment_2_2023::PlanningAction>> ac_;
        std::shared_ptr<ros::NodeHandle> nh_;


        ros::Subscriber sub_state_;     // Subscriber to robot's current position based on messages from /odom
        ros::Subscriber sub_target_;    // Subscriber to robot's new target based on msg type RobotTarget.msg
        ros::Subscriber sub_cancel_;    // Subscriber to cancel current robot's goal, msg type RobotCancelGoal.msg

        ros::Publisher pub_state_;      // Publisher to new

        // Timer to robot's current position, that logs the current position from Planning feedback
        ros::Timer get_actual_pose_timer_; 

        
        double feedback_pos_x_;     // Feedback position x from Planning/Feedback
        double feedback_pos_y_;     // Feedback position x from Planning/feedback
    };
}

#endif // RT_ASSIGNMENT_2_SET_TARGET_HPP
