FROM osrf/ros:iron-desktop-full

RUN apt-get update && apt-get install -y libiio-dev libiio-utils python3-pip ros-iron-plotjuggler-ros 
RUN python3 -m pip install pyadi-iio

WORKDIR /ros2_ws
