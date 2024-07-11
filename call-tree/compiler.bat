@echo off
set SOURCE_FILE=%1
set OPTIONS=%2
gcc -c "%SOURCE_FILE%" %OPTIONS%