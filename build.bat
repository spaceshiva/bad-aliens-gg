@echo off
REM echo Build assets.c and assets.h from assets folder
folder2c -v assets\bmp2tile assets\initial_scene
sdcc -c -mz80 assets/initial_scene.c -o assets/
if %errorlevel% NEQ 0 goto :EOF
echo Build Game
sdcc -c -mz80 game.c
if %errorlevel% NEQ 0 goto :EOF
echo Linking
sdcc -o output.ihx -mz80 --data-loc 0xC000 --no-std-crt0 crt0_sms.rel game.rel SMSlib_GG.lib assets\initial_scene.rel
if %errorlevel% NEQ 0 goto :EOF
ihx2sms output.ihx badaliens.gg
