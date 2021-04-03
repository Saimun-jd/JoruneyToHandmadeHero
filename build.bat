@echo off
mkdir ..\..\build
pushd ..\..\build
cd
cl -Zi w:\handmade\code\win32_handmade.cpp user32.lib gdi32.lib
popd
