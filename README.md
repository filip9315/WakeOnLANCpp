# Remote Wake On LAN

Wake your home computer from anywhere in the world.

### Installation
WakeOnLANCpp consists of two programs: client and server.

The server must be run on a computer in the same network as the PC you want to wake. 
The best option is to install it as a docker container on a Raspberry Pi or a NAS server inside your home network. There is a dockerfile in the server directory for building an image.

The client can run on any computer as a C++ program.

Before building, you have to create a `secrets.cmake` file in server and client directories with necessary api keys and database urls:

``` cmake
set(NGROK_AUTHTOKEN "YOUR_AUTHTOKEN")
set(MONGODB_URL "MONGODB_URL")
```

How to get mongodb cluster url: https://www.mongodb.com/docs/atlas/tutorial/connect-to-your-cluster/

How to get ngrok authtoken: https://dashboard.ngrok.com/get-started/your-authtoken


### Usage

After starting the client in the console, you will see 4 options:
1. Wake a PC
2. Add a PC to the list
3. Remove a PC from the list
4. Exit

Before first use, you must first enter the MAC address of the host (PC you want to wake). 
In order to do that, choose option 2. Then follow the instrucions diplayed on the screen.

To wake a PC, choose option 1 and then enter the number corresponding to the MAC address of the computer you want to wake 
(the addresses will be displayed on the screen).

To be able to wake a PC remotely, the PC must be configured to accept wake on lan packets.
