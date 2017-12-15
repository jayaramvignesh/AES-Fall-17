#Project Name: Designated Driver 

#Project Authors: Vignesh Jayaram
                  Arundhathi Swami 
                  University of Colorado Boulder 

#BOARDS, PROCESSOR AND OS USED
1. TIVA TM4C 1294XL - ARM M4 - FreeRTOS
2. BEAGLEBONE GREEN - ARM A8 - UBUNTU

#SENSORS USED 
1. MQ3 - Alcohol Sensor 
2. ADXL345 - Accelerometer 
3. HS-RC04 - Ultrasonic Sensor 

#COMMUNICATION INTERFACES
1. UART
2. I2C

#KERNEL LEVEL MODULES
1. CHARACTER DRIVER
2. LED DRIVER 

#FEATURES 
1. Reads sensor values on TIVA 
2. Creates a packet containing Data 
3. Sends it to BBG 
4. stores received values in a log file also determining whether driver is toointoxicated to drive, monitors distance to nearest collision and the motion of the vehicle.  
5. Heart beat system to monitor tasks on both boards 
6. Multiple tasks(FreeRTOS xTasks) on TIVA
7. Multiple pthreads (POSIX) on BBG
8. TIVA FreeRTOS Sync, InterTask Messaging and Locking Devices: xTaskNotify, Semaphores, Mutex, Queues 
9. BBG Sync, InterThread Messaging and Locking Devices: POSIX Semaphores, Condition Variables, Queues, Mutex 
10. Timers Implemented on both sides to monitor heartbeats and schedule Tasks in a predetermined order 
11. LED implemented via a self-written character driver to access BBG LEDs to signify alert levels and error occurences. 

#Initialization and execution commands
0. SSH into BBG and open CCS for TIVA
> ssh ubuntu@192.168.7.2

1. set up led config pins (run as root)
> echo leds > /sys/class/leds/beaglebone\:green\:usr0/device/driver/unbind

2. set up uart config pins (run as root)
>config-pin P9.11 uart
>config-pin P9.13 uart

3. load char driver kernel module 
> sudo su
> insmod ebbchar.ko
> lsmod

4. make 

5. start execuatble on bbg 
> ./project_2

6. start executable on tiva tm4c 1294

7. remove char driver kernel module 
> rmmod ebbchar.ko

8. check kernel log 
> dmesg 

9. log file 
> vim log.txt

#REFERENCES: 
1. Software ARchitecture Block Diagram 
2. Project Proposal Data Sheet
3. Unit Tests for Sub-Modules 
