@echo off
rmdir sln /S/Q
mkdir sln
cd sln
cmake -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14 2015 Win64" ..

pause