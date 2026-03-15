@echo off &:: 'echo off' stops printing the commands in the file as they run

mkdir build &:: create a build folder
pushd build
cl -Zi ..\src\win32_game.cpp user32.lib Gdi32.lib &:: need to include user32.lib import library
popd
