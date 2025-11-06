@echo off

if not exist build (
    mkdir build
)

if "%1" == "-cc" (
    @REM cmake --preset ninja-clangd
    move /y .\build-ninja\compile_commands.json .\build\compile_commands.json
    exit
)

if "%1" == "-vs" (
    start build/voxel_engine.sln
    exit
)

cmake -B build -S .
start build/voxel_engine.sln