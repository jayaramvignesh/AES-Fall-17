#!/usr/bin/env python

import os
import subprocess

os.chdir('/home/vigi/linux-4.12.10')
subprocess.call("sudo apt-get install git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc",shell=True,stdout=None)

subprocess.call("cp /boot/config-$(uname -r) .config",shell=True,stdout=None)
subprocess.call("make oldconfig",shell=True,stdout=None)

subprocess.call("sudo make -j4 && sudo make modules_install -j4 && sudo make install -j4",shell=True,stdout=None)

subprocess.call("sudo update-initramfs -c -k 4.12.10",shell=True,stdout=None)

subprocess.call("sudo update-grub",shell=True,stdout=None)
