# ============================================ [START OF EXPLOIT SCRIPT] ============================================ #
import os;
import sys;
import pexpect as pex;
import pexpect.popen_spawn as pop;
import signal;
import pathlib as plib;
import hashlib;
import random;
import string;

DEBUG = os.environ.get("DEBUG", "0").lower() in ["1", "y", "yes", "true", "on", "color"];

# ============================================= [HELPER FUNCTIONS] ============================================== #
def rm_all_files(dir_path):
    secrets_dir = dir_path;
    files = os.listdir(secrets_dir);

    if (len(files) > 0):
        for name in files:
            file = secrets_dir + name;
            if os.path.isfile(file):
                os.remove(file);
            else:
                continue;
    else:
        print("Directory is already empty, no files to remove...");


def create_N_notes(N, IP_ADDR, PORT):
    # Create the N users
    for i in range(1, N + 1):
        cli_process = pop.PopenSpawn(f"stdbuf -o0 ./client {IP_ADDR} {PORT}", timeout=5);
        cli_process.delaybeforesend = None;

        if DEBUG:
            cli_process.logfile = sys.stdout.buffer;
        # Create the user metadata.
        cli_process.expect_exact("Do you already have and account (Y/N) ? : ");
        cli_process.sendline("N");

        # Send in sample usernames with the same format : user_i
        cli_process.expect_exact("Please enter your username: ");
        cli_process.sendline(f"user_{i}");

        # Send length-20 passwords of psuedorandom alphanum passwords: [a-zA-Z0-9]
        cli_process.expect_exact("Please enter your password: ");
        pseudo_rand_pw = "".join(random.choices(string.ascii_letters + string.digits, k=20));
        cli_process.sendline(pseudo_rand_pw);

        # NOTE: The temp.txt file is created automatically from the C++ program, 
        # once you enter a username and password.
        cli_process.expect_exact("(1, 2, 3) ? : ");

        # NOTE: Make sure to write to the created temp.txt file BEFORE entering "1" input.
        with open("./temp.txt", "w") as fp:
            # Write a psuedorandom N-length a-zA-Z0-9 string as the secret message.
            secret_message = f"[{i}] ===> {"".join(random.choices(string.ascii_letters + string.digits, k=30))}";
            fp.write(secret_message);

        # Upload the file, which can be found at Secrets directory.
        cli_process.sendline("1");

        # Close the process, as the program will terminate once we enter and write the note and 
        # "1" input.
        cli_process.expect(pex.EOF, timeout=5);
        cli_process.kill(signal.SIGTERM);

    print(f"{N} users created, returning from function...");


# ============================================= [MAIN DEFINITION] ============================================== #
def main(IP_ADDRESS, PORT_NO):
    try:
        # Delete the temp file if it exists before running the exploit script.
        plib.Path("./temp.txt").unlink(missing_ok=True);
        
        # User creation loop.
        valid_input = False;
        while (not valid_input):
            N_users = input("How many users do you want to create (MAXIMUM 100): ").strip();
            if (N_users.isdigit() and (int(N_users) <= 100 and int(N_users) > 0)):
                N_users = int(N_users);
                valid_input = True;
                # Let's create N users
                create_N_notes(N=N_users, IP_ADDR=IP_ADDRESS, PORT=PORT_NO);
                print("create_N_notes() succeeded");
            
            # If the user did not put in anything for created users, just use the default of 50.
            elif (N_users == ""):
                print("No input detected, using default value of 50 users...");
                valid_input = True;
                create_N_notes(N=50, IP_ADDR=IP_ADDRESS, PORT=PORT_NO);
                print("create_N_notes() succeeded");
            
            else:
                print("INVALID INPUT, PLEASE ENTER A NUMBER <= 100\n");
    
        # Clear the temp file once it's done.
        plib.Path("./temp.txt").unlink(missing_ok=True);

    except Exception:
        print("create_N_notes() failed");


# ============================================= [MAIN CALL AND STARTING POINT] ============================================== #
if __name__ == "__main__":
    try:
        print("============================== [USER GEN START] ===================================\n");
        main(sys.argv[1], sys.argv[2]);
        print("\n============================== [FINISHED] ==============================");
    except Exception:
        print("\nCAUGHT ERROR: main() function failed, did you try: ");
        print("argv[1] == IP ADDRESS (127.0.0.1 for example)");
        print("argv[2] == PORT NUMBER (Ex: 5400)");
        print("REMEMBER: cd Client");
        print("Usage: python3 ./user-gen.py argv[1] argv[2]");
