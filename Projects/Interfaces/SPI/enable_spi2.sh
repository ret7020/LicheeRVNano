/etc/init.d/S30wifi stop # корректная остановка wifi (чтобы потом без перезагрузки включить обратно)
./devmem 0x030010D0 b 0x1 # GPIO P18 CS
./devmem 0x030010DC b 0x1 # GPIO P21 MISO
./devmem 0x030010E0 b 0x1 # GPIO P22 MOSI
./devmem 0x030010E4 b 0x1 # GPIO P23 SCK

