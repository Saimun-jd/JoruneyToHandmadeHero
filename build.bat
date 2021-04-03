@echo off
mkdir ..\..\build
pushd ..\..\build
cd
cl -Zi W:\handmade\JoruneyToHandmadeHero\code\win32_handmade.cpp user32.lib gdi32.lib
popd
