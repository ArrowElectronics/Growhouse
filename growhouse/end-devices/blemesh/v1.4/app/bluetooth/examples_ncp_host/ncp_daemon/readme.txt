Instructions for ncp_daemon and mesh-secure-ncp examples

Components for secure BGAPI communication 
1. ncp_daemon host example
All security logic is handled by this security component on the host side.

2. mesh-secure-ncp host example
Example host application for demonstrating secure BGAPI communication.

3. NCP-Secure NCP Mesh Empty Target sample app
NCP Target sample app 

Security of BGAPI is implemented by encrypting the communication between NCP target and Host. To minimize the changes needed for the host application the security is implemented in a separate component (NCP Daemon). 
The host application (mesh-secure-ncp) runs in the separate task from the security component. This allows the different applications accessing the secure NCP easily. All security logic is handled by this security component.

Prerequisites for setting up secure BGAPI communication:
- You must have a Linux or Cygwin platform 
- You must have openssl-devel package installed

Steps for setting up secure BGAPI communication

1.    NCP – Secure Mesh NCP Empty Target Sample app must be programmed to the EFR32 chip
    - Connect your WSTK to the PC
    - Open Simplicity Studio (with Bt Mesh SDK installed)
    - Select NCP-Secure NCP Mesh Empty Target sample app from Demos to flash your EFR32 device
    
2.   Compile and start ncp_daemon in a new terminal
    - Open a new terminal (cygwin terminal in windows platform)
    - Navigate to app/bluetooth/examples_ncp_host/ncp_daemon in the Bt Mesh SDK
    - Build the sample app by typing 'make'
    - run the sample app with appropriate parameters (eg  exe/ncp_daemon.exe /dev/ttyS13 115200 encrypted unencrypted )
        1st parameter: serial port
        2nd parameter: serial port speed
        3rd parameter: file descriptor for encrypted domain socket (it can be any string)
        4th parameter: file descriptor for unencrypted domain socket (it can be any string)
        
3.    Compile and start mesh-secure-ncp in a new terminal
    - Open a new terminal (cygwin terminal in windows platform)
    - Navigate to app/bluetooth/examples_ncp_host/mesh-secure-ncp in the Bt Mesh SDK
    - Build the sample app by typing 'make'
    - run the sample app with appropriate parameters (eg ./exe/empty.exe ../ncp_daemon/encrypted 1 )
        1st parameter: file descriptor for encrypted or unencrypted  domain socket (it can be any string, but must match appropriate file descriptor string used in Step 2)
        2nd parameter: Is the domain socket entered in 1st parameter encrypted (1) or unencrypted(0)    
    - After running mesh-secure-ncp the 'Reset event' and the 'Node Initialized event' should arrive