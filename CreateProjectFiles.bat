@echo off

mkdir bin\Debug
mkdir bin\Release
mkdir bin\RelWithDebInfo

:: GDAL
xcopy /E /Y 3rdparty\GDAL\bin\*.dll bin\Debug
xcopy /E /Y 3rdparty\GDAL\bin\*.dll bin\Release
xcopy /E /Y 3rdparty\GDAL\bin\*.dll bin\RelWithDebInfo

xcopy bin\*.cfg bin\Debug
xcopy bin\*.cfg bin\Release
xcopy bin\*.cfg bin\RelWithDebInfo

rmdir sln /S/Q
mkdir sln
cd sln
cmake -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -Dprotobuf_BUILD_TESTS=OFF -G "Visual Studio 14 2015 Win64" ..

pause