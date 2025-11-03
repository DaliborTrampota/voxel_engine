@echo off

if not exist build (
    mkdir build
)

if "%1" == "-vs" (
    start build/voxel_engine.sln
    exit
)

cmake -B build -S .
start build/voxel_engine.sln