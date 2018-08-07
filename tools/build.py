#!/usr/bin/python3

import argparse
import subprocess
import os, sys

# TODO: validate if 'mode' is "Debug" or "Release"
parser = argparse.ArgumentParser(description='Build script for MakeTemp')
parser.add_argument('-m', '--mode', action='store', help='mode', dest='mode')

args = parser.parse_args()

toolsDir = os.path.dirname(__file__)
projRoot = os.path.dirname(toolsDir)
buildDir = os.path.join(projRoot, "build")

if not os.path.exists(buildDir):
    os.mkdir(buildDir)

pwd = os.getcwd()
os.chdir(buildDir)

rc = subprocess.call("conan install .. -s build_type=" + args.mode, shell=True)
print("conan returned {}".format(rc))
rc = subprocess.call("cmake --build . --config " + args.mode, shell=True)
print("cmake returned {}".format(rc))

os.chdir(pwd)
