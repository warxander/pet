#!/bin/python3

import argparse
import os
import shutil
import subprocess


def main():
    root_dir = os.path.dirname(os.path.realpath(__file__))

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "pet_executable", nargs="?", default=os.path.join(root_dir, "../bin/pet")
    )
    args = parser.parse_args()

    pet_executable = args.pet_executable
    print(f"Using Pet executable: {pet_executable}")

    time_path = shutil.which("time")
    if time_path is None:
        print("time is not installed")
        return

    scripts_dir = os.path.join(root_dir, "scripts")
    for dirpath, _, filenames in os.walk(scripts_dir):
        for filename in filenames:
            if filename.endswith(".pet"):
                expected_result = 1 if filename.endswith("_fail.pet") else 0
                print(f"Running {filename}...")
                result = subprocess.run(
                    [time_path, "-f", "%E real\t%U user\t%S sys\t%M KB max-rss", args.pet_executable, os.path.join(dirpath, filename)]
                )
                if result.returncode != expected_result:
                    print(f"{filename} failed!")
                    return

    print("Success!")


if __name__ == "__main__":
    main()
