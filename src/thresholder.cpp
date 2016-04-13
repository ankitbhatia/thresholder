#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/Image.h>
#include <pcl/io/pcd_io.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <vector>
#include <string>
#include <iostream>

typedef pcl::PointCloud<pcl::PointXYZRGB> PointCloud;

//Global Reconfiguration options
double x_threshold =0.5;
double y_threshold = 0.5;
double z_threshold = 0.5;

ros::Publisher pub;
void callback(const PointCloud::ConstPtr& msg)
{

  PointCloud cloud = PointCloud(*msg);

  for(size_t i = 0; i < cloud.points.size(); ++i){
    if (cloud.points[i].x > x_threshold || cloud.points[i].y >y_threshold || cloud.points[i].z >z_threshold)
      cloud.points[i].rgb = 0;
    if (std::isnan(cloud.points[i].x) || std::isnan(cloud.points[i].y) || std::isnan(cloud.points[i].z))
      cloud.points[i].rgb = 0;
  }
  sensor_msgs::Image image;
  try {
    pcl::toROSMsg (cloud, image);
  }
  catch (std::runtime_error e) {
    ROS_ERROR_STREAM("Error in converting cloud to image message: "<<e.what());
  }
  pub.publish(image);
}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "thresholder");
  ros::NodeHandle nh;
  // while(!nh.hasParam("x_threshold"));
  double x, y, z;
  if (nh.getParam("/thresholder/x_threshold", x))
  {
    ROS_INFO("Got param x: %f", x);
  }
  else
  {
    ROS_ERROR("Failed to get param 'x'");
  }
  if (nh.getParam("/thresholder/y_threshold", y))
  {
    ROS_INFO("Got param y: %f", y);
  }
  else
  {
    ROS_ERROR("Failed to get param 'y'");
  }
  if (nh.getParam("/thresholder/z_threshold", z))
  {
    ROS_INFO("Got param z: %f", z);
  }
  else
  {
    ROS_ERROR("Failed to get param 'z'");
  }
  // ROS_INFO("Thresholds: %s %s %s", x.c_str(), y.c_str(), z.c_str()); 
  // x_threshold = (double)atof(x.c_str());
  // y_threshold = (double)atof(y.c_str());
  // z_threshold = (double)atof(z.c_str());
  x_threshold = x;
  y_threshold = y;
  z_threshold = z;

  ROS_INFO("Thresholds: %f %f %f", x_threshold, y_threshold, z_threshold); 
  ros::Subscriber sub = nh.subscribe<PointCloud>("kinect2/sd/points", 1, callback);
  pub = nh.advertise<sensor_msgs::Image>("thresholded",10);
  ros::spin();
}
