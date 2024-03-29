#include <rt_assignment_2/set_target_node.hpp>
#include <rt_assignment_2/RobotState.h>
#include <rt_assignment_2/RobotTarget.h>
#include <rt_assignment_2/DistLeftObstacle.h>

#include <ros/ros.h>
#include <ros/timer.h>

#include <boost/bind.hpp>

#include <actionlib/client/simple_action_client.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>

#include <assignment_2_2023/PlanningAction.h>



namespace rt_assignment_2
{

    SetTargetNode::SetTargetNode(
        const std::shared_ptr<actionlib::SimpleActionClient<assignment_2_2023::PlanningAction>> &ac,
        const std::shared_ptr<ros::NodeHandle> &nh)
        : ac_(std::move(ac)), nh_(std::move(nh))
    
    {
        ROS_INFO("Waiting for server to connect");
        ac->waitForServer(ros::Duration(2.0));

        // Subscribers
        sub_target_ = nh_->subscribe("/robot_target", 10, &SetTargetNode::setNewTargetCallback, this);
        sub_state_ = nh_->subscribe("/odom", 10, &SetTargetNode::robotStateCallback, this);
        sub_cancel_ = nh_->subscribe("/robot_cancel_goal", 10, &SetTargetNode::cancelGoalCallback, this);
        sub_scan_ = nh_->subscribe("/scan", 10, &SetTargetNode::scanCallback, this);

        // Publishers
        pub_state_ = nh_->advertise<rt_assignment_2::RobotState>("/robot_state", 10);

        // Timers
        get_actual_pose_timer_ = nh_->createTimer(ros::Duration(0.5), &SetTargetNode::actualPoseCallback, this);    // 2 Hz

        // services
        // This one throws me errors, most probably due to wrong parameter types 
        get_dist_left_obstacle_service_ = nh_->advertiseService("/dist_to_left_obstacle", &SetTargetNode::getDistanceToLeftObstacleCallback, this);


    }


    void SetTargetNode::actualPoseCallback(const ros::TimerEvent &event)
    {
        if (ac_->getState() == actionlib::SimpleClientGoalState::ACTIVE)
        {
            ROS_INFO("Robot's current position x = %f y = %f", feedback_pos_x_, feedback_pos_y_);
        }
    }

    void SetTargetNode::cancelGoalCallback(const rt_assignment_2::RobotCancelGoal::ConstPtr &msg)
    {
        ac_->cancelGoal();
        ROS_INFO("Robot's goal has been canceled");
    }

    void SetTargetNode::setNewTargetCallback(const rt_assignment_2::RobotTarget::ConstPtr &msg)
    {
        ROS_INFO("New Target arrived");
        // set up new goal
        assignment_2_2023::PlanningGoal goal;
        goal.target_pose.pose.position.x = msg->x;
        goal.target_pose.pose.position.y = msg->y;

        ac_->sendGoal(goal,
                      boost::bind(&SetTargetNode::doneCallback, this, _1, _2),
                      boost::bind(&SetTargetNode::activeCallback, this),
                      boost::bind(&SetTargetNode::feedbackCallback, this, _1));
        ROS_INFO("New Goal has been sent");

        // Set the goal to be remembered
        goal_x_ = goal.target_pose.pose.position.x;
        goal_y_ = goal.target_pose.pose.position.y;
    }

    void SetTargetNode::scanCallback(const sensor_msgs::LaserScan::ConstPtr &msg)
    {

        /**
         * def clbk_laser(msg):
    global regions_
    regions_ = {
        'right':  min(min(msg.ranges[0:143]), 10),
        'fright': min(min(msg.ranges[144:287]), 10),
        'front':  min(min(msg.ranges[288:431]), 10),
        'fleft':  min(min(msg.ranges[432:575]), 10),
        'left':   min(min(msg.ranges[576:719]), 10),
    }
        */

        // I dont know if this is correct // found this snippet online and modified it slightly
        int size = msg->ranges.size();
        int minIndex = 432; // I believe this is the range from fleft to left
        int maxIndex = 719; 
        int closestIndex = -1;
        double minVal = 999; 

        for (int i = minIndex; i < maxIndex; i++)
        {
            if ((msg->ranges[i] <= minVal) && (msg->ranges[i] >= msg->range_min) && (msg->ranges[i] <= msg->range_max))
            {
                minVal = msg->ranges[i];
                closestIndex = i;
            }
        }
        distance_to_left_obstacle_ = msg->ranges[closestIndex];
    }
    }

    // I have some errors, most probably
    void SetTargetNode::getDistanceToLeftObstacleCallback(rt_assignment_2::DistLeftObstacle::Request &req,
                                                          rt_assignment_2::DistLeftObstacle::Response &res)
    {
        // When service is called it should get this data
        ROS_INFO("The distance to the left obstacle: [%f]", distance_to_left_obstacle_);
    }

    void SetTargetNode::robotStateCallback(const nav_msgs::Odometry::ConstPtr &msg)
    {
        // Get msg from /odom and create msg to /robot_state
        rt_assignment_2::RobotState robot_state_msg;
        robot_state_msg.x_pos = msg->pose.pose.position.x;
        robot_state_msg.y_pos = msg->pose.pose.position.y;
        robot_state_msg.x_vel = msg->twist.twist.linear.x;
        robot_state_msg.y_vel = msg->twist.twist.linear.y;

        // Publish new robotStage message
        pub_state_.publish(robot_state_msg);
    }

    void SetTargetNode::doneCallback(const actionlib::SimpleClientGoalState &state,
                        const assignment_2_2023::PlanningResultConstPtr &result)
    {
        ROS_INFO("Goal finished in state [%s]", state.toString().c_str());
        ROS_INFO("Select new target by publishing into /robot_target");
    }

    void SetTargetNode::activeCallback()
    {
    }

    void SetTargetNode::feedbackCallback(const assignment_2_2023::PlanningFeedbackConstPtr &feedback)
    {
        feedback_pos_x_ = feedback->actual_pose.position.x;
        feedback_pos_y_ = feedback->actual_pose.position.y;

        // I already have a callback to the feedback, instead of creating new subscriber,
        // I prefer to check if the feedback is close  enough to the goal and point it there
        // Although im more than certain that this will never reach inside, as it might need some thresholds
        if (feedback_pos_x_ == goal_x_ && feedback_pos_x_ == goal_y_)
        {
            ROS_INFO("reached");
        }
    }
}