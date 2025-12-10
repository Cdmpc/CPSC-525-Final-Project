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

# NOTE: This exploit script is meant to be run from the REPOSITORY DIRECTORY.
# i.e, the directory that is created when you CLONE this repository.
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
        # Open up the client as well, on the same port.
        cli_process = pop.PopenSpawn(f"stdbuf -o0 ./Client/client {IP_ADDR} {PORT}", timeout=5);
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

# CREATES A SINGLE FILE THAT CONTAINS THE SHA256 HASHES OF EACH SECRET MESSAGE.
def create_hashes():
    # Get all the file names from Secrets
    secrets_dir = "./Secrets";
    files = os.listdir(secrets_dir);

    # Create HASH repo file
    with open("./Exploit/hash-repo.txt", "w") as fp_repo:
        for file in files:
            # Robust DIR + FILE path joining.
            file_path = os.path.join(secrets_dir, file);

            # Skip non-files
            if not (os.path.isfile(file_path)):
                continue;

            # SHA256 Hasher object and buffer to read secret.
            sha256_hasher = hashlib.sha256();

            # Open the Secrets files
            with open(file_path, "rb") as fp_secret:
                # Read the Secret contents
                secret = fp_secret.read();

                # Hash the secret, and write it to the REPO file.
                sha256_hasher.update(secret);
                fp_repo.write(f"HASH OF {file} ==> {sha256_hasher.hexdigest()}\n");

    # Confirm hashes were complete.
    print(f"HASHING COMPLETE CHECK ./Exploit/hash-repo.txt file");


# ============================================= [MAIN DEFINITION] ============================================== #
def main(IP_ADDRESS, PORT_NO):
    try:
        # Delete the temp file if it exists before running the exploit script.
        # Also delete the secret text files in the Secrets directory.
        plib.Path("./temp.txt").unlink(missing_ok=True);

        # Create the "Secrets" directory, if it does not exist.
        plib.Path("./Secrets").mkdir(exist_ok=True);
        valid_input = False;

        # User creation loop.
        while (not valid_input):
            N_users = input("How many users do you want to create (MAXIMUM 100): ").strip();
            if (N_users.isdigit() and (int(N_users) <= 100 and int(N_users) > 0)):
                N_users = int(N_users);
                valid_input = True;
                # Let's create N users
                create_N_notes(N=N_users, IP_ADDR=IP_ADDRESS, PORT=PORT_NO);
                create_hashes();
                print("create_N_notes() succeeded, please check ./Secrets directory.");
            
            # If the user did not put in anything for created users, just use the default of 50.
            elif (N_users == ""):
                print("No input detected, using default value of 50 users...");
                valid_input = True;
                create_N_notes(N=50, IP_ADDR=IP_ADDRESS, PORT=PORT_NO);
                create_hashes();
                print("create_N_notes() succeeded, please check ./Secrets directory.");
            
            else:
                print("INVALID INPUT, PLEASE ENTER A NUMBER <= 100\n");
    
        # Clear the temp file once it's done.
        plib.Path("./temp.txt").unlink(missing_ok=True);

    except Exception:
        print("create_N_notes() failed");

# ============================================= [MAIN CALL AND STARTING POINT] ============================================== #
if __name__ == "__main__":
    try:
        print("============================== [EXPLOIT START] ===================================\n");
        main(sys.argv[1], sys.argv[2]);
        print("\n============================== [EXPLOIT TERMINATED] ==============================");
    except Exception:
        print("\nCAUGHT ERROR: main() function failed, did you try: ");
        print("argv[1] == IP ADDRESS (127.0.0.1 for example)");
        print("argv[2] == PORT NUMBER (Ex: 5400)");
        print("Usage: python3 ./Client/user-gen.py argv[1] argv[2]");