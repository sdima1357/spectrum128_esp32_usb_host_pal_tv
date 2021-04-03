# spectrum128_esp32_usb_host_pal_tv
zx spectrum 128 emulator on single esp32

Single esp32 lolin32mini  spectrum 128 emulator with direct gpio 2 HID LS usb-hosts and PAL analog video output

board ~$3 :https://www.aliexpress.com/premium/LOLIN32.html or any of https://www.aliexpress.com/premium/ESP32.html

usb connectors(for example): https://www.aliexpress.com/item/1005002027124387.html

//set right esp32 env for me it:

>export IDF_PATH=$HOME/esp/esp-idf

>source $HOME/esp/esp-idf/export.sh


//connect the board, build & flash

>git clone  https://github.com/sdima1357/spectrum128_esp32_usb_host_pal_tv

>cd spectrum128_esp32_usb_host_pal_tv

>make flash monitor

SD card: Partition 1 type Start 2048 : W95 FAT32 (b)
Attributes: 80
Filesystem UUID: xxxx-xxxx
Filesystem: vfat

Dirs on root:

Z48
Put spectrum48 files here.Форматы: .TAP,.Z80,.SNA

Z128
Put spectrum128 files here spectrum128.Форматы: .TAP,.Z80,.SNA

