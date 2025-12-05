# CURRENT STAGE OF EXPLOIT (INCOMPLETE)

Authored by: Shawn Hanlon & Carlos Morera Pinilla

Some of this script is adapted from A3 code by Carlos Morera Pinilla and Pavol Federl.

At this stage, the exploit just automates the creation of up to 100 users based on the user's input.

Please run this from the directory created, when you cloned this repository.

## HOW TO RUN THE EXPLOIT:

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

### 3. Run the Python Exploit Script
```bash
python3 ./Exploit/heap-exploit.py ./Server/server ./Client/client
```

If you do not provide command-line arguements to the Python script, this prompt will appear.
```bash
argv[1] == <path_to_server_executable>
argv[2] == <path_to_client_executable>
Usage: python3 ./Exploit/heap-exploit.py argv[1] argv[2]
Example: python3 ./Exploit/heap-exploit.py ./Server/server ./Client/client
```
### 4. Follow the prompt:
```
============================== [EXPLOIT START] ===================================

How many users do you want to create (MAXIMUM 100): 45
45 users created, returning from function...
create_N_notes() succeeded, please check ./Secrets directory.

============================== [EXPLOIT TERMINATED] ==============================
```
NOTE: DO NOT leave the "users to create" prompt empty, as this will cause the prgoram to fail on it's next run.

### 5. The secret files will be located in a created directory called ./Secrets, where your current working directory is the repository's directory.