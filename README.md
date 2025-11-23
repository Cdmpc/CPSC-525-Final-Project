# CPSC-525-Final-Project
CPSC 525 Fall 2025 - Final Project Repository

## Compile program (Will change):

```bash
 g++ -Wall -o ./Execs/app ./App/log.cpp ./App/main.cpp ./App/server.cpp
 ```
 
## Run program on example port 5400 (Passed as argv[1] to main()):
```bash
./Execs/app 5400
```

On another terminal, run this command:
```bash
telnet localhost 5400
```
This will for now act as the "psuedo-client."


If you see:

    "connected on localhost : 5400 "

you are connected to the server and can use the other terminal to communicate to the server.

To quit, Ctrl+C on the server side.

Test messages(must connect each time):

CREATE bob secret1 *pay
CREATE bill secret2 *pay
USERNAME bob secret1 *12345678901234567890123456789012HACKED.txt
USERNAME bob secret2 *some payload
LOGOFF bob secret1 *I LIKE PEANUTS
LOGIN bob secret1 *