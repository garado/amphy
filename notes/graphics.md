# Overview
- Main screen buffer (background) consists of 256x256 pixels or 32x32 tiles (8x8px each)
- Only 160x144 can be displayed on the screen
- SCROLLX and SCROLLY regs hold coords of bg to be displayed in upper left corner of the screen
- bg wraps around the screen

- Background Tile Map
    - contains number of tiles to be displayed
    - 32 rows of 32 bytes each; each bye contains # of a tile to display