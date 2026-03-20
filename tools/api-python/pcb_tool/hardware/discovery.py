import os
import platform
import getpass

def find_pcb_mount():
    os_type = platform.system()
    if os_type == "Windows":
        for letter in "DEFGHIJKLMNOPQRSTUVWXYZ":
            drive = f"{letter}:\\"
            if os.path.exists(os.path.join(drive, "telemetry_log.csv")):
                return drive
    elif os_type == "Linux":
        user = getpass.getuser()
        base = f"/media/{user}/"
        if os.path.exists(base):
            for folder in os.listdir(base):
                path = os.path.join(base, folder)
                if os.path.exists(os.path.join(path, "telemetry_log.csv")):
                    return path
    return None
