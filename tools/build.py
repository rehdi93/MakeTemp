#!/usr/bin/python3

import argparse
import subprocess; from subprocess import call
import os, sys

# TODO: validate if 'mode' is "Debug" or "Release"
parser = argparse.ArgumentParser(description='Build script for MakeTemp')
parser.add_argument('-m', '--mode', action='store', help='mode', dest='mode')

args = parser.parse_args()

toolsDir = os.path.dirname(__file__)
projRoot = os.path.dirname(toolsDir)
buildDir = os.path.join(projRoot, "build")

pwd = os.getcwd()

cmake_gen = False
if not os.path.exists(buildDir):
    os.mkdir(buildDir)
    cmake_gen = True

os.chdir(buildDir)

rc = call("conan install .. -s build_type=" + args.mode, shell=True)
print("conan returned {}".format(rc))

if cmake_gen:
    gen = '-G "{}"'
    if sys.platform == "linux":
        gen = gen.format("Unix Makefiles")
    elif sys.platform == "win32":
        gen = gen.format("Visual Studio 15 2017")
    else:
        gen = ''

    rc = call("cmake .. " + gen, shell=True)
    print("cmake returned {}".format(rc))


rc = call("cmake --build . --config " + args.mode, shell=True)
print("cmake returned {}".format(rc))

os.chdir(pwd)
