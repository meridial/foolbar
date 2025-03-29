# foolbar

mi kreas cxi tiu status-panelo programmaro por mia uzo
mi ne rekomendas uzi cxi tiu senpere. (kvankam eble kiel ekzemplo por "zwlr-layer-shell")

## ne subteni la sekvanto
- "compositor" integro (ekzemple: workspaces (laborspacojn) )

## bezonajn pakojn (pakajn nomojn en Alpine Linux):
```
wayland
wayland-dev
wayland-protocols
```
- Konfidebla C kompilero
- "Compositor" kun "zwlr-layer-shell" subteneco
- 64-Borpinto centra-procez-unuo
- `wlr-layer-shell-unstable-v1.xml` de wlroots/protocol
- Systemo kun `pthreads` subteneco

## Kompili (certigxi ke `wlr-layer-shell-unstable-v1.xml` estas en la nuna adreslibro)
**Bonvolu noti ke vi eble bezoni trovi adres de `xdg-shell`
```
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.h
wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg_shell.c
wayland-scanner client-header ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.h
wayland-scanner private-code ./wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1.c

cc xdg_shell.c wlr-layer-shell-unstable-v1.c foolbar.c -l wayland-client -Wall -Wextra -Wno-unused-parameter -std=c99
```

Havu amuzon!

