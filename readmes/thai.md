# foolbar (ฟูล์บาร์)

ทำมาเล่นๆ

## ไม่ซัพพอร์ต:
- ไอ่ตรงเเทบตัวเลข

## ต้องมี
*** เเพ็กเกจ (Alpine Linux)***
```
wayland
wayland-dev
wayland-protocols
```
- ตัวคอมไพล์ C
- คอมโพซิเตอร์ที่มี "zwlr-layer-shelll"
- CPU 64 bit
- "wlr-layer-shell-unstable-v1.xml" จาก wlroots/protocol
- ระบบที่มี pthread

## วิีธีคอมไพล์ ต้อง มี "wlr-layer-shell-unstable-v1.xml" ในโฟล์เด้อ:
```
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.c
wayland-scanner client-header ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.h
wayland-scanner private-code ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.c

cc xdg_shell.c wlr-layer-shell-unstable-v1.c foolbar.c -l wayland-client -Wall -Wextra -Wno-unused-parameter -std=c99
```


