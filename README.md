# SECRET NOTE APP WITH HEAP-BASED BUFFER OVERFLOW VULNERABILITY - F25 CPSC 525 FINAL PROJECT

# STAGE: (EXPLOIT IS FINISHED, JUST NEED TO MOVE SOME FILES AROUND.)

### Authored by: Shawn Hanlon & Carlos Morera Pinilla

### This application is for educational purposes ONLY, to demonstrate CWE-122, Heap-Based Buffer Overflow.

### We DO NOT endorse or encourage using this application or any ideas taken from it, to perform any real-world heap-buffer exploits on real-world systems.

# Parts of the system

1 - Server 

2 - Client

3 - Exploit

# HOW TO RUN THE APPLICATION:

## 1. Clone the repository to your Linux Machine:
```bash
git clone https://github.com/Cdmpc/CPSC-525-Final-Project.git
```
```bash
cd CPSC-525-Final-Project 
```

## 2. Compile the C-programs for the Client, Server and Exploit
```bash
g++ -std=c++20 -Wall -o ./Server/server ./Server/server.cpp
```
```bash
g++ -std=c++20 -Wall -o ./Client/client ./Client/client.cpp
```
```bash
g++ -std=c++20 -Wall -o ./Exploit/exploit ./Exploit/exploit.cpp
```

## 3: Start the SERVER PROGRAM FIRST (Example PORT 7000)
NOTE: If you have a directory called Secrets, delete it.
```
cd Server
./server 7000
```

If you see this (Secrets directory will be created): 
```
LOG::port : 7000
LOG::file Secrets created
```
The server is connected and ready to go. Secrets is a sub-directory in Server directory.

## (OPTIONAL) 4. Normal Client Interaction (Ex: Port 7000)
Open another terminal and enter the following commands.
```bash
cd Client
./client 127.0.0.1 7000
```

You will be greeted by this interface:
```
LOG::object created
Welcome to SUPER DUPER SECURE NOTES ...
Store your deepest darkest secrets with us!

Do you already have and account (Y/N) ? :
```

Type "N"

```
Please enter your username:
```
Type any username (must be between 1-31 characters)

```
Please enter your password:
```
Type any password (must be at least 8-31 characters long)

After that, you will see this interface, and a temp.txt file will be created in the current working directory. This is where you will write your secret before it is saved to the server.

```
Your secret can be writen in your temp.txt file, remember to save when done

UPLOAD TO SERVER AND EXIT (1)
UPDATE USERNAME           (2)
UPDATE PASSWORD           (3)
(1, 2, 3) ? :
```
Write something to the temp.txt file, save that file (Ctrl + S), and then Enter "1" on the client terminal.

In the Secrets created directory, a file called user1.txt will appear with your secret you typed inside, temp.txt will be deleted, and the client will close.

## 5. (OPTIONAL) Updating username or password.
From here: 
```
Your secret can be writen in your temp.txt file, remember to save when done

UPLOAD TO SERVER AND EXIT (1)
UPDATE USERNAME           (2)
UPDATE PASSWORD           (3)
(1, 2, 3) ? :
```
If you enter 2, you will be prompted to enter your new username. Type a new username within the boundaries stated above, and hit enter and you will recieve a log confirming.

```
Please enter your new username: carlos2
LOG::carlos2
LOG::USERNAME carlos password *carlos2
LOG::bytes sent : 33
LOG::response : STORED
LOG::UPDATE USERNAME...now returning
```
The same steps apply for option (3)

## 6. PYTHON USER-GEN and HASHING
This step can be done even if you do not do steps 4 and 5.
This Python script is meant to automate the creation of 100 users
to the server.
```bash
cd Client # If you are not in Client directory yet.
python3 ./user-gen.py 127.0.0.1 7000
```

If you do not provide command-line arguements to the Python script, this prompt will appear.
```bash
CAUGHT ERROR: main() function failed, did you try:
"argv[1] == IP ADDRESS (127.0.0.1 for example)"
"argv[2] == PORT NUMBER (Ex: 5400)"
"Usage: python3 ./Exploit/heap-exploit.py argv[1] argv[2]"
```
### a. Follow the prompt:
```
============================== [EXPLOIT START] ===================================

How many users do you want to create (MAXIMUM 100): 45
45 users created, returning from function...
create_N_notes() succeeded, please check ./Secrets directory.

============================== [EXPLOIT TERMINATED] ==============================
```
The passwords, will be psuedo-random 20 character strings of range [0-9A-Za-z]

### b. The secret files will be located in a created directory called ./Secrets, where your current working directory is the repository's directory.

The secret files will have the name format "userN.txt" and contain the following format:
```
[N] ==> {30 psuedorandom strings of range [0-9A-Za-z]}
```

## NOTE: Limitation, the Client can only run once, subsequent runs of the Client/automation script will cause it to fail. To run it successfuly again, you must manually close and restart the Server, then run the Client again once the Server restarts.

### c. In the Server directory, run the hash-secrets.py file.
```bash
cd Server # If you are not in Server directory yet.
python3 ./hash-secrets.py
```
This will create a single file called hash-repo.txt in the Exploit directory, so 
```
./Server/hash-repo.txt
```

hash-repo.txt will have the following format. 
```
HASH OF user1.txt ==> [SHA256 HASH OF USER1]

HASH OF user2.txt ==> [SHA256 HASH OF USER2]

HASH OF user3.txt ==> [SHA256 HASH OF USER3]
```
These hashes will be used to verify that we have indeed gotten the correct file when we run the exploit, and did not just hardcode it.


## 7. Running the Exploit.
Now comes the fun part. After you have done step 6, you can run the C++ exploit script like so:
```bash
cd Exploit # If not in Exploit directory yet.
./exploit 127.0.0.1 7000
```

You will see the ./Secrets/userN.txt files have been replaced with this message:

```
YOU HAVE BEEN HACKED
```

### Again, please ensure the PORT number, is the same as the server's.