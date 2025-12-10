# OVERVIEW

Authored by: Shawn Hanlon & Carlos Morera Pinilla

Some of this script is adapted from A3 code by Carlos Morera Pinilla and Pavol Federl.

This Project has been created to demonstrate a heap overflow.

## Parts of the system

1 - Server 

2 - Client

3 - Exploit

## HOW TO RUN THE SERVER:

The server can be run from inside the server folder.

## NEED TO FINISH FROM HERE!!!

### 1. Clone the repository to your Linux Machine:
```bash
git clone https://github.com/Cdmpc/CPSC-525-Final-Project.git
```
```bash
cd CPSC-525-Final-Project
```

### 2. Compile the C-programs for the Client and Server
```bash
g++ -std=c++20 -Wall -o ./Server/server ./Server/server.cpp
```
```bash
g++ -std=c++20 -Wall -o ./Client/client ./Client/client.cpp
```

### 2.5: Start the SERVER PROGRAM FIRST (Example PORT 7000)
```
./Server/server 7000
```

If you see this: 
```
LOG::port : 7000
```
The server is connected and ready to go.

### 3. Run the Python Exploit Script, using example 127.0.0.1 and port 7000
#### NOTE: Use the SAME port number for the Client that you started the Server with.
```bash
python3 ./Client/user-gen.py 127.0.0.1 7000
```

If you do not provide command-line arguements to the Python script, this prompt will appear.
```bash
CAUGHT ERROR: main() function failed, did you try:
"argv[1] == IP ADDRESS (127.0.0.1 for example)"
"argv[2] == PORT NUMBER (Ex: 5400)"
"Usage: python3 ./Exploit/heap-exploit.py argv[1] argv[2]"
```
### 4. Follow the prompt:
```
============================== [EXPLOIT START] ===================================

How many users do you want to create (MAXIMUM 100): 45
45 users created, returning from function...
create_N_notes() succeeded, please check ./Secrets directory.

============================== [EXPLOIT TERMINATED] ==============================
```
The passwords, will be psuedo-random 20 character strings of range [0-9A-Za-z]

### 5. The secret files will be located in a created directory called ./Secrets, where your current working directory is the repository's directory.

The secret files will have the name format "userN.txt" and contain the following format:
```
[N] ==> {30 psuedorandom strings of range [0-9A-Za-z]}
```

## NOTE: Limitation, the Client can only run once, subsequent runs of the Client/automation script will cause it to fail. To run it successfuly again, you must manually close and restart the Server, then run the Client again once the Server restarts.

### 6. In the Exploit directory, a file called hash-repo.txt will be created.
It will have the following format: 
```
HASH OF user1.txt ==> [SHA256 HASH OF USER1]

HASH OF user2.txt ==> [SHA256 HASH OF USER2]

HASH OF user3.txt ==> [SHA256 HASH OF USER3]
```
These hashes will be used to verify that we have indeed gotten the correct file when we run the exploit, and did not just hardcode it.