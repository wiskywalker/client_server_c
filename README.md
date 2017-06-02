# client_server_c
Client - Server Model programmed on C


--------------------------------------------------------
JOSE WILFREDO ALEMAN ESPINOZA

Computer information: 

  Model Identifier:	MacBookPro12,1
  Processor Name:	Intel Core i5
  Processor Speed:	2.7 GHz
  Number of Processors:	1
  Total Number of Cores:	2
  L2 Cache (per Core):	256 KB
  L3 Cache:	3 MB
  Memory:	8 GB
  Operative System: MacOS Sierra version 10.12.4 (16E195)


--------------------------------------------------------
Client - Server 
Simple communication protocol

Programming Language: C. 

Based on the Appendix B: Network programming of the reference book on Software Engineering by Prof Ivan Marsic. 

IP Address and Port are fixed in the code. 

IP Address: 127.0.0.1
Port: 5100

--------------------------------------------------------

To compile:

CLIENT

    gcc -o client client.c

SERVER 

    gcc -o server server.c
    

--------------------------------------------------------


To execute:

CLIENT

    ./client

SERVER 

    ./server
    

--------------------------------------------------------

Functionality: 

> GET or get: 

Will ask the user for a <file.txt>, will display the content of the file. 

Example: 
>get
>Filename to get: song.txt
>Server says:
"Bored To Death"....

> BOUNCE or bounce: 

Will prompt the user for a message, will send the message and display the server response. 

> Exit or exit;

Default exit, will return "Client disconnected."

> Exit <code> or exit <code>:

Will send exit with code and receive server message with the code. 


--------------------------------------------------------
