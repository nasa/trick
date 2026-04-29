## 1. Supported ROS Version

These Trick-based simulations embed native ROS nodes using `rclcpp`.

**Required ROS distribution:**
- **ROS 2 Humble**
- Default DDS: **Fast DDS** (`rmw_fastrtps_cpp`)

**Tested on:**
- RHEL 8
- GCC toolsets 11 and newer
- C++ 17 standard

ROS is expected to be installed under:

```text
/opt/ros/humble
```

# Installing ROS Humble on RHEL 8 via `dnf`

## 1. Enable the EPEL repository (if not already enabled)

```bash
sudo dnf install epel-release -y
```
## 2. Install required dependencies
```bash
sudo dnf install dnf-plugins-core curl gnupg2 -y
```
## 3. Install latest ROS Humble package from github
```bash
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest \
  | grep -F "tag_name" | awk -F\" '{print $4}')

sudo dnf install "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-release-${ROS_APT_SOURCE_VERSION}-1.noarch.rpm"
```
## 4. Install ROS Humble desktop
```bash
sudo dnf install ros-humble-desktop -y
```
## 5. Required Environment Setup (Every Shell)

Before **building or running** any Trick simulation that uses ROS 2, you must source the ROS environment:
```bash
source /opt/ros/humble/setup.bash
```
