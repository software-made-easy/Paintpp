# Paint++

- A simple bitmap painting program built with Qt. 
- Can be compiled to an executable on platform of choice (tested on Wine, Windows 7 and Linux Mint 20.3 Una, base: Ubuntu 20.04 focal)
- For professional photo editing, I recommend [Inkscape](https://inkscape.org)

## Features: 

- Save and load images. 
- Stack-based undo-redo which can store up to 100 actions.
- Change ~~background and~~ foreground colors
- Fill image with a background color
- Resize image
- Pen tool with 3 different caps
- Line tool with 5 styles, 3 caps, and a poly mode (when active, drag to continuously draw connected lines)
- Rectangle tool with 5 styles, 3 shapes, and ability to fill or draw border only
- Eraser tool
- Can adjust thickness for all tools

![alt-text](https://i.imgur.com/IzC44vr.png "Paint")

# Build Instructions:

- Open the Paint++.pro file using Qt Creator and build using the default settings. Qt will take care of the rest!

    Version used: 5.12.8 and 5.15.2

# To do:
- [ ] Use QGraphicsScene do draw lines, etc
- [ ] Add more tools
- [ ] Optimasations for android
- [ ] Fix change background color
- [ ] Print/Print preview support
- [ ] Translate complete App, not just the File menu
- [ ] Add change language option
- [ ] ...

## Notes:
- On Android you need to clone [this](https://gitlab.com/mattbas/Qt-Color-Widgets) git repo using
  ``git clone https://gitlab.com/mattbas/Qt-Color-Widgets.git``
- This APP will not receive any further updates
