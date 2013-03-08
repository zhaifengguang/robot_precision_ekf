/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2013, EJ Kreinar, Case Western Reserve University
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
*   * Neither the name of the Willow Garage nor the names of its
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

// Based loosely on the robot_pose_ekf program by Wim Meeussen
//  and Willow Garage

#ifndef __ROBOT_PRECISION_EKF_NODE__
#define __ROBOT_PRECISION_EKF_NODE__

// ros stuff
#include <ros/ros.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include "robot_precision_ekf.h"
//#include <robot_pose_ekf/GetStatus.h>

// messages
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"

#include <boost/thread/mutex.hpp>

// log files
#include <fstream>

/** \mainpage
 *  \htmlinclude manifest.html
 * 
 * <b>Package Summary</b>
 * This package provides two main classes: 
 *  1) RobotPrecisionEKF performs all sensor fusion operations, and 
 *  2) RobotPrecisionEKFNode provides a ROS wrapper around RobotPrecisionEKF
*/

typedef boost::shared_ptr<nav_msgs::Odometry const> OdomConstPtr;
typedef boost::shared_ptr<sensor_msgs::Imu const> ImuConstPtr;
typedef boost::shared_ptr<geometry_msgs::PoseStamped const> GpsConstPtr;

class RobotPrecisionEKFNode
{
public:
  /// constructor
  RobotPrecisionEKFNode();

  /// destructor
  virtual ~RobotPrecisionEKFNode();

private:
  /// the mail filter loop that will be called periodically
  void spin(const ros::TimerEvent& e);
  
  /// Publishes the Filter solution
  void publish();

  /// callback function for gps data
  void gpsCallback(const GpsConstPtr& gps);
  
  /// callback function for odo data
  void odomCallback(const OdomConstPtr& odom);

  /// callback function for imu data
  void imuCallback(const ImuConstPtr& imu);

  ros::NodeHandle node_;
  ros::Timer timer_;
  ros::Publisher pose_pub_;
  ros::Subscriber odom_sub_, imu_sub_, gps_sub_;
  ros::ServiceServer state_srv_;

  // ekf filter
  RobotPrecisionEKF* ekf_filter_;

  // estimated robot pose message to send
  geometry_msgs::PoseWithCovarianceStamped  output_; 
  // TODO: Also send the transform from map frame to Odom

  // Transform handlers
  tf::TransformBroadcaster* tfb_;
  tf::TransformListener* tf_;
  
  // frame names
  std::string global_frame_id_, odom_frame_id_, base_frame_id_;

  // variables
  ros::Duration transform_tolerance_;
  tf::Transform latest_tf_;
  ros::Time odom_time_, imu_time_, gps_time_;
  ros::Time odom_stamp_, imu_stamp_, gps_stamp_, filter_stamp_;
  bool odom_used_, imu_used_, gps_used_;
  double timeout_;
  bool debug_, self_diagnose_;
  /*
  tf::Transform odom_meas_, imu_meas_, gps_meas_;
  tf::Transform base_gps_init_;
  tf::StampedTransform camera_base_;
  ros::Time odom_init_stamp_, imu_init_stamp_, gps_init_stamp_;
  bool odom_active_, imu_active_, gps_active_;
  bool odom_initializing_, imu_initializing_, gps_initializing_;
  MatrixWrapper::SymmetricMatrix odom_covariance_, imu_covariance_, gps_covariance_;
  */

  // log files for debugging
  std::ofstream odom_file_, imu_file_, gps_file_, corr_file_, time_file_, extra_file_;

  // counters
  unsigned int odom_callback_counter_, imu_callback_counter_, gps_callback_counter_, ekf_sent_counter_;

}; // class

#endif
