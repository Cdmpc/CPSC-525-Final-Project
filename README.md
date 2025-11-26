# CPSC-525-Final-Project
CPSC 525 Fall 2025 - Final Project Repository

## Compile program (Will change):

```bash
g++ -std=c++20 -Wall -o ./Execs/app ./App/log.cpp ./App/main.cpp ./App/server.cpp ./App/client.cpp
 ```
 
## Start up the SERVER on Port 5400 - (SERVER keyword is not case-sensitive):
```bash
./Execs/app 5400 SERVER
```
## Start up the CLIENT on Port 5400 - (CLIENT keyword is not case-sensitive):
```bash
./Execs/app 5400 CLIENT
```

You will be interacting with the CLIENT as the SERVER logs your requests and other information.

### Below is an example interaction from the CLIENT Terminal:
```bash
---------------------------------------------------------------
|  WELCOME TO NOTEE-FY! (Type QUIT or EXIT to disconnect...)  |
---------------------------------------------------------------
[CLIENT] ENTER COMMAND: CREATE bob secret1 *
LOG::bytes recieved from server : 8

[SERVER]: CREATED


[CLIENT] ENTER COMMAND: USERNAME bob secret1 *bob\x001234567890123456789012345678user1.txt
LOG::bytes recieved from server : 4096

[SERVER]: STORED


[CLIENT] ENTER COMMAND: LOGIN bob secret1 *
LOG::bytes recieved from server : 13

[SERVER]: I LIKE PEANUT

[CLIENT] ENTER COMMAND: exit
Shutting down client...
---------------------------------------------
|         DISCONNECTED, GOODBYE!...         |
---------------------------------------------
```

### Here is what the SERVER logs during this interaction:
```bash
LOG::port : 5400

LOG::handling client

LOG::bytes recieved : 21
LOG::message : CREATE BOB SECRET1 *
LOG::command : CREATE
LOG::username : BOB
LOG::password : SECRET1
LOG::payload :
LOG::CREATED
LOG::handle : user1.txt

LOG::bytes recieved : 67
LOG::message : USERNAME BOB SECRET1 *BOB\X001234567890123456789012345678USER1.TXT
LOG::command : USERNAME
LOG::username : BOB
LOG::password : SECRET1
LOG::payload : BOB\X001234567890123456789012345678USER1.TXT
LOG::found : BOB
LOG::password valid
LOG::new username : BOB\X001234567890123456789012345678USER1.TXT
LOG::new handle : 678USER1.TXT

LOG::bytes recieved : 20
LOG::message : LOGIN BOB SECRET1 *
LOG::command : LOGIN
LOG::username : BOB
LOG::password : SECRET1
LOG::payload :
LOG::found : BOB
LOG::password valid
LOG::contents returned
LOG::Client disconnected or recv error.
```

### NOTE: The SERVER does NOT exit when the Client types exit, it is still running...


Test messages:

CREATE bob secret1 *

CREATE bill secret2 *pay

USERNAME bob secret1 *12345678901234567890123456789012HACKED.txt

USERNAME bob secret2 *some payload

LOGOFF bob secret1 *I LIKE PEANUTS

LOGIN bob secret1 *

PASSWORD bob secret1 *newpass123

LOGIN bob newpass123 *