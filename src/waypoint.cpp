#include "ros/ros.h"
#include "tf/tf.h"
#include "eigen3/Eigen/Dense"

#include <std_msgs/Float64.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/AccelStamped.h>

#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <visualization_msgs/Marker.h>

#include <stdlib.h>


Eigen::Vector2d wanted_position = {0.0, 0.0};
Eigen::Vector2d wanted_speed = {0.0, 0.0};
Eigen::Vector2d wanted_acceleration = {0.0, 0.0};


void posCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    wanted_position[0]  = wanted_position[0] + msg->linear.x;
    wanted_position[1]= wanted_position[1]+msg->angular.z;
}

int main(int argc, char **argv) {
    // ROS Node declaration
    ros::init(argc, argv, "waypoint_node");
    ros::NodeHandle n;
    ros::NodeHandle n_private("~");

    // Parameters
    wanted_position[0] = n_private.param<double>("x0", 0.0);
    wanted_position[1] = n_private.param<double>("y0", 0.0);

    // Publisher and Subscriber
    ros::Publisher position_publisher = n.advertise<geometry_msgs::PointStamped>("wanted_position", 0);
    ros::Publisher speed_publisher = n.advertise<geometry_msgs::TwistStamped>("wanted_speed", 0);
    ros::Publisher acceleration_publisher = n.advertise<geometry_msgs::AccelStamped>("wanted_acceleration", 0);
    tf2_ros::TransformBroadcaster tf_broadcaster;
    ros::Publisher visualization_publisher = n.advertise<visualization_msgs::Marker>("/target_marker", 0);

    // Subscriber
    ros::Subscriber position_suscribe = n.subscribe("position", 1000, &posCallback);

    // Visualization Message
    visualization_msgs::Marker marker;
    marker.header.frame_id = "map";
    marker.id = 0;
    marker.type = visualization_msgs::Marker::SPHERE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 1.0;
    marker.scale.y = 1.0;
    marker.scale.z = 1.0;
    marker.color.a = 1.0;
    marker.color.r = 1.0;
    marker.color.g = 0.0;
    marker.color.b = 0.0;

    // Wanted position message
    geometry_msgs::PointStamped w_point;
    w_point.header.frame_id = "map";
    w_point.point.z = 0.0;

    // Wanted speed message
    geometry_msgs::TwistStamped w_twist;
    w_twist.twist.linear.z = 0.0;

    // Wanted acceleration message
    geometry_msgs::AccelStamped w_accel;
    w_accel.accel.linear.z = 0.0;

    // Setting the loop rate
    ros::Rate loop_rate(2);

    // Loop 
    while (ros::ok()) {
        // Getting the incomming messages
        ros::spinOnce();


        // Wanted position message
        w_point.point.x = wanted_position[0];
        w_point.point.y = wanted_position[1];
        position_publisher.publish(w_point);

        // Wanted speed message
        w_twist.twist.linear.x = wanted_speed[0];
        w_twist.twist.linear.y = wanted_speed[1];
        speed_publisher.publish(w_twist);

        // Wanted acceleration message
        w_accel.accel.linear.x = wanted_acceleration[0];
        w_accel.accel.linear.y = wanted_acceleration[1];
        acceleration_publisher.publish(w_accel);

        // Visualization Message
        marker.header.stamp = ros::Time();
        marker.pose.position.x = wanted_position[0];
        marker.pose.position.y = wanted_position[1];
        visualization_publisher.publish(marker);

        loop_rate.sleep();
    }
    return 0;
}