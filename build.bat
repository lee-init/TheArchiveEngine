@echo off &:: 'echo off' stops printing the commands in the file as they run

mkdir build &:: create a build folder
pushd build
cl ..\src\win32_game.cpp
popd
