# SocketDemoCode
This repository contains demonstration code showing how to communicate via TCP/IP sockets.  The server side will open the file that is specified by the command line provided filename.  The server reads the file for a determined number of characters and waits for the client side socket to connect.  When it connects, the server transmits the file data to the client and the shuts down.  The client side will receive the data and save it to a file called "OutputFile".

To compile the client side:
    gcc -o ClientSocketExample ClientSocketExample.c
    
To compile the server side:
    gcc -o ServerSocketExample ServerSocketExample.c 
    
To run the demo, run the server side first - enter the executable name followed by a port number and some file name.  The following example reads the server C code file:
    ./ServerSocketExample 2222  ServerSocketExample.c    

The server will wait until the client connects.

To run the client - enter the executable name followed by host name and port number of the server.  When ready to initiate the transfer hit any key when prompted:
    ./ClientSocketExample 127.0.0.1 2222
    
Use "ls" to list the contents of the directory.  The file "OutputFile" will be listed to verify the transfer operation.
