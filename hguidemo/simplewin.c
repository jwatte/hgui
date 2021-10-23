// SPDX-License-Identifier: GPL-2.0-or-later
//simplest hgui application, an empty window with window-icon

#include <hgui.h>

int main (int argc, char **argv)
{
#include "icon.xpm"
    hWindow *hw=createWindow(50,50,300,200,"Simplest HermitGUI window",0xc0c0c0,0x000000,200,100,0,icon_xpm,NULL);
    addTextCentered(hw,0,0,-1,-1,"Hello World!",0x000000,"-*-*vera sans*-bold-r-*-*-24-*-*-*-p-*-*-*");
    eventLoop(hw,NULL,NULL);
    destroyWindow(hw);
    return 0;
}
