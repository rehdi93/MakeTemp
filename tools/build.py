#!/usr/bin/python3

import argparse
import subprocess
import os, sys

# TODO: valid set: "Debug", "Release"
parser = argparse.ArgumentParser(description='Build script for MakeTemp')
parser.add_argument('-m', '--mode', action='store', help='mode', dest='mode')

args = parser.parse_args()

toolsDir = os.path.dirname(__file__)
projRoot = os.path.dirname(toolsDir)
buildDir = os.path.join(projRoot, "build")

print(toolsDir, projRoot, buildDir, sep=', ')

if not os.path.exists(buildDir):
    os.mkdir(buildDir)

pwd = os.getcwd()
os.chdir(buildDir)

subprocess.call("conan install .. -s build_type=" + args.mode, shell=True)
subprocess.call("cmake --build . --config " + args.mode, shell=True)

os.chdir(pwd)
