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

// or if You don't have make
>idf.py flash monitor


SD card: Partition 1 type Start 2048 : W95 FAT32 (b)
Attributes: 80
Filesystem UUID: xxxx-xxxx
Filesystem: vfat

Dirs on root:

Z48
Put spectrum48 files here.Форматы: .TAP,.Z80,.SNA

Z128
Put spectrum128 files here spectrum128.Форматы: .TAP,.Z80,.SNA

![image1](https://github.com/sdima1357/spectrum128_esp32_usb_host_pal_tv/blob/main/images/IMG_20210403_234328.jpg?raw=true)

1 Analog color PAL-TV 50 Hz output: (GPIO25)

2 Two  (bitband) USB-HID controllers (DP/DM) GPIO16/ GPIO17 - USB 1 GPIO18/ GPIO19 - USB 2

3 Emulation  AY-3-8910 (GPIO27)

4 SDCard control (.tap .z80 .sna) read pins:

PIN_NUM_MISO 2

PIN_NUM_MOSI 15

PIN_NUM_CLK 14

PIN_NUM_CS 13

5 Screen width F4/F5

6 Tape speed normal/fast F11/F12


Credits:

PAL init & video PAL code from here:

bitluni's lab
https://bitluni.net/imprint https://bitluni.net/esp32-color-pal


rossum
https://rossumblog.com/
https://github.com/rossumur/esp_8_bit

