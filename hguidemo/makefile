#HermuitGUI demonstration program makefile
#(hgui should be built first)

CD=cd
CP=cp -a
LN=ln -sfn 
MD=mkdir -p 
MV=mv 
RM=rm -rf 
STRIP=strip 
CHMOD=chmod

all: hguidemo hguidemsh simplewin simplewsh screensav diagram # hguidemst

#static build using only local sources
hguidemo: hguidemo.c ../hgui.c ../hgui.h robodance.anim
	$(CC) hguidemo.c ../hgui.c -s -o hguidemo -lX11 -I"../" \
     # -DHGUI_SELECTWIDGETS -DHGUI_BUTTONS  #  -DDEBUG  #  -DUSE_XFT -lXft
     #(see top of hgui.h to get what -D parameters / directives control the selective inclusion of widgets)

#dynamic build using shared-library (.so)
hguidemsh: hguidemo.c ../libhgui.so ../hgui.h robodance.anim
	$(CC) hguidemo.c -s -I"../" -L"../" -lhgui -o hguidemsh -Wl,-rpath="../"

#dynamic build using static-library (.a)
hguidemst: hguidemo.c ../libhgui.a ../hgui.h robodance.anim
	$(CC) hguidemo.c -static -s -I"../" -L"../" -lhgui -o hguidemst -lX11 -lXau -lxcb -lpthread -ldl

simplewin: simplewin.c ../hgui.c ../hgui.h                    #selecting only needed widgets
	$(CC) simplewin.c ../hgui.c -s -o simplewin -lX11 -I"../" -DHGUI_SELECTWIDGETS -DHGUI_TEXTLINES

simplewsh: simplewin.c ../libhgui.so ../hgui.h
	$(CC) simplewin.c -s -I"../" -L"../" -lhgui -o simplewsh -Wl,-rpath="../"

screensav: screensav.c ../hgui.c ../hgui.h
	$(CC) screensav.c ../hgui.c -s -o screensav -lX11 -lm -I"../" -DHGUI_SELECTWIDGETS -DHGUI_PRIMITIVES

diagram: diagram.c ../hgui.c ../hgui.h
	$(CC) diagram.c ../hgui.c -s -o diagram -lX11 -I"../"

install:
	#$(CP) hguidemsh hguidemst /usr/bin/hgui/

clean:
	$(RM) ./hguidemo ./hguidemsh ./hguidemst ./simplewin ./simplewsh ./screensav  hguidemo.o simplewin.o screensav.o

uninstall:
	#$(RM) /usr/bin/hgui/hguidemsh /usr/bin/hgui/hguidemst

purge: clean uninstall
