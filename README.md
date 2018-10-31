# motionAPI64
C++ and C# Examples for motionAPI, replaces KMAPI.
Supported by firmware versions:<br/>
- ***0.4.17.5.x***<br/>
- ***0.4.19.x*** and above<br/>

***Requires ControlStudio 3.0.0 or newer<br/>***
Communication with softMC via TCP/IP ports 4001, 4003, 4004 and 4005 instead of legacy 5001, 5003, 5004 and 5005<br/>
~~RBOOTP~~ is **discontinued**<br/>

> Please check project Wiki
      
# Purpose of Motion API

Motion API is a library that enables communication with softMC motion controller via TCP/IP socket connection via port 4001. MotionAPI provides an access to interactive command line interface (CLI), which allows execution of commands, loading and controlling of tasks and many other services that are available from terminal window of ControlStudio .
Library supports multiple connection, each new connection creates a separate service thread on motion controller. 

Motion API provides following services:

 - Send files to motion controller
 - Retrieve files from motion controller
 - Scan network (subnet) and get a list of motion controllers. 
 - Send a command to motion controller and get a response.

# Simple Workflow

 
|  |  |
|--|--|
|Create API instance  |  [KMNewKMAPI](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#ace4118c6ba20659c52106507061756cb) |
|Connect to controller|[Connect](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#ad165eca24e7f8c8dba9277ab9bd5ffdc)
|Execute command|[SendCommand](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#a00dff298d0ce9b95e17d10faf9703a34)
|Disconnect and free resources|[MCFreeKMApi](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#a3ddb56384a6d0d3f4c21a3669bff6333)
# Handing asynchronous messages and Error messages
|  |  |
|--|--|
| Getting notification about message or error  | [SetCallback](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#a8271a6182ae771e82f917f744ec92cc4) |
 
# File transfer functions
|  |  |
|--|--|
| Send file to softMC| [UploadFile](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#aa964ca34d2f52c7c10716ec8e9ec80b2) |
|Receive File from softMC|[DownLoadFile](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#a89d3b11e4b75e3c154a86ae25a06adee)
# Network scan
|  |  |
|--|--|
| Scan network | [DetectDevices](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#a198301566229dca02927ada1c44e82ca) |
| Get list of devices|[GetDeviceList](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#ae47dad290a55c5d4ec50ff9d310c97a6)

# Limitations
Motion API is designed for setup and monitoring purposes rather for real-time.
Execution and interpretation of each command can take up to several mSecs (depending on softMC model). 
Refer to  [softMC manual/wiki](http://softmc.servotronix.com/wiki/Welcome) for list of available commands and limitation for commands issued via command line interface. All limitations also apply to commands issued via ([ExecCommand](https://cdn.rawgit.com/stx-sas/motionAPI64/dbfc2a5d/Examples/example1/DemoMCCpp/DemoMCCpp/html/_k_m_api_8h.html#a62116030d92671325976b45c9b93a14b)) .

# Extensions 
Every expression sent via *ExecCommand* is evaluated and executed by softMC Command Line Interface.
Every global variable can be set or read with this mechanism.
Public function implemented in global libraries also can be called. In order to eliminate blocking of *ExecCommand*, library function shall not contain blocking calls or loops.
# Advanced function
Library allows password authentication when password is set on softMC.
