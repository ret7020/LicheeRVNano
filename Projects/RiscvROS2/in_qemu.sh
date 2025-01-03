sudo apt update
sudo apt install \
    bison \
    cmake \
    curl \
    libasio-dev \
    libbullet-dev \
    libcunit1-dev \
    libcurl4-openssl-dev \
    libeigen3-dev \
    liblog4cxx-dev \
    libopencv-dev \
    libtinyxml2-dev \
    python3-dev \
    python3-netifaces \
    python3-numpy \
    python3-setuptools \
    python3-yaml \
    python3-pip \
    libcppunit-dev python3-sip-dev python3-psutil python3-future \
    libacl1-dev \
    unzip

pip3 uninstall empy
pip3 install empy==3.3.4
pip3 install rosinstall_generator colcon-common-extensions vcstool lark-parser packaging rosdep

wget https://github.com/ros2/ros2/archive/refs/tags/release-humble-20241205.zip
unzip release-humble-20241205.zip
cd ./ros2-release-humble-20241205
mkdir src

vcs import --input ros2.repos src

# Disable build for next packages
touch src/ros-visualization/COLCON_IGNORE
touch src/ros/ros_tutorials/turtlesim/COLCON_IGNORE
touch src/ros2/demos/image_tools/COLCON_IGNORE
touch src/ros2/demos/intra_process_demo/COLCON_IGNORE
touch src/ros2/rviz/COLCON_IGNORE

