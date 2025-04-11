@echo off
cd src
cd resource
del resource.o
windres resource.rc -O coff -o resource.o
cd ..
cd ..
gcc src/main.c src/resource/resource.o -lgdi32 -luser32 -lcomctl32 -o catchzeblock.exe
catchzeblock.exe