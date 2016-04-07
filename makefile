#=======================================================================
#HermuitGUI makefile
#===================
VERSION=1

AR=ar rcs 

CD=cd
CP=cp -a
LN=ln -sfn 
MD=mkdir -p 
MV=mv 
RM=rm -rf 
STRIP=strip 
CHMOD=chmod 


all: libhgui.so libhgui.a hguidemo/hguidemo

libhgui.so: hgui.c hgui.h
	$(CC) -c hgui.c -fpic -o hgui.so.o -lX11  # -DHGUI_SELECTWIDGETS -DHGUI_BUTTONS  #  -DDEBUG  #  -DUSE_XFT -lXft
	$(CC) hgui.so.o -shared -o libhgui.so -lX11 #-lm

libhgui.a: hgui.c hgui.h
	$(CC) -c hgui.c -o hgui.a.o -lX11 #-lm
	$(AR) libhgui.a hgui.a.o

hguidemo/hguidemo: hguidemo/hguidemo.c hgui.c hgui.h hguidemo/robodance.anim
	make -C hguidemo


install: /usr/lib/libhgui.so /usr/lib/libhgui.a /usr/include/hgui.h

/usr/lib/libhgui.so: ./libhgui.so
	$(CP) libhgui.so /usr/lib/libhgui.so.$(VERSION)
	$(STRIP) /usr/lib/libhgui.so.$(VERSION) #not a standard thing for GNU but it's a bit smaller this way
	$(CHMOD) 0755 /usr/lib/libhgui.so.$(VERSION)
	$(CD) /usr/lib/ ; $(LN) libhgui.so.$(VERSION) libhgui.so

/usr/lib/libhgui.a: ./libhgui.a
	$(CP) libhgui.a /usr/lib/

/usr/include/hgui.h: ./hgui.h
	$(CP) hgui.h /usr/include/hgui.h


clean:
	$(RM) hgui.so.o libhgui.so hgui.a.o libhgui.a hgui.o
	make -C hguidemo clean

uninstall:
	$(RM) /usr/lib/libhgui.so /usr/lib/libhgui.so.$(VERSION) /usr/lib/libhgui.a
	$(RM) /usr/include/hgui.h

purge: clean uninstall


#=======================================================================
