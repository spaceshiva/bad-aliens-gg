#!/bin/sh

# echo Build assets.c and assets.h from assets folder
# folder2c assets assets
# sdcc -c -mz80 assets.c
# if %errorlevel% NEQ 0 goto :EOF
echo Build Game
sdcc -c -mz80 game.c
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
echo Linking
sdcc -o output.ihx -mz80 --data-loc 0xC000 --no-std-crt0 crt0_sms.rel game.rel SMSlib_GG.lib
# adicionar os assets na linha acima
# assets.rel
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
ihx2sms output.ihx badaliens.gg
