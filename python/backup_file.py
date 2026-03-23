import os
import sys
import time

def save_backup(body):
    # get the directory where the script is located
    script_dir = os.path.dirname(os.path.abspath(__file__))
    timestamp = time.strftime("%Y-%m-%d_%H-%M-%S")
    file_path = os.path.join(script_dir, "../res/backups/state backup " + timestamp + ".txt")
    with open(file_path, "w") as f:
        f.write(body)

body = sys.argv[1]
save_backup(body)