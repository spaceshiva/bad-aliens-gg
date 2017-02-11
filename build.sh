#!/bin/sh

echo Build title_scene from assets folder
folder2c -v assets/bmp2tile assets/assets
sdcc -c -mz80 assets/assets.c -o assets/
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
echo Build Game
sdcc -c -mz80 main.c
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
echo Linking
sdcc -o output.ihx -mz80 --data-loc 0xC000 --no-std-crt0 crt0_sms.rel main.rel SMSlib_GG.lib assets/assets.rel
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
ihx2sms output.ihx badaliens.gg
