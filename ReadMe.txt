Contents:
Client:
clientdr.cpp and clientdr.h are the source code for the "client" that measures the data rate.
Server:
serverdr.cpp is the source code of "server" that sends data to the client and is also connected to remote server
Remote_server:
remote_serverdr.cpp is the source code of the "remote server" that takes the desired data rate and reads the email update about measured data rate from the client and sends a message to the "server"
For sending and reading email update:
"email_send.py" and "email_read.py" are the python scripts for sending the email update and for reading the email update respectively.
"email_send.py" is embedded inside the "clientdr.cpp" for sending an email with measured data rate and "email_read.py" is embedded inside the "remote_serverdr.cpp" for reading the email sent by the client.

Type "make" for compiling all the source code and for generating the corresponding executables.
make will generate the following executables:
./clientdr.out
./serverdr.out
./remote_serverdr.out

Execution:
1. First start the server to listen to the client by typing the following command:

"./serverdr.out" 

2. Next start the client as follows by giving the IP address of the server started before:

"./clientdr.out  ip.address.of.server"

-The above commands will result in series of data exchanges between the server and client, and client will 
measure the data rate and send an email with the measured data rate and stops Execution. The measured data rate is displayed on the console before terminating the execution.

3. Next start the remote server as follows by entering the IP address of the server above and the desired data rate: (Please enter the desired data rate that is less than the measured data rate)

"./remote_serverdr.out  ip.address.of.server  Desired_Data_Rate"

-Now the "remote_server" reads the email sent by the client in the previous step and calculates a time delay for server based on the desired data rate entered and sends the time delay update to the server using a TCP connection.
-The server receives the update and displays it on the console.

4. Now start the client again by typing  "./clientdr.out  ip.address.of.server". The measured data rate is displayed on the console and it is lesser than the data rate that is measured earlier and closer to the value of desired data rate.

**For every new test, please close the previous "./serverdr.out" and "./remoteserverdr.out" and restart them again. 

Note: 
-Please make sure to execute the steps in the same order as above.
-Please make sure that desired data rate entered is less than the measured data rate.
-If there is a problem, please make sure to close all the three terminals and restart them again.
