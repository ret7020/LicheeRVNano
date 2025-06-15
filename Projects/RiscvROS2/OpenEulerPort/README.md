# Port RISC-V OpenEuler

Prepare x86 enviroment for RISC-V 64 emulation:

Example for Arch Linux:

```bash
sudo pacman -S qemu-user-static binfmt-support
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
```

Pull container (https://hub.docker.com/layers/xfan1024/openeuler/24.03-lts-riscv64/images/sha256-9cd3a3060f7ef8c157ebc6475ae42af23640978ccb354379b391f9021fad09d7):

```bash
docker pull xfan1024/openeuler:24.03-lts-riscv64
```

Run OpenEuler RISC-V bash:

```
docker run --rm -it --platform linux/riscv64 xfan1024/openeuler:24.03-lts-riscv64 bash
```

Install ROS2 Humble inside container:

```bash
cat << EOF > /etc/yum.repos.d/ROS.repo
[openEulerROS-humble]
name=openEulerROS-humble
baseurl=https://build-repo.tarsier-infra.com/openEuler:/ROS/23.09/
enabled=1
gpgcheck=0
EOF'
```

Install ROS base packages:

```bash
dnf install ros-humble-ros-base
```

Optional install cyclone dds:

```bash
dnf install ros-humble-rmw-cyclonedds-cpp
```
