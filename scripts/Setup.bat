@echo off

pushd ..
Walnut\vendor\bin\premake\Windows\premake5.exe --file=Build-Ray-Tracing.lua vs2022
popd
pause