# foolbar

I made this bar for my usage :3  
You shouldn't use this directly. (maybe as a zwlr-layer-shell example)


### provides extremely unoptimized shi:
- handwritten charcter bitmap `uint64_t char_map[]` based on the font used in **TempleOS**
- function for drawing said bitmap `void paint_char`
- function for drawing string of said bitmap `void paint_str`
- function for drawing rectangles `void fill_rect`
- function for filling the whole buffer with a color `void fill`

### doesnt support:
- compositor integrations (workspaces, etc..)
- font scaling


## Requirements for the default (package names on alpine):
```
wayland
wayland-dev
wayland-protocols
```
- A trusty C compiler
- compositor with zwlr-layer-shell support
- CPU as fast. or faster than 100Mhz (rawdogging a scrolling text is quiet expensive)
- `wlr-layer-shell-unstable-v1.xml` from wlroots/protocol (clone it or smth)
- System with pthreads support (currently i use linux. you might need to change a few things)

## Build (assuming you already put `wlr-layer-shell-unstable-v1.xml in the current directory`):
**NOTE: you might need to find path for xdg-shell yourself!**  
```
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.c
wayland-scanner client-header ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.h
wayland-scanner private-code ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.c

cc xdg_shell.c wlr-layer-shell-unstable-v1.c foolbar.c
```
