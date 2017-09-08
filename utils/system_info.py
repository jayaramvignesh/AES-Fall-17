import platform
import sys
import os
import sys

endian = sys.byteorder

arch = platform.architecture()
architecture = arch[0]

uname_info = platform.uname()
kernel_build_time = uname_info[3]


version_file = open('/proc/version')
content = version_file.read()

data = content.split(' ')

os_type = data[0]

kernel_version = data[2]

space = ' '
gcc_version = data[4] + space + data[5] + space + data[6]
gcc = gcc_version.replace("(","")
print gcc

os_dist = platform.dist()
dist = os_dist[0]


f = open('system_info.txt','w')
f.write('\nOperating System Type : ' + os_type )
f.write('\nOS distribution : ' + dist)
f.write('\nKernel version : ' + kernel_version)
f.write('\nKernel Gcc version build : ' + gcc)
f.write('\nKernel Build time : ' + kernel_build_time)
f.write('\nSytem information : ' +architecture)
f.write('\nArchitecture is : ' +endian + ' endian ')
f.close()
