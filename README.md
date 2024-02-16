# SeekurROS
## Thesis: Integration of mobile robot Seekur Jr with Robot Operating System using TCP

Presented thesis concerns the implementation of the solution for programming the mobile robot Seekur Jr with the Robot Operating System (ROS). Such integration allows to create advanced control algorithms and systems with the use of this kind of robot and also gives the possibilities for further research and development on Seekur Jr. 

Because of the old and incompatibile infrastructure on robot, this integration had to be with the use of external computer with installed ROS.

The repository consists of two folders - Robot_app and ROS_app. Ultimately, they have programs that should be started separatly on robot's computer and the external one with installed ROS2 (here used Humble Hawksbill distribution).

### Robot_app:
- *robot.cpp* - program for controlling the robot and collect data from it
- *server.cpp* - program for connecting with ROS on the external computer using TCP protocol

### ROS_app:
This is single ROS package with three programs, wrote in C++. Each of them is a TCP client for different kind of data - laser scan, position coordinates and move command.
