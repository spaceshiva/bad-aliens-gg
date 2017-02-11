#!/bin/sh

echo Build initial_scene from assets folder
folder2c -v assets/bmp2tile assets/initial_scene
sdcc -c -mz80 assets/initial_scene.c -o assets/
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
echo Build Game
sdcc -c -mz80 game.c
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
echo Linking
sdcc -o output.ihx -mz80 --data-loc 0xC000 --no-std-crt0 crt0_sms.rel game.rel SMSlib_GG.lib assets/initial_scene.rel
# adicionar os assets na linha acima
# assets.rel
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
ihx2sms output.ihx badaliens.gg
