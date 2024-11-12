#!/usr/bin/env python3

import argparse
import json
import os
import shutil
import shlex
import sys
import subprocess

from xml.etree.ElementTree import Element, ElementTree, SubElement


COMPILER_CFG_FILENAME = "compiler.cfg"


def get_compiler_defines(compiler):
    defines = []

    proc_result = subprocess.run(
        f"echo | {compiler} -dM -E -x c++ -",
        stdout=subprocess.PIPE,
        shell=True,
    )
    proc_result.check_returncode()

    for define_line in proc_result.stdout.decode("utf-8").splitlines():
        if define_line.startswith("#define"):
            defines.append(define_line.removeprefix("#define").strip().split(" ", 1))

    return defines


def write_compiler_defines_to_cfg_file(defines, dir):
    print(f"Writing {len(defines)} defines to {COMPILER_CFG_FILENAME}...")

    root = Element("def", {"format": "2"})

    for define in defines:
        SubElement(
            root,
            "define",
            {"name": define[0], "value": define[1] if len(define) == 2 else ""},
        )

    with open(os.path.join(dir, COMPILER_CFG_FILENAME), "w") as f:
        ElementTree(root).write(f, encoding="unicode")


def main():
    if shutil.which("cppcheck") is None:
        sys.exit("cppcheck is missing")

    parser = argparse.ArgumentParser()
    parser.add_argument("build_dir")
    parser.add_argument("-j", "--jobs")
    parser.add_argument("-v", "--verbose", action="store_true")
    args = parser.parse_args()

    compile_commands_json_path = os.path.join(args.build_dir, "compile_commands.json")
    if not os.path.isfile(compile_commands_json_path):
        sys.exit(f"${compile_commands_json_path} is missing")

    compiler = None

    with open(compile_commands_json_path) as f:
        compiler = shlex.split(json.load(f)[0]["command"])[0]

    if compiler is None:
        sys.exit("Failed to detect compiler from compile_commands.json")

    print(f"Detected compiler: {compiler}")

    cppcheck_build_dir = os.path.abspath(os.path.join(args.build_dir, "cppcheck"))
    if not os.path.exists(cppcheck_build_dir):
        os.mkdir(cppcheck_build_dir)

    compiler_defines = get_compiler_defines(compiler)
    has_compiler_defines = len(compiler_defines) != 0
    if has_compiler_defines:
        write_compiler_defines_to_cfg_file(compiler_defines, cppcheck_build_dir)

    cppcheck_command = (
        f"cppcheck -j{args.jobs or os.cpu_count()}"
        f" --cppcheck-build-dir={cppcheck_build_dir}"
        " --std=c++17"
        " --enable=warning,style,performance,portability"
        " --project=compile_commands.json"
    )

    if has_compiler_defines:
        cppcheck_command += (
            f" --library={os.path.join(cppcheck_build_dir, COMPILER_CFG_FILENAME)}"
        )

    cppcheck_command += " -v" if args.verbose else " -q"

    suppressed_errors = {"syntaxError", "preprocessorErrorDirective", "unknownMacro"}
    for error in suppressed_errors:
        cppcheck_command += f" --suppress={error}"

    ignored_paths = {
        "../thirdparty",
    }
    script_dir = os.path.dirname(__file__)
    for path in ignored_paths:
        cppcheck_command += f" -i{os.path.abspath(os.path.join(script_dir, path))}"

    print(f"Command: {cppcheck_command}")

    return subprocess.run(cppcheck_command, cwd=args.build_dir, shell=True).returncode


if __name__ == "__main__":
    sys.exit(main())
