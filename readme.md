# foolbar

![sample](/sample.png)

![readme en esperanto](/readmes/esperanto.md)
![readme ภาษาไทย] (/readmes/thai.md)

I made this bar for my usage :3  
You shouldn't use this directly. (maybe as a zwlr-layer-shell example)

### doesnt support:
- compositor integrations (workspaces, etc..)

### NOTE (for me. myself and I):
- monet scrolltext depends on batt_acc by batt_draw


## Requirements (package names on alpine):
```
wayland
wayland-dev
wayland-protocols
``` 
- A trusty C compiler
- compositor with zwlr-layer-shell support
- A 64 bit CPU
- `wlr-layer-shell-unstable-v1.xml` from wlroots/protocol (clone it or smth)
- System with pthreads support (currently i use linux. you might need to change a few things)

## Build (assuming you already put `wlr-layer-shell-unstable-v1.xml in the current directory`):
**NOTE: you might need to find path for xdg-shell yourself!**  
```
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.c
wayland-scanner client-header ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.h
wayland-scanner private-code ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.c

cc xdg_shell.c wlr-layer-shell-unstable-v1.c foolbar.c -l wayland-client -Wall -Wextra -Wno-unused-parameter
```
