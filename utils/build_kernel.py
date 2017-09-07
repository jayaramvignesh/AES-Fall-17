import subprocess

subprocess.call("sudo apt-get install git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc",shell=True,stdout=None)

subprocess.call("cp /boot/config-$(uname -r) .config",shell=True,stdout=None)
subprocess.call("make oldconfig",shell=True,stdout=None)

subprocess.call("sudo make -j 4 && sudo make modules_install -j 4 && sudo make install -j 4",shell=True,stdout=None)

subprocess.call("sudo update-initramfs -c -k 4.12.10",shell=True,stdout=None)

subprocess.call("sudo update-grub",shell=True,stdout=None)
