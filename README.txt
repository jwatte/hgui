========================================================================
                         HermitGUI 1.0 readme
========================================================================

Introduction:
-------------
 HermitGUI ( abbreviated: 'hgui' ) is a GUI-toolkit for the X11 
graphic environment (Xorg) for Linux/Unix operating systems. 
(Cross-platform (Win/OSX), framebuffer(fbterm/kmscon), SDL, SVGAlib,xcb,
alternative-X (nanoX/DirectFB/Wayland) support might come in the future 
utilizing more macros in 'hgui.c' and additional codes split by #ifdef.)

 You can easily make graphic programs with hgui in very small size.
(Dynamically linked programs depending on stripped 70k shared libhgui.so
weight minimally around 6kbyte, statically linked programs can be as 
small as 25kB if only some widgets are compiled in, but with all widgets
included their minimum size is around 60 kilobytes. 
Nice, eh? These would still fit even into a Commodore 64's memory. :)

 My main motivation to spend those 3 weeks writing this toolkit was to 
have a basic GUI in my upcoming linux distribution for system settings
and utilities, that doesn't depend on much libraries and isn't as big as
current Fox/GTK/Qt. For small daily used applications like calculator,
text-editor,etc. there should be a tiny toolkit for tiny linux distros, 
and size/scalability is an important thing to me. Though there were some
fairly small toolkits to choose from (libGx,tekUI,GraphApp,XForms,FLTK),
in the end I came to the conclusion that they're aren't as small as I
wished for (below 200kbyte), and aren't looking/working the way I like.
That's the strange story how HermitGUI was born...

 On the other hand I want a GUI toolkit that has a wrapper for BASH too.
I know, BASH shell is not the best for GUI application development, but
it has direct access already to the system commands and is already in
there, not needing extra libraries like Lua/tinypy/TCL/etc. The idea 
is something like in Puppy Linux, where 'gtkdialog'/'xdialog' is used in
conjuction with BASH script, and many Puppy-appliactions are made this 
way. Why is it better than coding apps in C? Of course the CPU-intensive
tasks still should be done in C (or even better Assembly) but for simple
frontends to the powerful commandline tools, a scripting might be the 
best way. E.g. modifying the code to the user's needs gets as simple as
editing a text file containing a BASH script...

Building & Installing HermitGUI:
--------------------------------
The building procedure is simple: you only need the basic development 
toolchain (binutils, 'gcc' toolchain and GNU 'make'), C standard-library
(stdlib.h,stdio.h) and the Xorg libraries (Xlib/libX11) in the places
'/usr/include/X11' (for headers) and '/usr/lib' (shared dynamic libs), 
and you can build HermitGUI by entering to its directory and typing:

 make

The shared library libhgui.so, and libhgui.a will be created. Also some 
demo-programs get created in 'hguidemo' folder (dynamic & static ver.).
To install HermitGUI system-wide, the usual command is to be typed:
 
 make install

(Not much is done, just libhgui.so(.1) and libhgui.a gets copied to 
 /usr/lib/ and the header file hgui.h to /usr/include/ )

Cleaning these source-folders from intermediate binaries created can be 
done with 'make clean', while removing the system files can be done
by 'make uninstall'. To remove everything created, type: 'make purge'

Using HermitGUI in your project:
--------------------------------
 As mentioned above, you have two ways to build HermitGUI applications:
Static build, which includes all the sources of the toolkit in your app,
and therefore GUI won't have dependencies other than X & libc libraries.
The other method is the Dynamic build, which uses the shared library,
libhgui.so at runtime, which is in /usr/lib (or .. rpath folder below).
For both methods you need to #include <hgui.h> header-file. Check out
hgui.h, the makefile and hguidemo.c in hguidemo folder as an example...
(Especially how -D gcc directives can be used to omit unneeded widgets.)

 The documentation for the Window / Widget related functions is 'hgui.h'
header-file itself. I hope it's self-explanatory enough for programmers.
(Probably the only thing that differs from most of the toolkits is that
negative coordinates are interpreted as coordinates relative to the
right/bottom edge of the created window. That way many widgets can be
moved/scaled dynamically according to the window-size in a simple run.)
 A short note about fonts: currently no XFT + fontconfig is supported, 
only the non-antialiased server-side Xorg 'core' font engine, so you can
use only the XLFD (and aliases in 'fonts.alias' like '6x13' and 'fixed')
as font-names in the widget adder functions. (If a font is not found on 
the system,e.g. /usr/share/fonts/fonts.dir doesn't contain the fontname,
hgui will warn and use Xorg's built-in 6x13 fixed font as fallback... 
Tip for a considerably good looking and small font: Bitstream Vera Sans)

ToDo?textedit/inputfield,list/pulldown/menu(bar),tab/panel/frame,dialog,
-----statusbar,tickboxes/radiobuttons,tooltips,filedialog,colorchooser,
     numberbox, table, scrollable window, toolbar, divider/separator
     UTF8,selection/copy-paste, mousex/y/button anytime, 
     scrollbar:make more precise, knob dragging/gripping (outside too)
     button-hold/repeat/release/toggle, widget-member convenience-func.,
     maximize/fullscreen fucks up refresh coordinates (border/mov+resiz)

Closing Words:
--------------
HermitGUI is under WTF-license:Do what the fukk you want with this code.
I hope you'll enjoy using HermitGUI and will have success with it.

In case you need support, have an idea or want to contribute, don't 
hesitate to contact me via messagebox at:
                                                http://hermit.sidrip.com

                                     Hermit (Mihaly Horvath) , July 2015

========================================================================
