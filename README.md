# OpenRTM_aist_amqpcpp_interface

This software is a set of extension modules for OpenRTM-aist C++ developed by National Institute of Advanced Industrial Science and Technology (AIST) to realize communication using **AMQP (Advanced Message Queuing Protocol)** protocol in robot systems built with RT-middleware, one of the robot middleware. 

AMQP is one of the IoT protocols that uses the same broker architecture as MQTT. Compared to MQTT, which is a lightweight protocol, AMQP has a more complex specification, but it can provide highly functional and reliable messaging.

By extending OpenRTM-aist with this group of modules, AMQP can be added to the interface-type of data ports of RT-Components. Normally, OpenRTM-aist uses CORBA as the interface-type, but by selecting AMQP as the communication interface, communication between data ports can be performed using AMQP.

In addition to AMQP, the RT-Component can be co-located with conventional data ports such as CORBA and shared memory, making it possible to represent the entire IoRT (Internet of Robotic Things) using only OpenRTM-aist.

## Requirement
 
To install and use the AMQP communication module, you will need the C++ version of OpenRTM-aist, the AMQP client library AMQP-CPP, and the asynchronous processing library libev.
 
* OpenRTM-aist-C++ ver. 1.2.x
* AMQP-CPP
* libev

You will need one of the AMQP brokers to actually use AMQP to communicate between the data ports of the RT-Components. If you do not use an external messaging service such as an online IoT platform, but prepare your own, please install the OSS broker beforehand. Please note that this communication module has been tested with RabbitMQ server.
 
## Installation

The following assumes installation on each Linux distribution, Ubuntu or debian.

### How to install when using OpenRTM-aist of ver.1.2.x

(1) For details on how to install OpenRTM-aist-C++, please refer to the official OpenRTM-aist website of the National Institute of Advanced Industrial Science and Technology (AIST). We omit it here.

(2) Install the AMQP client library, AMQP-CPP, by downloading the source code from the [AMQP-CPP GitHub repository](https://github.com/CopernicaMarketingSoftware/AMQP-CPP "AMQP-CPP official web site"). Please refer to the README in the GitHub repository of AMQP-CPP for installation instructions.

(3) Install libev, a library that handles asynchronous processing.
```bash
$ sudo apt install libev-dev
```

(4) Clone the files related to AMQP communication modules from this site and install the AMQP communication modules.
```bash
$ cd ~/
$ git clone https://github.com/dyubicuoa/OpenRTM_aist_amqpcpp_interface
$ cd OpenRTM_aist_amqpcpp_interface
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This will create **OutPortAmqpcppProducer.so**, an AMQP transmitter module, and **InPortAmqpcppConsumer.so**, an AMQP receiver module, under `~/OpenRTM_aist_amqpcpp_interface/build/src`.

(5) If necessary, install one of the AMQP-enabled broker software; RabbitMQ ｓerver is recommended, see the RabbitMQ official website for installation instructions. As RabbitMQ is an erlang implementation, it also requires a set of erlang-related packages, which makes the installation a bit more complicated.
