@echo off

cmake -S . -B bootstrap
cmake --build bootstrap --config Release

rmdir /s /q bootstrap
