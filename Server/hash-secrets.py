import os;
import sys;
import signal;
import pathlib as plib;
import hashlib;
import random;
import string;

# CREATES A SINGLE FILE THAT CONTAINS THE SHA256 HASHES OF EACH SECRET MESSAGE.
def create_hashes():
    # Get all the file names from Secrets
    secrets_dir = "./Secrets";
    files = os.listdir(secrets_dir);

    # Create HASH repo file
    with open("hash-repo.txt", "w") as fp_repo:
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


if __name__ == "__main__": 
    create_hashes()