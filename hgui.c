// Hermit simple GUI toolkit (with C and Bash support), mainly for Herminux
//=========================================================================

//------------------------ included libraries --------------------------
#include <stdlib.h>     //C standard library
#include <stdio.h>      //printf,sprintf,fprintf,etc.
#include <stdarg.h>
#include <string.h>     //operations like strlen()
//#include <math.h>
//#include <time.h>
#include <errno.h>
#include <X11/Xlib.h>   //libX11
#include <X11/Xos.h>    //minimize system-dependencies
#include <X11/Xutil.h>  //XWMHints,XChangeProperty,etc.
#include <X11/keysym.h> //key symbols
#include <X11/Xatom.h>  //X11 predefined atoms & properties (e.g. size-hints)
#include <X11/Xlocale.h> //Internationalization and Unicode support (better doing it with freetype2 library)
//#ifdef USE_XFT
// #include <X11/Xft/Xft.h>
// XftDraw *xftdraw; XftColor xftcolor; XRenderColor xrcolor; XftFont *xftfont;
//#endif

#include "hgui.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//window definition aliases/dependencies
#define NOICONIFY NOTASKBAR
#define NOMAXIMIZE NORESIZE //nomaximize is an alias to noresize, means: no maximize-button, which is straightforward in case of non-resizable window

//handle internal widget dependencies
#ifdef HGUI_BUTTONS
#define HGUI_TEXTLINES
#endif
#ifdef HGUI_PICBUTTONS
#define HGUI_BUTTONS
#define HGUI_PICTURES
#endif
#ifdef HGUI_TEXTBLOCKS
#define HGUI_TEXTLINES
#endif
#ifdef HGUI_TEXTBOXES
#define HGUI_TEXTLINES
#define HGUI_TEXTBLOCKS
#define HGUI_BLOCKS
#define HGUI_SCROLLBARS
#endif
#ifdef HGUI_SCROLLBARS
#define HGUI_BLOCKS
#endif
#ifdef HGUI_PICANIMS
#define HGUI_PICTURES
#endif
#ifdef HGUI_TEXTANIMS
#define HGUI_TEXTLINES
#endif
#if defined(HGUI_SLIDERS) || defined(HGUI_PROGBARS)
#define HGUI_TEXTLINES //for caption
#endif
//#if defined(HGUI_FILEBROWSER) || defined(HGUI_COLORCHOOSER)
// #define HGUI_CHILDWINDOWS
// #define HGUI_CHILDWIN_COMPOSITES
//#endif

//handle internal widget-groups
#if defined (HGUI_TEXTBUTTONS) || defined(HGUI_PICBUTTONS)
#define HGUI_BUTTONS
#endif
#if defined(HGUI_TEXTLINES) || defined(HGUI_TEXTBLOCKS) || defined(HGUI_TEXTBOXES) || defined(HGUI_TEXTANIMS)
#define HGUI_TEXTFIELDS
#endif
#if defined(HGUI_TEXTANIMS) || defined(HGUI_PICANIMS)
#define HGUI_ANIMATIONS
#endif


//----------------internal program constants and settings---------------
//some EWMH (Extended Window Manager Hints) values
#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_TOGGLE 2
#define SOURCE_INDICATION_NO_OLD 0
#define SOURCE_INDICATION_NORMAL 1
#define SOURCE_INDICATION_PAGERS 2
//#define DEBUG True //debug mode
#define EXIT_OK    0
#define EXIT_ERROR 1
#define EXIT_OUTOFMEM 2
#define RET_ERR -1
#define LINESIZE_MAX 1024 //maximum width of a text-line (in conversion)
#define TEXTSIZE_MAX 1000000 //maximum amount (bytes) of text to display in a textbox/textblock
#define XPM_DEPTH 32  //not the colour-depth, but the representation in memory
#define REFRESHRATE 100 //Hz the maximum frequency to refresh window-contents in case of destructive resizing / moving / covering (Expose-event)
#define REFRESHDELAY 1000000/REFRESHRATE
#define CHILDWIN_AMOUNT_MAX 64 //64
#define WIDGET_AMOUNT_MAX 1024
//#define POLYGON_MAX_POINTS 256 (using 'char' type prevents problems aready)
#define ANIM_AMOUNT_MAX WIDGET_AMOUNT_MAX
#define SCROLLTEXTWIDTH_MAX 1024 //max.length of a scrolltext-bar in characters
#define PROPSIZE_MAX 64 //Window-Manager property text max. size
//#define XBUILTINFONT "-*-fixed-medium-r-*-*-13-*-*-*-c-*-*-*" //"fixed" //"6x13" //probably built into every Xorg-servers, if no font described, this comes into play
#define XBUILTINFONT_WIDTH  6  //if the OS has a different font as default, these settings should be changed or loading XBUILTINFONT will be necessary at window-creation
#define XBUILTINFONT_HEIGHT 13
#define XBUILTINFONT_ASCENT 10
#define XBUILTINFONT_DESCENT XBUILTINFONT_HEIGHT-XBUILTINFONT_ASCENT
#define TABSIZE 4
#define BORDERSIZE 2
#define TRANSPCOLOR 0x808080 //fallback when no background-colour is given for transparency-supported XPM
#define BUTTFADE 0.1

//default theme (used for simple/automatic routines not having all inputs):
#define CONTRAST 0.4 //0.0 to 1.0
#define BGCOLOR 0xCEC8CA
#define FGCOLOR 0x001000
//#define DEFAULTFONT XBUILTINFONT //e.g. "-*-arial-medium-r-*-*-17-*-*-*-p-*-*-*"
#define HOLECOLOR 0x001100
#define BUTTBGCOL 0x10D428
#define BUTTFGCOL 0x202020
#define BUTTBEVEL 2
#define BUTTBORDER 1
#define BUTTBORDCOL HOLECOLOR
#define TEXTBGCOL 0xE0F0E0
#define TEXTFGCOL 0x203010
#define PROGBGCOL 0x80E0C0
#define PROGFGCOL 0x1020E0
#define PROGBEVEL BUTTBEVEL
#define PROGTEXTCOL 0xFFFFFF
#define SLIDERHOLE 2
#define SLIDSCALEW 20 //slide-scalemark-width should be automatic depending on horizontal knob-size
//#define SLIDEMARG 10 //should be automatic depending on vertical knob-size (which is derived from horizontal knobsize)
#define SLIDKNOBSIZE 16
#define SLIDBGCOL 0xC0E0D0
#define SLIDFGCOL 0x20C080
#define SLIDKNOBCOL 0xC020E0
#define SCROLLBGCOL 0xC0A080
#define SCROLLFGCOL 0x624050
#define SCROLLKNOBCOL 0xD8D0D4
#define SCROLLBUTTSIZE 14
#define ANIMDELAY 4  //in frames
#define SCROLLSPEEDY 1 //4 character-rows to jump on scrolloffsetheel in textbox
#define SCROLLSPEEDX 8 //pixel-rows to jump with horizontal scrolling

enum WidgetTypes { BUTTON,BUTTONDYN,BUTTONXPM,VSLIDER,HSLIDER,TEXTLINE,TEXTCENTER,TEXTBLOCK,TEXTBOX,HPROGBAR,VPROGBAR,VSCROLL,HSCROLL,BLOCK,BLOCKFILL,PICTURE,XPM,ANIM,ANIMXPM,TEXTSCROLL,
                   POINT,LINE,RECT,RECTFILL,ARC,ARCFILL,CIRCLE,CIRCLEFILL,ELLIPSE,ELLIPSEFILL,POLYGON,FILLSOLID
                 };


//---------------- macros (for readability and going towards cross-platform/framebuf(fbterm,kmscon)/SDL/alternative-X(nanoX,DirectFB) support) --------------
#ifdef __unix__ //X11 Xorg    (note: space is not allowed between macro-function-name and left curly brace)
#define SETBGCOL(col) ( XSetBackground(hw->d,hw->gc,col) )
#define SETDRAWCOL(col) ( XSetForeground(hw->d,hw->gc,col) )
#define DRAWPOINT(x,y,col) ( SETDRAWCOL(col), XDrawPoint(hw->d,hw->w,hw->gc,x,y) )
#define DRAWLINE(x1,y1,x2,y2) ( XDrawLine(hw->d,hw->w,hw->gc,x1,y1,x2,y2) )
#define DRAWRECT(x1,y1,x2,y2) ( XDrawRectangle(hw->d,hw->w,hw->gc,x1,y1,x2,y2) )
#define DRAWRECTFILL(x1,y1,x2,y2) ( XFillRectangle(hw->d,hw->w,hw->gc,x1,y1,x2,y2) )
#define DRAWARC(x,y,wi,h,startdeg,extentdeg) ( XDrawArc(hw->d,hw->w,hw->gc,x,y,wi,h,startdeg*64,extentdeg*64) )
#define DRAWARCFILL(x,y,wi,h,startdeg,extentdeg) ( XFillArc(hw->d,hw->w,hw->gc,x,y,wi,h,startdeg*64,extentdeg*64) )
#define DRAWSTRINGN(x,y,text,col,font,len) ( XDrawString(hw->d,hw->w,setFontGC(hw,hw->bgcol,col,font), x, y, text, len) )
#define DRAWSTRING(x,y,text,col,font) ( XDrawString(hw->d,hw->w,setFontGC(hw,hw->bgcol,col,font), x, y, text, strlen(text)) )
#endif
//#ifdef __WIN32__
// #define SETDRAWCOL(col) ( ?! )
// #define DRAWPOINT(x,y,color) \ ( ? )
// #define DRAWLINE(x1,y1,x2,y2) \ ( ? )
// ....
//#endif


//------------------------- type and struct definitions ----------------
typedef struct Widget { //this is the common struct for widgets,

//1st public part: structure should match exactly to the 1st part of specific structs in hgui.h (like Button,VScroll,etc.))
    union {
        int x,x1;
    } ;
    union {
        int y,y1;
    } ;
    union {
        int x2w,x2,w,r,rx;
    } ;
    union {
        int y2h,y2,h,ry;
    } ;
    union {
        int zh,hpos,border,knobsize,depth,startdeg,scrollpixcnt;
    } ;
    union {
        long zv,vpos,bevel,scalewidth,degree,scrollindex;
    } ;
    union {
        unsigned long bgcol,color,transpcol;
    } ;
    union {
        unsigned long fgcol,textcol;
    } ;
    union {
        unsigned long knobcol,progtextcol,vpixsize,scrolldisplen;
    } ;
    union {
        unsigned long captcol,vpixpos,scrolloffset;
    } ;
    union {
        void *data,*text,*content,*title,*caption,*pixdata,*xpm,*xpmlist;
    } ;
    union {
        float pos;
    } ;
    union {
        float ratio,scale;
    } ;
    union {
        char opt1,wrap,framedelay;
    } ;
    union {
        char opt2,scrollbars,spdcnt;
    } ;
    union {
        char opt3,pixstep,autofit;
    } ;   //char modeswitches; an alternative parametering method for one-bit mode-switches like wrap/scrollbars/autofit/edit/readonly/etc.
    union {
        char state,buttonstate,framecount,scrollendstate;
    } ;
    int actx1,acty1,actx2,acty2; //read-only (writing could fuck up the GUI)

//2nd private part: this must be hidden from the outside wordld, the following members are for internal use only. if needed, write function to get/set them
    union {
        void *fsp, *font, *fontstruct;
    } ; //XFontStruct pointer
    void (*callback)(); //callback-routine pointer
    char grabbed;
    int margin;
    union {
        int knobx1,knoby1;
    } ;
    union {
        int hpixsize,knobx2,knoby2;
    }; //active-area is calculated by widget-drawing routines in drawWidget()
    char type;
    int id;
    void* parenthw;
    void* argptr; //'id' is set by addWidget()
    union {
        void *child1,*vscroll;
    } ;
    union {
        void *parentbox,*child2,*hscroll;
    } ; //link parent (e.g. textbox) with children (e.g.scrollbars)

} Widget;


//#undef hWindow
typedef struct hWindow {
//1st more public part: structure should match the one in hgui.h if exists
    unsigned long bgcol;
    float contrast;
    int eventx,eventy;
    char eventbutton;
    int width, height;
//2nd private part for internal use
    int orig_width,orig_height;
//int norm_x,norm_y, norm_width,norm_height; //fullscreen status and position+dimensions before going to fullscreen state (restoring window-size/position should be handled by window-manager)
    char options;
    char *title;
    void (*exitCallback)();
    void *parenthw; //hWindow*
    void* Child[CHILDWIN_AMOUNT_MAX];
    int ChildAmount;
    Widget W[WIDGET_AMOUNT_MAX];
    int WidgetAmount; //'W':widget-array, WidgetAmount counts added items, used for Widget-ID
    int AnimID[ANIM_AMOUNT_MAX];
    int AnimAmount; //so no need to look up all widget-array to check if animations are added
    Display *d;
    int fd; /*Drawable*/ Window w;
    int s;
    GC gc, defgc;
    Atom wm_protocols,delete_window;
    char dragging;
    int exposecnt, resizecnt, exitsig;
    int id;
} hWindow;


typedef struct TextDim {
    int x,y;
    long w;
    short h, ascent,descent;
} TextDim;
typedef struct TextBoxDim {
    long w, h, rowcount, vpixpos;
} TextBoxDim;


//----------------- global (preferably constant) content ---------------
const char ErrPfx[]="ERROR(hgui): ", WarnPfx[]="WARNING(hgui): ";
const char ERRORTXT_OUTOFMEM[]="Out of memory!", EmptyString[]="";
Widget eWidget;
void* EmptyWidget=&eWidget;
XFontStruct eFontStruct;
XFontStruct* EmptyFontStruct=&eFontStruct;
XFontStruct *xfont=NULL; //built-in Xorg font
short DownArrowPoints[]= {-4,5, 5,5, 0,10}, UpArrowPoints[]= {-5,9, 5,9, 0,3};
short RightArrowPoints[]= {5,-5, 5,5, 10,0}, LeftArrowPoints[]= {9,-5, 9,5, 4,0};


//--------------------- internal function prototypes -------------------
//(public functions are prototyped in the header-file, local functions are in dependency-order)
int dynCoordX(hWindow *hw, int x);
int dynCoordY(hWindow *hw, int y);
TextDim drawTextScrollFrame(hWindow *hw, Widget *wp, int x1, int y1, int x2, int y2, char *text);
void cleanUpRect(hWindow *hw, int x1o, int y1o, int x2o, int y2o,  int x1, int y1, int x2, int y2);
void refreshWidget(Widget *w);

//------------------------- basic GUI routines -------------------------
XPMstruct XPMtoPixels(char* source[], unsigned long bgcol) //only suitable for XPMs with around max. 50 colours (single-character colour-lookups)
{
    int i,j,k,sourceindex,pixdataindex,width,height,colours,charpix,transpchar=0xff;
    unsigned int colr[256],colg[256],colb[256];
    XPMstruct xpms;
    unsigned char colchar[256], colcode,colindex, transpr=(bgcol&0xFF0000)/65536,transpg=(bgcol&0xFF00)/256,transpb=bgcol&0xFF;
    sscanf(source[0],"%d %d %d %d",&width,&height,&colours,&charpix); //printf("width:%d, height:%d, colours:%d, charpix:%d",width,height,colours,charpix);
    unsigned char *pixdata = malloc (width*height*XPM_DEPTH/8); //32 is sure to have enough room
    for (i=0; i<colours; i++) {
        if (sscanf(source[1+i],"%c%*s #%2X%2X%2X",&colchar[i],&colr[i],&colg[i],&colb[i])!=4) transpchar=colchar[i];
    }
    for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
            sourceindex=(i*width+j);
            colcode=source[colours+1+i][j];
            for(k=0; k<colours; k++) if (colcode==colchar[k]) break;
            colindex=k;
            pixdataindex=(i*width+j)*XPM_DEPTH/8;
            pixdata[pixdataindex+0] = (colcode!=transpchar)?colb[colindex]:transpb; //Blue 0x502040
            pixdata[pixdataindex+1] = (colcode!=transpchar)?colg[colindex]:transpg; //Green
            pixdata[pixdataindex+2] = (colcode!=transpchar)?colr[colindex]:transpr; //Red
            pixdata[pixdataindex+3] = (colcode==transpchar)?0x00:0xff ;//Aplha - 0:fully transparent...0xFF:fully opaque /Xlib has no alpha-channel support, Xrender has transparency-support
        }
    }
    xpms.pixdata=pixdata;
    xpms.width=width;
    xpms.height=height;
    return xpms;
}

Display* openDisplay()
{
    Display *d=XOpenDisplay(NULL);
    if(d==NULL) {
        fprintf(stderr,"%s Can't open display.\n",ErrPfx);
        exit(EXIT_ERROR);
    }
    return d;
}

//tip: 'xprop' is an useful utility to check the window's properties
Atom changeWMprop(hWindow *hw, char* property, char* data, Bool overwrite) //window-manager hint properties _NET_WM_WINDOW_TYPE / _NET_WM_STATE / MOTIF_WM_HINTS / etc. which are not settable directly by Xlib convenience functions
{
    Atom wmatom = XInternAtom(hw->d,data,False);
    XChangeProperty(hw->d,hw->w, XInternAtom(hw->d,property,False), XA_ATOM, 32,overwrite?PropModeReplace:PropModeAppend, (unsigned char*) &wmatom, 1);
    return wmatom;
}

void setWindowType(hWindow *hw, char *type)
{
    //called when creating window, preferably not called afterwards
    char prop[PROPSIZE_MAX];
    sprintf(prop,"_NET_WM_WINDOW_TYPE_%s",type); //printf("Setting window as type %s\n",type);
    changeWMprop(hw,"_NET_WM_WINDOW_TYPE",prop,True);
}

Atom addWindowState(hWindow *hw, char *type)
{
    char prop[PROPSIZE_MAX];
    sprintf(prop,"_NET_WM_STATE_%s",type); //printf("Setting window to state %s\n",type);
    return changeWMprop(hw,"_NET_WM_STATE",prop,False);
}

void sendStateClientMessage(hWindow *hw, char *type, char action)
{
    //informs the window-manager about the state-change
    char prop[PROPSIZE_MAX];
    sprintf(prop,"_NET_WM_STATE_%s",type);
    Atom msgtypeatom=XInternAtom(hw->d,"_NET_WM_STATE",False), stateatom=XInternAtom(hw->d,prop,False);
    XEvent stev;
    stev.xclient.type=ClientMessage;
    stev.xclient.serial=0;
    stev.xclient.send_event=True;
    stev.xclient.message_type=msgtypeatom;
    stev.xclient.window=hw->w;
    stev.xclient.format=32;
    stev.xclient.data.l[0]=action;
    stev.xclient.data.l[1]=stateatom;
    stev.xclient.data.l[2]=0;
    stev.xclient.data.l[3]=SOURCE_INDICATION_NORMAL;
    stev.xclient.data.l[4]=0;
    XSendEvent(hw->d, RootWindow(hw->d,hw->s), True, StructureNotifyMask | SubstructureNotifyMask | SubstructureRedirectMask , &stev);
    XSync(hw->d,True);
}

void setWindowStates(hWindow *hw) //all-at-once, starting from empty to avoid accumulation/duplication
{
    //based on current value of hw->options   //XWMHints *hints = XAllocWMHints(); hints->flags=StateHint; hints->initial_state=IconicState; XSetWMHints(hw->d,hw->w,hints);
    Atom msgtypeatom=XInternAtom(hw->d,"_NET_WM_STATE",False);
    XDeleteProperty(hw->d,hw->w,msgtypeatom);
    if (hw->options&NOTASKBAR) addWindowState(hw,"SKIP_TASKBAR");
    if (hw->options&ALWAYSONTOP) addWindowState(hw,"ABOVE");
    if (hw->options&MODALWIN) addWindowState(hw,"MODAL");
    if (hw->options&FULLSCREEN) addWindowState(hw,"FULLSCREEN");
//sendStateClientMessage should be called after these changes to inform window-manager with XSendEvent()
}

void setWinTypeToOptions(hWindow *hw, short options)
{
    //at the moment I don't know about distinct properties (other than nonstandard _MOTIF_WM_HINTS) for titlebar-button-selection / window-decoration-disabling, until that this function is handy
    if      (options==NOMAXIMIZE) setWindowType(hw,"UTILITY");
    else if ((options&NODECOR && options&ALWAYSONTOP) || (options&TRAYAPP)) setWindowType(hw,"DOCK"); //DOCKAPP needs proper initialization afterwards (not to seen in taskbar, but be seen in system-tray)
    else if (options&NOTASKBAR) {
        if (options&NODECOR) setWindowType(hw,"NOTIFICATION");
        else if (options&NOICONIFY) setWindowType(hw,"DIALOG");   //NOICONIFY is only relevant if there's NOTASKBAR, vice-versa, and is more important (not to have minimize-icon) than NOMAXIMIZE, ALWAYSONTOP can be set later
        else if (options&NOMAXIMIZE) {
            if (options&ALWAYSONTOP) setWindowType(hw,"TOOLBAR");
            else setWindowType(hw,"MENU");
        }
    } else if (options&NODECOR) setWindowType(hw,"SPLASH");
    else setWindowType(hw,"NORMAL"); //printf("%d\n",options);
}

hWindow* createAndSetWindow(hWindow *hw, hWindow *parenthw, Window *rootw, int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol, int minwidth, int minheight, short options, char* xpmicon[], void* exitCallback, int argc, char **argv )
{
    if(hw==NULL) return NULL;
    Pixmap icon=None;
    hw->exitsig=0;
    hw->orig_width=w;
    hw->orig_height=h;
    hw->bgcol=bgcol; //hw->norm_x=x; hw->norm_y=y; hw->norm_width=w; hw->norm_height=h;
    if (options&NORESIZE) {
        minwidth=w;
        minheight=h;
    };
    hw->exitCallback=exitCallback;
    hw->w=XCreateSimpleWindow(hw->d,*rootw,x,y,w,h,(options&(NODECOR||NOWINMAN))?0:BORDERSIZE,BlackPixel(hw->d,hw->s),bgcol);
    hw->options=options;
    hw->gc=XCreateGC(hw->d,hw->w,0,NULL);
    hw->defgc=XCreateGC(hw->d,hw->w,0,NULL); //DefaultGC(hw->d,hw->s); //3rd parameter for XCreateGC could be (GCFont|GCForeground|GCBackground), 4th is &XGCValues
    XSizeHints *sizehints=XAllocSizeHints();
    sizehints->flags=PPosition|PMinSize;
    sizehints->min_width=minwidth;
    sizehints->min_height=minheight;
    if (options&NORESIZE) {
        sizehints->flags|=PMaxSize;
        sizehints->max_width=minwidth;
        sizehints->max_height=minheight;
    }
    if (xpmicon!=NULL) {
        XPMstruct xpms=XPMtoPixels(xpmicon,TRANSPCOLOR);
        XImage *iconimg = XCreateImage(hw->d,CopyFromParent,DefaultDepth(hw->d,hw->s),ZPixmap,0,xpms.pixdata,xpms.width,xpms.height,XPM_DEPTH,0);
        XInitImage(iconimg); //Pixmap icon=XCreatePixmapFromBitmapData(hw->d,hw->w,icon_pixels,ICON_WIDTH,ICON_HEIGHT,0x001122,0xFFEECC,XPM_DEPTH);
        icon=XCreatePixmap(hw->d,hw->w,xpms.width,xpms.height,DefaultDepth(hw->d,hw->s));
        XPutImage(hw->d,icon,hw->gc,iconimg,0,0,0,0,xpms.width,xpms.height);
        XDestroyImage(iconimg);
    }
    XSetStandardProperties(hw->d,hw->w,title,title,icon,argv,argc,sizehints);  //XSetWMProperties(hw->d,hw->w,NULL,NULL,NULL,0,sizehints,NULL,NULL); //XSetWMSizeHints(hw->d,hw->w,sizehints,sizeatom); //setIconXPM(hw->d,hw->w,hw->s,icon_xpm); //setTitle(hw->d,hw->w,title);  //XListFonts()
//XSetWindowBackgroundPixmap(hw->d,hw->w,None); //avoid flicker with trick, by setting empty pixmap background and repainting the window in eventloop at expose-events
    XSetWindowAttributes xwa;
    xwa.bit_gravity=StaticGravity;
    xwa.override_redirect=(options&NOWINMAN)?True:False; //override-redirect causes window to be a 'splash-creen' with no titlebar (but black border?) on top of all windows, also _NET_WM_WINDOW_TYPE property can (& should) be used to set splash
    XChangeWindowAttributes(hw->d,hw->w,CWBitGravity|CWOverrideRedirect,&xwa); //bit_gravity=StaticGravity to avoid flickering made by repainting background by X automatically when resizing window
    if(options) setWinTypeToOptions(hw,options); //this is not needed if I find a way to tell WM which particular buttons to include in titlebar or not to display window-decoration at all
    if(options) setWindowStates(hw); //initialize window-state based on 'options'
    //#ifdef USE_XFT xftdraw=XftDrawCreate(hw->d,hw->w,DefaultVisual(hw->d,hw->s),DefaultColorMap(hw->d,hw->s));
    XMapWindow(hw->d,hw->w);
    XClearWindow(hw->d,hw->w);
    XFlush(hw->d);
    XSelectInput(hw->d,hw->w,ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask|Button1MotionMask|Button3MotionMask); //XCopyGC(hw->d,XDefaultGC(hw->d,hw->s),hw->gc,GCFont);
    xfont=NULL;//=loadFont(XBUILTINFONT);  //Font *font=XLoadFont(hw->d,"6x13"); XFontStruct *fs=XLoadQueryFont(hw->d,"6x13");  //XGCValues gcval; XGetGCValues(hw->d,DefaultGC(hw->d,hw->s),GCFont,&gcval);
    SETDRAWCOL(FGCOLOR); //XSetFont(hw->d,hw->gc,defont->fid); //mkfontscale,mkfontdir->/usr/share/fonts.dir   //Font fontx=gcval.font; xfont=XQueryFont(hw->d,fontx);
    hw->wm_protocols=XInternAtom(hw->d,"WM_PROTOCOLS",False);
    hw->delete_window=XInternAtom(hw->d,"WM_DELETE_WINDOW",False);
    XSetWMProtocols(hw->d,hw->w,&hw->delete_window,1); //Set WM protocols (for proper close-button hangling throug ClientMessage event-type)
    if(parenthw!=NULL) {
        hw->parenthw=parenthw;
        parenthw->Child[parenthw->ChildAmount]=hw;
        hw->id=parenthw->ChildAmount;
        parenthw->ChildAmount++;
    } else hw->parenthw=NULL;
    hw->title=getTitle(hw);
    hw->contrast=CONTRAST;
    hw->width=getWinWidth(hw);
    hw->height=getWinHeight(hw);
    hw->WidgetAmount=0;
    hw->AnimAmount=0;
    return hw;
}

hWindow* createWindow (int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol, int minwidth, int minheight, short options, char* xpmicon[], void (*exitCallback)() )
{
//setlocale(LC_CTYPE,"en_EN.UTF-8"); if(XSupportsLocale()!=True){printf("\n %s No Locale Support\n",WarnPfx);} else XSetLocaleModifiers("");
    hWindow *hw=malloc(sizeof(hWindow));
    if(hw==NULL) {
        printf("%s %s",ErrPfx,ERRORTXT_OUTOFMEM);
        exit(EXIT_OUTOFMEM);
    }
    hw->d=openDisplay();
    hw->s=DefaultScreen(hw->d);
    hw->fd=ConnectionNumber(hw->d);
    return createAndSetWindow(hw,NULL,&RootWindow(hw->d,hw->s),x,y,w,h,title,bgcol,fgcol,minwidth,minheight,options,xpmicon,exitCallback,0,NULL);
}

#ifdef HGUI_CHILDWINDOWS
hWindow* createChildWindow (hWindow *parenthw, int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol, int minwidth, int minheight, short options, char* xpmicon[], void (*exitCallback)() )
{
    if(parenthw->ChildAmount>=CHILDWIN_AMOUNT_MAX) {
        printf("%s Reached maximum child-window amount (%d). Child-window \"%s\" was not created!\n",ErrPfx,CHILDWIN_AMOUNT_MAX,title);
        return NULL;
    }
//x+=getWinX(parenthw);y+=getWinY(parenthw); relative location to parent-window (might be an issue if parent-window is not in place / minimized -> better leave it for Window-Manager
    hWindow *hw=malloc(sizeof(hWindow));
    if(hw==NULL) {
        printf("%s %s",ErrPfx,ERRORTXT_OUTOFMEM);
        return NULL;
    }
    hw->d=parenthw->d;
    hw->s=parenthw->s;
    createAndSetWindow(hw,parenthw,&RootWindow(hw->d,hw->s),x,y,w,h,title,bgcol,fgcol,minwidth,minheight,options,xpmicon,exitCallback,0,NULL);
    return hw;
}

hWindow* createSubWindow(hWindow *parenthw, int x1, int y1, int x2, int y2, char *title, unsigned long bgcol, unsigned long fgcol)
{
    if(parenthw->ChildAmount>=CHILDWIN_AMOUNT_MAX) {
        printf("%s Reached maximum child-window amount (%d). Subwindow \"%s\" was not created!\n",ErrPfx,CHILDWIN_AMOUNT_MAX,title);
        return NULL;
    }
    x1=dynCoordX(parenthw,x1);
    y1=dynCoordY(parenthw,y1);
    x2=dynCoordX(parenthw,x2);
    y2=dynCoordY(parenthw,y2); //pos might be relative to right/bottom window-edges (negative coordinate-values)
    hWindow *hw=malloc(sizeof(hWindow));
    if(hw==NULL) {
        printf("%s",ERRORTXT_OUTOFMEM);
        return NULL;
    }
    hw->d=parenthw->d;
    hw->s=parenthw->s; //hw->options=0;
    createAndSetWindow(hw,parenthw,&parenthw->w,x1,y1,x2-x1,y2-y1,title,bgcol,fgcol,0,0,0b0000,NULL,NULL,0,NULL);
    return hw;
}
#endif //HGUI_CHILDWINDOWS

void moveWindow (hWindow *hw, int x, int y)
{
    if(hw==NULL) return;
    XMoveWindow(hw->d,hw->w,x,y);
}
void resizeWindow (hWindow *hw, int w, int h)
{
    if(hw==NULL) return;
    XResizeWindow(hw->d,hw->w,w,h);
}
void iconifyWindow(hWindow *hw)
{
    if(hw==NULL) return;
    XIconifyWindow(hw->d,hw->w,hw->s);
}
void hideWindow(hWindow *hw)
{
    if(hw==NULL) return;    //_NET_WM_STATE_HIDDEN or XWithdrawWindow(hw->d,hw->w,hw->s) does nearly the same (no window and no icon is seen)
    XUnmapWindow(hw->d, hw->w);
}
void showWindow(hWindow *hw)
{
    if(hw==NULL) return;
    XMapWindow(hw->d, hw->w);
}

void  toggleFullscreen(hWindow *hw)
{
    if(hw==NULL) return;
    hw->options^=FULLSCREEN;
    //normally restoring original position and size after fullscreen should be handled by window-manager
    //if(hw->options&FULLSCREEN) { hw->norm_width=getWinWidth(hw); hw->norm_height=getWinHeight(hw); hw->norm_x=getWinX(hw); hw->norm_y=getWinY(hw); setWindowStates(hw); }
    //else { setWindowStates(hw); resizeWindow(hw,hw->norm_width,hw->norm_height); moveWindow(hw,hw->norm_x,hw->norm_y); }
    //XUnpapMapWindow-XMapWindow trickery is not good, use XSendEvent to send _NET_WM_STATE ClientMessage to rootwindow instead:
    setWindowStates(hw);
    sendStateClientMessage(hw,"FULLSCREEN",hw->options&FULLSCREEN?_NET_WM_STATE_ADD:_NET_WM_STATE_REMOVE);
}   //XMapWindow(hw->d,hw->w); XSync(hw->d,True); }

void hideMouseCursor(hWindow *hw) //{ XUndefineCursor(hw->d,hw->w); } //XDefineCursor(hw->d,hw->w,None); }
{
    XColor black;
    black.red=black.green=black.blue=0;
    static char emptydat[]= {0,0,0,0,0,0,0,0};
    Pixmap emptypix=XCreateBitmapFromData(hw->d,hw->w,emptydat,8,8);
    Cursor blankcur=XCreatePixmapCursor(hw->d,emptypix,emptypix,&black,&black,0,0);
    XDefineCursor(hw->d,hw->w,blankcur);
    XFreeCursor(hw->d,blankcur);
    XFreePixmap(hw->d,emptypix);
}

//void setMouseCursor(hWindow *hw) { XColor fc, bc; Cursor cur=XCreatePixmapCursor(hw->d,icon,icon,&fc,&bc,1,1); XDefineCursor(hw->d,hw->w,cur);}

hWindow* destroyWindow(hWindow *hw)
{
    if(hw==NULL) return NULL;
    hWindow *parenthw=hw->parenthw;
//#ifdef USE_XFT XftDrawDestroy(xftdraw);
    if(parenthw==NULL)XFreeGC(hw->d,hw->gc);
    XDestroyWindow(hw->d,hw->w);
    if(parenthw==NULL)XCloseDisplay(hw->d);
    else {
        parenthw->Child[hw->id]=NULL;
    }
    free(hw);
    return NULL;
}

void setClass(hWindow *hw, char *class, char *name)
{
    if(hw==NULL) return;
    XClassHint *ch=XAllocClassHint();
    ch->res_name=name;
    ch->res_class=class;
    XSetClassHint(hw->d,hw->w,ch);
};

void setTitle(hWindow *hw, char *title)
{
    if(hw==NULL) return;    //XSetWMName(hw->d,hw->w,XTextProperty *text)
    XStoreName(hw->d,hw->w,title);
    XSetIconName(hw->d,hw->w,title);
    hw->title=title;
}

void setIconXPM(hWindow *hw, char *xpm[])
{
    if(hw==NULL) return;
    XPMstruct xpms=XPMtoPixels(xpm,TRANSPCOLOR);
    XImage *iconimg = XCreateImage(hw->d,CopyFromParent,DefaultDepth(hw->d,hw->s),ZPixmap,0,xpms.pixdata,xpms.width,xpms.height,XPM_DEPTH,0); //bitmap_pad: how much bits a colour is stored on
    XInitImage(iconimg); //pic->byte_order = LSBFirst; pic->bitmap_bit_order = LSBFirst; //The bitmap_bit_order doesn't matter with ZPixmap images.
    Pixmap icon=XCreatePixmap(hw->d,hw->w,xpms.width,xpms.height,DefaultDepth(hw->d,hw->s));
    XPutImage(hw->d,icon,hw->gc,iconimg,0,0,0,0,xpms.width,xpms.height); //Pixmap icon=XCreatePixmapFromBitmapData(hw->d,hw->w,icon_pixels,ICON_WIDTH,ICON_HEIGHT,0xffffff,0,XPM_DEPTH);
    XWMHints *hints = XAllocWMHints();
    hints->flags=IconPixmapHint;
    hints->icon_pixmap=icon;
    XSetWMHints(hw->d,hw->w,hints);
    XDestroyImage(iconimg);
    XUnmapWindow(hw->d,hw->w);
    XMapWindow(hw->d,hw->w);
    XFree(hints);
    XSync(hw->d,False); //XFlush(hw->d); //XUnmapWindow-XMapWindow - a dirty hack for JWM to refresh icon (XSync seems not enough. What about XSendEvent ClientMessage _NET_WM_ICON?)
}

//to get display-size (monitor-resolution), DisplayWidth(hw->d,hw->s) and DisplayHeight(hw->d,hw->s) can be used, or XGetGeometry()
hWindow* getWin(void *widgetptr)
{
    Widget* wp=widgetptr;
    return wp->parenthw;
}
int getScreenWidth()
{
    Display *d=openDisplay();
    return XDisplayWidth(d,DefaultScreen(d));
}
int getScreenHeight()
{
    Display *d=openDisplay();
    return XDisplayHeight(d,DefaultScreen(d));
}
int getWinWidth(hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    XWindowAttributes xwa;
    XGetWindowAttributes(hw->d,hw->w,&xwa);
    return xwa.width;
}
int getWinHeight(hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    XWindowAttributes xwa;
    XGetWindowAttributes(hw->d,hw->w,&xwa);
    return xwa.height;
}
int getWinX(hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    XWindowAttributes xwa;
    int x,y;
    Window child;
    XTranslateCoordinates(hw->d,hw->w,RootWindow(hw->d,hw->s),0,0,&x,&y,&child);
    XGetWindowAttributes(hw->d,hw->w,&xwa);
    return x-xwa.x;
}
int getWinY(hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    XWindowAttributes xwa;
    int x,y;
    Window child;
    XTranslateCoordinates(hw->d,hw->w,RootWindow(hw->d,hw->s),0,0,&x,&y,&child);
    XGetWindowAttributes(hw->d,hw->w,&xwa);
    return y-xwa.y;
}
int getWidgetCount(hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    return hw->WidgetAmount;
}

char* getTitle (hWindow *hw)
{
    if (hw==NULL) return (char*)EmptyString;
    char* charptr;
    XFetchName(hw->d,hw->w,&charptr);
    return charptr;
}

float getContrast (hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    return hw->contrast;
}
void setContrast(hWindow *hw, float contrast)
{
    if(hw==NULL) return;
    correctRange(&contrast);
    hw->contrast=contrast;
    refreshWindow(hw);
}

void  setWinCallback  (hWindow *hw, void (*callback)() )
{
    if(hw==NULL) return;
    hw->exitCallback=callback;
}

char eventButton (hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    return hw->eventbutton;
}
int eventX (hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    return hw->eventx;
}
int eventY (hWindow *hw)
{
    if(hw==NULL) return RET_ERR;
    return hw->eventy;
}

unsigned long fadeCol(unsigned int col, float bri) //positive bri values cause brightening, negative values cause darkening
{
    int r,g,b;
    if (bri==0.0) return col;
    if (bri<-1.0) bri=-1.0;
    if(bri>1.0) bri=1.0; //saturation logic for input
    else {
        r=(int)(((col&0xFF0000)/65536)*(1.0+bri));
        g=(int)(((col&0xFF00)/256)*(1.0+bri));
        b=(int)((col&0xFF)*(1.0+bri));
    }
    if (r>255) r=255;
    if(g>255) g=255;
    if(b>255) b=255;
    if(r<0) r=0;
    if(g<0) g=0;
    if(b<0) b=0; //saturation logic for output
    return r*65536+g*256+b;
}

int roundfl (float num)
{
    if (num>=0 && num-(int)num>=0.5) return ((int)num+1);
    else if (num<0 && num-(int)num<-0.5) return ((int)num)-1;
    else return (int)num;
}

int dynCoordX(hWindow *hw, int x) //turn negative X coordinate into dynamic coordinate (counted from right-edge of the window)
{
    if(hw->options&ZOOMABLE) x=roundfl(x*((float)hw->width/(float)hw->orig_width));
    if(x<0) return hw->width+x;
    else return x;
}
//these 2 functions also provide zooming if set for the window.
int dynCoordY(hWindow *hw, int y) //turn negative Y coordinate into dynamic coordinate (counted from bottom-edge of the window)
{
    if(hw->options&ZOOMABLE) y=roundfl(y*((float)hw->height/(float)hw->orig_height));
    if(y<0) return hw->height+y;
    else return y;
}
//this sets the region where the widget is sensitive for mouse-click/scroll events
int setActiveRegion(hWindow *hw, int id, int x1, int y1, int x2, int y2, char margin, int knobxy1, int knobxy2)
{
    hw->W[id].actx1=x1;
    hw->W[id].acty1=y1;
    hw->W[id].actx2=x2;
    hw->W[id].acty2=y2;
    hw->W[id].margin=margin;
    hw->W[id].knobx1=knobxy1;
    hw->W[id].knobx2=knobxy2;
}

#if defined(HGUI_TEXTFIELDS) || defined(HGUI_TEXTBUTTONS)
XFontStruct* loadFont(hWindow *hw, char *fontname)
{
//#ifdef USE_XFT  return XftFontOpenName(hw->d,hw->s,"Arial-17");
    if (fontname==NULL) return NULL;
    XFontStruct* fs = XLoadQueryFont(hw->d,fontname);
    if(fs==NULL) printf("%s Font %s not found! Using Xorg built-in 6x13 fixed font in place of it.\n",WarnPfx,fontname); //XLoadQueryFont returns NULL ('xfont' fallback) of font not found
    return fs;
}

XFontStruct* getFontStruct(hWindow *hw, char *fontname)
{
    if(hw==NULL) return EmptyFontStruct;    //should prevent redundand loading of fonts; also: only creating FontStruct for a new size of the same type
    return loadFont(hw,fontname);
}

GC setFontGC (hWindow *hw, unsigned long bgcol, unsigned long fgcol, XFontStruct *font)
{
    if (font==NULL) {
        XSetBackground (hw->d,hw->defgc,bgcol);
        XSetForeground(hw->d,hw->defgc,fgcol);
        return hw->defgc;
    } else {
        XSetBackground (hw->d,hw->gc,bgcol);
        SETDRAWCOL(fgcol);
        XSetFont(hw->d,hw->gc,font->fid);
    }
    return hw->gc;
}

int getTextWidth(char *text, XFontStruct *font,int charcount)
{
    if (font!=NULL) return XTextWidth(font,text,charcount);
    else return XBUILTINFONT_WIDTH*charcount;
}

TextDim getTextDimN (char *text, XFontStruct *font, int len)
{
    TextDim dim;
    int dir,ascent,descent;
    XCharStruct overall; //XChar2b widetxt[];   // font->max_bounds.ascent; font->max_bounds.width ?
//#ifdef USE_XFT XGlyphInfo extents; XftTextExtents8(hw->d,font,(XftChar8*)text,len,&extents); -> extents.width; extents.height;
    if (font!=NULL) {
        XTextExtents(font,text,len,&dir,&ascent,&descent,&overall);    //printf("\n%d,%d,%s",overall.width,height,text);
        dim.h=ascent+descent;
        dim.w=overall.width;
        dim.ascent=ascent;
        dim.descent=descent;
    } else {
        dim.w=XBUILTINFONT_WIDTH*len;
        dim.h=XBUILTINFONT_HEIGHT;
        dim.ascent=XBUILTINFONT_ASCENT;
        dim.descent=XBUILTINFONT_DESCENT;
    }
    return dim;
}

TextDim getTextDim (char *text, XFontStruct *font)
{
    return getTextDimN(text,font,strlen(text));
}
#endif //textfields/buttons

#ifdef HGUI_TEXTBLOCKS
long findSpaceBack(char *text, long pos)
{
    for(; pos>0; pos--) {
        if(text[pos]==' '||text[pos]=='\t') break;
    }
    return pos;
}
//long findNonSpaceBack(char *text, long pos) { for(;pos>0;pos--) {if(text[pos]!=' '&&text[pos]!='\t') break;} return pos; }
#endif

void setClipRegion(hWindow *hw, int x1, int y1, int x2, int y2)
{
    XRectangle rectangle[1];
    rectangle[0].x=x1, rectangle[0].y=y1, rectangle[0].width=x2-x1, rectangle[0].height=y2-y1;
    XSetClipRectangles(hw->d,hw->gc,0,0,rectangle,1,Unsorted);
    XSetClipRectangles(hw->d,hw->defgc,0,0,rectangle,1,Unsorted);
}

void removeClipRegion(hWindow *hw)
{
    XGCValues gcval; //XGetGCValues(hw->d,gcontext,GCClipXOrigin|GCClipYOrigin|GCClipMask,&gcval);
    gcval.clip_x_origin=0;
    gcval.clip_y_origin=0;
    gcval.clip_mask=None;
    XChangeGC(hw->d,hw->gc,GCClipXOrigin|GCClipYOrigin|GCClipMask,&gcval);
    XChangeGC(hw->d,hw->defgc,GCClipXOrigin|GCClipYOrigin|GCClipMask,&gcval);
}


//-------------------------- Graphic Primitives ------------------------
#ifdef HGUI_PRIMITIVES
void drawPoint(hWindow *hw, int x, int y, unsigned long col)
{
    if(hw==NULL) return;
    DRAWPOINT(x,y,col);
}

void drawLine(hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWLINE(x1,y1,x2,y2);
}


void drawArc(hWindow *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWARC(x1,y1,x2-x1,y2-y1,startdeg,degree);
}

void drawArcFill(hWindow *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWARCFILL(x1,y1,x2-x1,y2-y1,startdeg,degree);
}

void drawRect(hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWRECT(x1,y1,x2-x1,y2-y1);
}

void drawRectFill(hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWRECTFILL(x1,y1,x2-x1,y2-y1);
}

void drawCircle(hWindow *hw, int x, int y, int r, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWARC(x-r,y-r,r*2,r*2,0,360);
}
void drawCircleFill(hWindow *hw, int x, int y, int r, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWARCFILL(x-r,y-r,r*2,r*2,0,360);
}

void drawEllipse(hWindow *hw, int x, int y, int rx, int ry, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWARC(x-rx,y-ry,rx*2,ry*2,0,360);
}
void drawEllipseFill(hWindow *hw, int x, int y, int rx, int ry, unsigned long col)
{
    if(hw==NULL) return;
    SETDRAWCOL(col);
    DRAWARCFILL(x-rx,y-ry,rx*2,ry*2,0,360);
}

void drawPolygon(hWindow *hw, unsigned long col, char count, ...) //variadic function with ellipsis (...) as last parameter, taking 'count' number of parameters
{
    if(hw==NULL) return;
    va_list arg;
    int i,x,y;
    va_start (arg, count);
    XPoint *points=malloc(count*sizeof(XPoint));
    for (i=0; i<count; i++) {
        points[i].x=va_arg(arg,int);
        points[i].y=va_arg(arg,int);
    } //points[i].x=dynCoordX(hw,points[i].x); points[i].y=dynCoordX(hw,points[i].y); }
    va_end (arg);
    XFillPolygon(hw->d,hw->w,hw->gc,points,count,Complex,CoordModeOrigin);
    free(points);
}
#endif //HGUI_PRIMITIVES


//-------------------------- Basic widget drawing routines -----------------------------
#ifdef HGUI_PICTURES
XImage* putPicture(hWindow *hw, int x, int y, char* pixeldata, int width, int height, int depth, int id)
{
    //no scaling or colour conversion, the image-data must be prepared; transparency could be handled for images that have alpha channel
    XImage *pic = XCreateImage(hw->d,CopyFromParent,DefaultDepth(hw->d,hw->s),ZPixmap,0,pixeldata,width,height,(depth>=24)?32:16,0); //bitmap_pad: how much bits a colour is stored on, 32 for 24/32bit, 16 for 15/16bit
    XInitImage(pic); //pic->byte_order = LSBFirst; //pic->bitmap_bit_order = LSBFirst; //The bitmap_bit_order doesn't matter with ZPixmap images.
    XPutImage(hw->d,hw->w,hw->gc,pic,0,0,x,y,width,height);
    return pic; //XDestroyImage(pic);
}

//XImage* putScaledPicture(int x1, int y1, int x2, int y2, char* pixeldata, int orig_width, int orig_height, int orig_depth)
//{} //conversion between image-depth and visual-depth should happen here (also scaling-support can be done here)

XPMstruct putXPM(hWindow *hw, int x, int y, char* xpm[], unsigned long bgcol, int id)
{
    XPMstruct xpms=XPMtoPixels(xpm,bgcol);
    XImage *img=putPicture(hw,x,y,xpms.pixdata,xpms.width,xpms.height,XPM_DEPTH,id);
    XDestroyImage(img);
    return xpms; //depth is always 32bit for XPM
}
#endif //HGUI_PICTURES

#ifdef HGUI_TEXTLINES
TextDim drawCenText(hWindow *hw, int x, int y, int w, int h, char *text, unsigned long fgcol, XFontStruct *font) //draw text centered into x,y,w,h rectangle
{
    //get dimensions of string to pos then shift text by needed amount to be aligned to center of bounding box given as parameters
    if(text==NULL) return;
    TextDim dim=getTextDim(text,font);
    dim.x=x+(w-dim.w)/2;
    dim.y=y+(h-dim.ascent)/2;
    setClipRegion(hw,x,y,x+w,y+h);
    DRAWSTRING(dim.x, dim.y+dim.ascent, text, fgcol, font);
    removeClipRegion(hw);
//XCreateFontset(w->d,"fixed",
//XwcDrawString(hw->d,hw->w,font,setFontGC(hw,fgcol,font), x+(w-dim.w)/2,  y+(h+dim.ascent)/2, text, strlen(text));
    return dim;
} //used for onerow-buttons //DRAWRECT(x+(w-overall.width)/2,y+(h-height)/2,overall.width,ascent-descent);

void drawCaption(hWindow *hw, int x, int y, int w, char *caption, unsigned long captcol, XFontStruct *font, int id)
{
    //y will be bottom of caption
    if(caption==NULL || font==NULL) return;
    TextDim dim=getTextDim(caption,font);
    XDrawImageString(hw->d,hw->w,setFontGC(hw,hw->bgcol,captcol,font), x+(w-dim.w)/2, y-dim.descent-2, caption, strlen(caption));
}

void clearCaptionTrails(hWindow *hw, int x, int y, int w, char *caption, XFontStruct *font, int id, Bool FullClear)
{
    int x1,y1,x1o,y1o,wo;
    if(caption==NULL || font==NULL) return;
    TextDim dim=getTextDim(caption,font);
    x1=x+(w-dim.w)/2;
    y1=y-dim.h-2;
    wo=hw->W[id].actx2-hw->W[id].actx1;
    x1o = hw->W[id].actx1+(wo-dim.w)/2;
    y1o=hw->W[id].acty1-dim.h-2;
    if(!FullClear) cleanUpRect(hw, x1o, y1o, x1o+dim.w, y1o+dim.h,  x1, y1, x1+dim.w, y1+dim.h);
    else XClearArea(hw->d,hw->w,x1,y1,dim.w,dim.h,False);    //SETDRAWCOL(0x000000); DRAWRECT(x1o,y1o,dim.w,dim.h); SETDRAWCOL(0xff0000); DRAWRECT(x1,y1,dim.w,dim.h);
}

void drawTextCent(hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long fgcol, XFontStruct *font, int id)
{
    TextDim dim=drawCenText(hw,x1,y1,x2-x1,y2-y1,text,fgcol,font);
    setActiveRegion(hw,id,dim.x,dim.y,dim.x+dim.w,dim.y+dim.h,0,0,0);
}

TextDim drawTextLine(hWindow *hw, int x, int y, int h, char *text, unsigned long fgcol, XFontStruct *font) //if doesn't fit into h, not drawing
{
    //XTextItem *xp,xtext; xp=&xtext; xtext.font=defont->fid; xtext.chars="apadanyad\naniad"; xtext.nchars=strlen(xtext.chars); XDrawText(hw->d,hw->w,hw->gc,x1+4,y1+16,xp,1);
//#ifdef USE_XFT XftColorAllocValue(hw->d,DefaultVisual(hw->d,hw->s),DefaultColormap(hw->d,hw->s),&xrcolor,&xftcolor); XftDrawString8(xftdraw,&xftcolor,xftfont,x,y,(XftChar8*)text,strlen(text));
    TextDim dim=getTextDim(text,font);  //XDrawText (allows more strings with more fonts & encodings),
    if (h>0) {
        DRAWSTRING(x,y+dim.ascent,text,fgcol,font);    //if (dim.h<=h &&
    }
    return dim; //info to know where to place next row,etc.  ,XDrawImageString, XmbDrawText, XwcDrawText (wide-char), XmbDrawString (multibyte), XTextProperty, Xutf8DrawString (utf-8)
}
#endif //HGUI_TEXTLINES

#ifdef HGUI_TEXTBLOCKS
TextBoxDim drawTextBlock(hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long fgcol, char wrap, int hpos, long vpos, XFontStruct *font, char countallrows, char checkfullwidth)
{
    if (wrap) {
        checkfullwidth=0;
        hpos=0;
    }
    long ptr,i,pixrow;
    char textstart,textend;
    int j,txtwidth,linestart,xoffset,croppos,fullrowsize=0,w,h;
    static char linebuf[LINESIZE_MAX];
    TextDim linedim;
    TextBoxDim dim;
    w=x2-x1;
    h=y2-y1;
    ptr=0;
    i=xoffset=linestart=0;
    dim.rowcount=0;
    dim.w=0;
    dim.h=0;
    pixrow=0;
    croppos=LINESIZE_MAX;
    textstart=0;
    textend=0;
    linebuf[0]='\0';
    setClipRegion(hw,x1,y1,x2,y2);
    while(ptr<TEXTSIZE_MAX) {            //tip: clipmask for GC: XRectangle rectangle[1]; XSetClipRectangles(hw->d,hw->gc,x,y,rectangle,1,Unsorted);
        if (textstart==0) {
            if (dim.rowcount>=vpos) textstart=1;    //check when reached vertical pos inside the text to display
            dim.vpixpos=dim.h;
        }
        if (textend==0 && pixrow>=h) {
            if(countallrows) textend=1;    //check vertical overflow
            else break;
        }
        txtwidth=getTextWidth(linebuf,font,i);
        if(txtwidth<hpos) {
            linestart=i;
            xoffset=txtwidth-hpos;
        }
        if (i>0 && linebuf[wrap?i-1:i]!=' ' && croppos==LINESIZE_MAX && txtwidth>=w+hpos) {
            if(i<2)break;    //check horizontal overflow, handle wrapping or tell 'croppos'
            else croppos=wrap?i-1:i;
        }
        if (text[ptr]=='\0' || text[ptr]=='\n' || text[ptr]=='\r' || (wrap && croppos!=LINESIZE_MAX) ) { //check end of text and newlines
            if(!wrap && checkfullwidth) {
                fullrowsize=getTextWidth(linebuf,font,i);    //check for longest row
                if (dim.w < fullrowsize) dim.w=fullrowsize;
            }
            if (croppos==LINESIZE_MAX) linebuf[i]='\0';
            else {
                linebuf[croppos]='\0';
                if(wrap) if (i=findSpaceBack(linebuf,croppos)) {
                        linebuf[i]='\0';
                        ptr=findSpaceBack(text,ptr-2)+1;
                    }  else ptr-=1;
            }
            linedim=drawTextLine(hw,x1+xoffset,y1+pixrow,(textstart&&!textend)?h-pixrow:0,linebuf+linestart,fgcol,font);
            dim.h+=linedim.h;
            dim.rowcount++;
            if(textstart) pixrow+=linedim.h;
            if(!checkfullwidth) {
                if (dim.w < linedim.w) dim.w = linedim.w;    //check for longest visible row
            }
            if (text[ptr]=='\0') break;
            else if (text[ptr]=='\n' && text[ptr+1]=='\r' || text[ptr]=='\r' && text[ptr+1]=='\n') ptr++; //check CR+LF combined newlines
            if (!wrap || croppos==LINESIZE_MAX) ptr++;
            i=xoffset=linestart=0;
            croppos=LINESIZE_MAX;
        } else if (text[ptr]=='\t') { //check TAB whitespace
            for(j=0; j<TABSIZE; j++,i++) linebuf[i]=' ';
            ptr++;
        } else {
            linebuf[i]=text[ptr];
            i++;
            ptr++;
        }
    }
    if (font!=NULL) removeClipRegion(hw);
    return dim; //should return full height of text in pixels (including rows not seen) to inform scrollbar about ratio
}
#endif //HGUI_TEXTBLOCKS

#ifdef HGUI_BUTTONS
void drawButton(hWindow *hw, int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol, char state, char bevel, char border, XFontStruct *font, int id)
{
    int i,shinecol,shadowcol,bodycol,textcol,bordercol1=fadeCol(hw->bgcol,-hw->contrast*1.6),bordercol2=bordercol1, statey=(state&&bevel>1)?1:0;
    if (state) {
        shinecol=fadeCol(bgcol,-hw->contrast*0.01);
        shadowcol=fadeCol(bgcol,-hw->contrast-BUTTFADE);
        bodycol=fadeCol(bgcol,-BUTTFADE);
        textcol=fadeCol(fgcol,-BUTTFADE);
    } else {
        shinecol=fadeCol(bgcol,hw->contrast);
        shadowcol=fadeCol(bgcol,-hw->contrast);
        bodycol=bgcol;
        textcol=fgcol;
        bordercol2=fadeCol(hw->bgcol,-hw->contrast*2);
    }
    SETDRAWCOL(bodycol);
    DRAWRECTFILL(x+bevel,y+bevel+statey,w-bevel*2+1,h-bevel*2+1);
    SETDRAWCOL(shinecol);
    for(i=0; i<bevel; i++) {
        DRAWLINE(x+i,y+i+statey,x+w-i,y+i+statey);
        DRAWLINE(x+i,y+i+statey,x+i,y+h-i);
    }
    SETDRAWCOL(shadowcol);
    for(i=0; i<bevel; i++) {
        DRAWLINE(x+i,y+h-i+statey,x+w-i,y+h-i+statey);
        DRAWLINE(x+w-i,y+i+statey,x+w-i,y+h-i);
    }
    if(state) {
        SETDRAWCOL(fadeCol(bgcol,-hw->contrast*0.5));
        DRAWLINE(x,y,x+w,y);
        DRAWLINE(x,y,x,y+h);
    }
    for(i=0; i<border; i++) {
        SETDRAWCOL(bordercol2);
        DRAWRECTFILL(x-border+1,y+h+1,w+border*2-1,border);
        DRAWRECTFILL(x+w+1,y-border+1,border,h+border*2-1);
        SETDRAWCOL(bordercol1);
        DRAWRECTFILL(x-border+1,y-border,w+border*2-1,border);
        DRAWRECTFILL(x-border,y-border+1,border,h+border*2-1);
    }
    if(title!=NULL) drawCenText(hw,x,y+statey,w,h,title,textcol,font);
    setActiveRegion(hw,id,x,y,x+w,y+h,0,0,0);
}

#ifdef HGUI_PICBUTTONS
void drawButtonXPM(hWindow *hw, int x, int y, int w, int h, char *xpm[], unsigned long bgcol, char state, char bevel, char border, int id)
{
    drawButton(hw,x,y,w,h,NULL,bgcol,0,state,bevel,border,NULL,id);
    XPMstruct xpms=XPMtoPixels(xpm,fadeCol(bgcol,state?-BUTTFADE:0));
    int picx=x+(w-xpms.width)/2, picy=y+(h-xpms.height)/2+((state&&bevel>1)?1:0); //printf("\n%d,%d,%d,%d,%d\n",y,h,xpms.height,picx,picy);
    setClipRegion(hw,x,y,x+w,y+h);
    XImage *img=putPicture(hw,picx,picy,xpms.pixdata,xpms.width,xpms.height,XPM_DEPTH,id);
    removeClipRegion(hw);
    XDestroyImage(img); //depth is always 32bit for XPM
}
#endif //HGUI_PICBUTTONS
#endif //HGUI_BUTTONS

#ifdef HGUI_SLIDERS
void drawVSlider(hWindow *hw, int x, int y, int w, int y2, char knobsize, char scalewidth, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, unsigned long captcol, float pos, float scale , XFontStruct *font, int id)
{
    float i;
    int h=y2-y, sy, middle=x+w/2, knobsizey=knobsize*0.65, vmargin=knobsizey/2+4, range=h-vmargin*2;
    int knobx=middle-knobsize/2, knobmiddley=y+vmargin+(1-pos)*range, knoby=knobmiddley-knobsizey/2, sx=middle-scalewidth/2;
    if (knobsize>w) XClearArea(hw->d,hw->w,knobx,y,knobsize,y2-y,False);
    if(bgcol!=hw->bgcol) {
        SETDRAWCOL(bgcol);
        DRAWRECTFILL(x,y,w,h);
    }
    if(h>2*vmargin) {
        if (scale>0) {
            SETDRAWCOL(fgcol);
            for(i=0; i<1.0; i+=scale) {
                sy=y+range+vmargin-range*i-1;
                DRAWLINE(sx,sy,sx+scalewidth,sy);
            }
        }
        SETDRAWCOL(fadeCol(bgcol,-hw->contrast*1.8));
        DRAWRECTFILL(middle-SLIDERHOLE/2,y+vmargin,SLIDERHOLE,range);
    }
    SETDRAWCOL(knobcol);
    DRAWRECTFILL(knobx,knoby,knobsize,knobsizey);
    SETDRAWCOL(fadeCol(knobcol,hw->contrast));
    DRAWRECTFILL(knobx+2,knobmiddley-1,knobsize-4,2);
    SETDRAWCOL(fadeCol(knobcol,hw->contrast));
    DRAWLINE(knobx,knoby-1,knobx+knobsize-1,knoby-1);
    DRAWLINE(knobx-1,knoby,knobx-1,knoby+knobsizey-1);
    SETDRAWCOL(fadeCol(knobcol,-hw->contrast));
    DRAWLINE(knobx,knoby+knobsizey,knobx+knobsize-1,knoby+knobsizey);
    DRAWLINE(knobx+knobsize,knoby,knobx+knobsize,knoby+knobsizey-1);
    setActiveRegion(hw,id, knobsize>w?knobx:x, y, knobsize>w?knobx+knobsize:x+w, y2, vmargin,knoby,knoby+knobsize);
    drawCaption(hw,x,y,w,caption,captcol,font,id);
}

void drawHSlider(hWindow *hw, int x, int y, int x2, int h, char knobsize, char scalewidth, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, unsigned long captcol, float pos, float scale , XFontStruct *font, int id)
{
    float i;
    int w=x2-x, sx, middle=y+h/2, knobsizex=knobsize*0.65, hmargin=knobsizex/2+4, range=w-hmargin*2;
    int knoby=middle-knobsize/2, knobmiddlex=x+hmargin+(1-pos)*range, knobx=knobmiddlex-knobsizex/2, sy=middle-scalewidth/2;
    if (knobsize>h) XClearArea(hw->d,hw->w,x,knoby,x2-x,knobsize,False);
    if(bgcol!=hw->bgcol) {
        SETDRAWCOL(bgcol);
        DRAWRECTFILL(x,y,w,h);
    }
    if(w>2*hmargin) {
        if (scale>0) {
            SETDRAWCOL(fgcol);
            for(i=0; i<1.0; i+=scale) {
                sx=x+range+hmargin-range*i-1;
                DRAWLINE(sx,sy,sx,sy+scalewidth);
            }
        }
        SETDRAWCOL(fadeCol(bgcol,-hw->contrast*1.8));
        DRAWRECTFILL(x+hmargin,middle-SLIDERHOLE/2,range,SLIDERHOLE);
    }
    SETDRAWCOL(knobcol);
    DRAWRECTFILL(knobx,knoby,knobsizex,knobsize);
    SETDRAWCOL(fadeCol(knobcol,hw->contrast));
    DRAWRECTFILL(knobmiddlex-1,knoby+2,2,knobsize-4);
    SETDRAWCOL(fadeCol(knobcol,hw->contrast));
    DRAWLINE(knobx-1,knoby,knobx-1,knoby+knobsize-1);
    DRAWLINE(knobx,knoby-1,knobx+knobsizex-1,knoby-1);
    SETDRAWCOL(fadeCol(knobcol,-hw->contrast));
    DRAWLINE(knobx+knobsizex,knoby,knobx+knobsizex,knoby+knobsize-1);
    DRAWLINE(knobx,knoby+knobsize,knobx+knobsizex-1,knoby+knobsize);
    setActiveRegion(hw,id, x, knobsize>h?knoby:y, x2, knobsize>h?knoby+knobsize:y+h, hmargin,knobx,knobx+knobsize);
    drawCaption(hw,x,y,w,caption,captcol,font,id);
}
#endif //HGUI_SLIDERS

#ifdef HGUI_PROGBARS
void drawHProgBar(hWindow *hw, int x, int y, int x2, int y2, char bevel, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long textcol, unsigned long captcol, float pos, float scale, XFontStruct *font, int id)
{
    int w=x2-x,h=y2-y, i, length=(w-bevel*2)*pos;
    char percent[16];
    TextDim dim;
    if (scale==0 || w<=(int)(2.0/scale)) {
        SETDRAWCOL(bgcol);
        DRAWRECTFILL(x+bevel+length,y+bevel,w-length-bevel*2+1,h-bevel*2+1);
        SETDRAWCOL(fgcol);
        DRAWRECTFILL(x+bevel,y+bevel,length,h-bevel*2+1);
    } else {
        SETDRAWCOL(bgcol);
        DRAWRECTFILL(x+bevel,y+bevel,w-bevel*2+1,h-bevel*2+1);
        SETDRAWCOL(fgcol);
        for(i=0; i<=length-w*scale+1; i+=w*scale) DRAWRECTFILL(x+i+2,y+bevel+1,w*scale-1,h-bevel*2-1);
    }
    SETDRAWCOL(fadeCol(bgcol,-hw->contrast));
    for(i=0; i<bevel; i++) {
        DRAWLINE(x+i,y+i,x+w-i,y+i);
        DRAWLINE(x+i,y+i,x+i,y+h-i);
    }
    SETDRAWCOL(fadeCol(bgcol,hw->contrast));
    for(i=0; i<bevel; i++) {
        DRAWLINE(x+i,y+h-i,x+w-i,y+h-i);
        DRAWLINE(x+w-i,y+i,x+w-i,y+h-i);
    }
    if (font!=NULL) {
        sprintf(percent,"%2.f%%",pos*100);
        dim=drawCenText(hw,x,y,w,h,percent,textcol,font);
    }
    setActiveRegion(hw,id, x, y, x2, y2, 0,0,0);
    drawCaption(hw,x,y,w,caption,captcol,font,id);
}

void drawVProgBar(hWindow *hw, int x, int y, int x2, int y2, char bevel, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long textcol, unsigned long captcol, float pos, float scale, XFontStruct *font, int id)
{
    int w=x2-x,h=y2-y, i, length=(h-bevel*2+1)*pos;
    char percent[16];
    TextDim dim;
    if (scale==0 || h<=(int)(2.0/scale)) {
        SETDRAWCOL(bgcol);
        DRAWRECTFILL(x+bevel,y+bevel,w-bevel*2+1,h-bevel-length-1);
        SETDRAWCOL(fgcol);
        DRAWRECTFILL(x+bevel,y2-bevel-length+1,w-bevel*2+1,length);
    } else {
        SETDRAWCOL(bgcol);
        DRAWRECTFILL(x+bevel,y+bevel,w-bevel*2+1,h-bevel*2+1);
        SETDRAWCOL(fgcol);
        for(i=0; i<=length-h*scale+2; i+=h*scale) DRAWRECTFILL(x+bevel+1,y2+bevel-i-h*scale-2,w-bevel*2-1,h*scale-1);
    }
    SETDRAWCOL(fadeCol(bgcol,-hw->contrast));
    for(i=0; i<bevel; i++) {
        DRAWLINE(x+i,y+i,x+w-i,y+i);
        DRAWLINE(x+i,y+i,x+i,y+h-i);
    }
    SETDRAWCOL(fadeCol(bgcol,hw->contrast));
    for(i=0; i<bevel; i++) {
        DRAWLINE(x+i,y+h-i,x+w-i,y+h-i);
        DRAWLINE(x+w-i,y+i,x+w-i,y+h-i);
    }
    if (font!=NULL) {
        sprintf(percent,"%2.f%%",pos*100);
        dim=drawCenText(hw,x,y,w,h,percent,textcol,font);
    }
    setActiveRegion(hw,id, x, y, x2, y2, 0,0,0); //setActiveRegion(hw,id, (dim.x<x)?dim.x:x, y, (dim.x+dim.w>x2)?dim.x+dim.w:x2, y2, 0,0,0);
    drawCaption(hw,x,y,w,caption,captcol,font,id);
}
#endif //HGUI_PROGBARS

#ifdef HGUI_BLOCKS
void drawBlock(hWindow *hw, int x1, int y1, int x2, int y2)
{
    SETDRAWCOL(fadeCol(hw->bgcol,hw->contrast));
    DRAWLINE(x1,y1+1,x2,y1+1);
    DRAWLINE(x1+1,y2+1,x2,y2+1);
    DRAWLINE(x1+1,y1,x1+1,y2);
    DRAWLINE(x2+1,y1+1,x2+1,y2+1);
    SETDRAWCOL(fadeCol(hw->bgcol,-hw->contrast));
    DRAWLINE(x1,y1,x2-1,y1);
    DRAWLINE(x1,y2,x2,y2);
    DRAWLINE(x1,y1,x1,y2);
    DRAWLINE(x2,y1,x2,y2);
}

void drawBlockFill(hWindow *hw, int x1, int y1, int x2, int y2, unsigned long color)
{
    drawBlock(hw,x1-1,y1-1,x2,y2);
    SETDRAWCOL(color);
    DRAWRECTFILL(x1,y1,x2-x1,y2-y1);
}
#endif //HGUI_BLOCKS

#ifdef HGUI_SCROLLBARS
void drawVScroll(hWindow *hw, int x, int y, int w, int y2, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, float pos, float ratio, char press, int id)
{
    if (ratio>=1.0) ratio=1.0;
    if (pos<0||pos>=1.0) pos=0;
    int i, h=y2-y, knobarea=h-SCROLLBUTTSIZE*2, knobsize=knobarea*ratio, knoby=y+SCROLLBUTTSIZE+pos*knobarea, butt2y=y2-SCROLLBUTTSIZE;
    XPoint downarrow[3],uparrow[3];
    drawBlockFill(hw,x,y,x+w,y+h,bgcol);
    SETDRAWCOL(fadeCol(knobcol,press&1?-BUTTFADE:0));
    DRAWRECTFILL(x+1,y+1,w-2,SCROLLBUTTSIZE-1);
    SETDRAWCOL(fadeCol(knobcol,press&2?-BUTTFADE:0));
    DRAWRECTFILL(x+1,butt2y-1,w-2,SCROLLBUTTSIZE); //XSetFillStyle(hw->d,hw->gc,FillStippled);
    if(knoby+knobsize>butt2y) knobsize=butt2y-knoby;
    if(knobsize<3)knobsize=3;
    if(knobarea>2) {
        SETDRAWCOL(knobcol);
        DRAWRECTFILL(x+1,knoby,w-2,knobsize-2);
    }
    SETDRAWCOL(fadeCol(knobcol,hw->contrast));
    if(knobarea>2) DRAWLINE(x+1,knoby+1,x+w-2,knoby+1);
    if((press&2)==0)DRAWLINE(x+1,butt2y,x+w-2,butt2y);
    if((press&1)==0)DRAWLINE(x+2,y+1,x+w-3,y+1);
    SETDRAWCOL(fadeCol(knobcol,-hw->contrast));
    if(knobarea>2) DRAWLINE(x+1,knoby+knobsize-1,x+w-1,knoby+knobsize-1);
    if((press&1)==0) DRAWLINE(x+1,y+SCROLLBUTTSIZE+1,x+w-1,y+SCROLLBUTTSIZE+1);
    for (i=0; i<3; i++) {
        uparrow[i].x=x+w/2+UpArrowPoints[i*2];
        uparrow[i].y=y+UpArrowPoints[i*2+1];
        downarrow[i].x=x+w/2+DownArrowPoints[i*2];
        downarrow[i].y=butt2y+DownArrowPoints[i*2+1];
    }
    SETDRAWCOL(pos+ratio<1.0?fgcol:fadeCol(knobcol,-0.2));
    XFillPolygon(hw->d,hw->w,hw->gc,downarrow,3,Convex,CoordModeOrigin);
    SETDRAWCOL(pos>0?fgcol:fadeCol(knobcol,-0.2));
    XFillPolygon(hw->d,hw->w,hw->gc,uparrow,3,Convex,CoordModeOrigin);
    setActiveRegion(hw,id,x,y,x+w,y2,0,knoby,knoby+knobsize); //printf("%f,%f\n",pos,ratio);
}

void drawHScroll(hWindow *hw, int x, int y, int x2, int h, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, float pos, float ratio, char press, int id)
{
    if (ratio>=1.0) ratio=1.0;
    if (pos<0||pos>=1.0) pos=0;
    int i, w=x2-x, knobarea=w-SCROLLBUTTSIZE*2, knobsize=knobarea*ratio, knobx=x+SCROLLBUTTSIZE+pos*knobarea, butt2x=x2-SCROLLBUTTSIZE;
    XPoint rightarrow[3], leftarrow[3];
    drawBlockFill(hw,x,y,x+w,y+h,bgcol);
    SETDRAWCOL(fadeCol(knobcol,press&1?-BUTTFADE:0));
    DRAWRECTFILL(x+1,y+1,SCROLLBUTTSIZE-1,h-2);
    SETDRAWCOL(fadeCol(knobcol,press&2?-BUTTFADE:0));
    DRAWRECTFILL(butt2x-1,y+1,SCROLLBUTTSIZE,h-2);
    if(knobx+knobsize>butt2x) knobsize=butt2x-knobx;
    if(knobsize<3)knobsize=3;
    if(knobarea>2) {
        SETDRAWCOL(knobcol);
        DRAWRECTFILL(knobx,y+1,knobsize-2,h-2);
    }
    SETDRAWCOL(fadeCol(knobcol,hw->contrast));
    if(knobarea>2) DRAWLINE(knobx+1,y+1,knobx+1,y+h-2);
    if((press&2)==0)DRAWLINE(butt2x,y+1,butt2x,y+h-2);
    if((press&1)==0)DRAWLINE(x+1,y+2,x+1,y+h-3);
    SETDRAWCOL(fadeCol(knobcol,-hw->contrast));
    if(knobarea>2) DRAWLINE(knobx+knobsize-1,y+1,knobx+knobsize-1,y+h-1);
    if((press&1)==0) DRAWLINE(x+SCROLLBUTTSIZE+1,y+1,x+SCROLLBUTTSIZE+1,y+h-1);
    for (i=0; i<3; i++) {
        leftarrow[i].x=x+LeftArrowPoints[i*2];
        leftarrow[i].y=y+h/2+LeftArrowPoints[i*2+1];
        rightarrow[i].x=butt2x+RightArrowPoints[i*2];
        rightarrow[i].y=y+h/2+RightArrowPoints[i*2+1];
    }
    SETDRAWCOL(pos+ratio<1.0?fgcol:fadeCol(knobcol,-0.2));
    XFillPolygon(hw->d,hw->w,hw->gc,rightarrow,3,Convex,CoordModeOrigin);
    SETDRAWCOL(pos>0?fgcol:fadeCol(knobcol,-0.2));
    XFillPolygon(hw->d,hw->w,hw->gc,leftarrow,3,Convex,CoordModeOrigin);
    setActiveRegion(hw,id,x,y,x2,y+h,0,knobx,knobx+knobsize); //printf("%f,%f\n",pos,ratio);
}
#endif //HGUI_SCROLLBARS



//------------- composite widgets made from other widgets --------------
#ifdef HGUI_TEXTBOXES
void drawTextBox(hWindow *hw, int x1, int y1, int x2, int y2, char *content, unsigned long bgcol, unsigned long fgcol, char wrap, char scrollbars, long vpos, int hpos, XFontStruct *font, int id, Widget *vscroll, Widget *hscroll)
{
    //composite widget (textblock + scrollbars if requested)
    long pixrows;
    int w,h;
    TextBoxDim dim;
    if(wrap) hpos=0;
    w=x2-x1;
    h=y2-y1;
    drawBlockFill(hw,x1,y1,x2,y2,bgcol);
    dim=drawTextBlock(hw,x1+1,y1+1,x2-1,y2-1,content,fgcol,wrap,hpos,vpos,font,scrollbars&1,scrollbars&2); //printf("\n---%d-%d-%d---%s\n",dim.w,dim.h,dim.rowcount,content);
    setActiveRegion(hw,id,x1,y1,x2,y2,0,0,0);
    hw->W[id].vpixpos=dim.vpixpos;
    hw->W[id].vpixsize=dim.h;
    hw->W[id].hpixsize=dim.w;
    if(scrollbars&1) {
        vscroll->pos=(float)dim.vpixpos/(float)dim.h;
        vscroll->ratio=h/(float)dim.h ;
        refreshWidget(vscroll);
    }
    if(scrollbars&2) {
        hscroll->pos=(float)hpos/(float)dim.w;    //printf("%d,%d\n",dim.vpixpos,dim.h);
        hscroll->ratio=w/(float)dim.w ;
        refreshWidget(hscroll);
    }
}
//void drawTextBox(int x1, int y1, int x2, int y2, char *content, int pos) {drawTextBox(x1,y1,x2,y2,content,BGCOLOR,FGCOLOR,1,1,pos,DEFAULTFONT) }
#endif //HGUI_TEXTBOXES

//inputfield,list,radiobutton/checkbutton,tab,menu/pulldown-menu,colorchooser,filedialog...

#ifdef HGUI_FILEBROWSER
void filedialog() //composite widget (made from textboxes,scrollbar,buttons,etc.)
{

}
#endif //HGUI_FILEBROWSER




//-------------------------- widget adder routines ---------------------
void emptyCallback() {} //NULL can be used if safely handled in safeCallback()

Widget* denyWidget(char type, char* text)
{
    char name[16];
    int i,j=0;
    if(text!=NULL) {
        for(i=j=0; i<16; i++) if(text[i]!='\n' && text[i]!='\r') {
                name[j]=text[i];    //strncpy(name,text,16);
                j++;
            }
    }
    name[j]='\0';
    printf("%s Window doesn't exist. Not adding widget of type %d with text/content: \"%s...\"\n",ErrPfx,type,name);
    return EmptyWidget;
}

void* addWidget(hWindow *hw, char type, int x, int y, int x2w, int y2h, int zh, long zv, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, unsigned long captcol,
                void *data, float pos, float ratio, char state, char opt1, char opt2, char opt3, XFontStruct *fsp, void (*callback)(), void* argptr )
{
    if(hw==NULL) return denyWidget(type,data);
    int L=hw->WidgetAmount;
    if(L>=WIDGET_AMOUNT_MAX) {
        printf("%s Cannot add more widgets, reached the maximum %d. (Check WIDGET_AMOUNT_MAX in hgui.c)\n",ErrPfx,WIDGET_AMOUNT_MAX);
        return NULL;
    }
    hw->W[L].type=type;
    hw->W[L].x=x;
    hw->W[L].y=y;
    hw->W[L].x2w=x2w;
    hw->W[L].y2h=y2h;
    hw->W[L].zh=zh;
    hw->W[L].zv=zv;
    hw->W[L].bgcol=bgcol;
    hw->W[L].fgcol=fgcol;
    hw->W[L].knobcol=knobcol;
    hw->W[L].captcol=captcol;
    hw->W[L].data=data;
    hw->W[L].pos=pos;
    hw->W[L].ratio=ratio;
    hw->W[L].state=state;
    hw->W[L].opt1=opt1;
    hw->W[L].opt2=opt2;
    hw->W[L].opt3=opt3;
    hw->W[L].fsp=fsp;
    hw->W[L].callback=callback;
    hw->W[L].actx1=hw->W[L].acty1=hw->W[L].actx2=hw->W[L].acty2=hw->W[L].hpixsize=hw->W[L].knobx1=hw->W[L].knobx2=0;
    hw->W[L].id=L;
    hw->W[L].parenthw=hw;
    hw->W[L].argptr=argptr;
    hw->W[L].parentbox=NULL;
    return &hw->W[hw->WidgetAmount++];
} //init actx1,acty1,actx2,acty2 to 0, drawAllWidgets() widget-routines will init/refresh them

#ifdef HGUI_BUTTONS
#ifdef HGUI_TEXTBUTTONS
Button* addButton(hWindow *hw, int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol, char state, char bevel, char border, char *font, void (*callback)(), void* argptr )
{
    return addWidget(hw,BUTTON,x,y,w,h,border,bevel,bgcol,fgcol,0,0, title,0,0,state,0,0,0,getFontStruct(hw,font),callback,argptr);
}
ButtonDyn* addButtonDyn(hWindow *hw, int x1, int y1, int x2, int y2, char *title, unsigned long bgcol, unsigned long fgcol, char state, char bevel, char border, char *font, void (*callback)(), void* argptr )
{
    return addWidget(hw,BUTTONDYN,x1,y1,x2,y2,border,bevel,bgcol,fgcol,0,0, title,0,0,state,0,0,0,getFontStruct(hw,font),callback,argptr);
}
#endif //HGUI_TEXTBUTTONS
#ifdef HGUI_PICBUTTONS
ButtonXPM* addButtonXPM(hWindow *hw, int x, int y, int w, int h, char *xpm[], unsigned long bgcol, char state, char bevel, char border, void (*callback)(), void* argptr )
{
    return addWidget(hw,BUTTONXPM,x,y,w,h,border,bevel,bgcol,0,0,0, (char*)xpm,0,0,state,0,0,0,NULL,callback,argptr);
}
#endif //HGUI_PICBUTTONS
#endif //HGUI_BUTTONS
#ifdef HGUI_SLIDERS
VSlider* addVSlider(hWindow *hw, int x, int y1, int w, int y2, char knobsize, char scalewidth, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, unsigned long captcol, float pos, float scale, char *font, void (*callback)(), void* argptr )
{
    return addWidget(hw,VSLIDER,x,y1,w,y2,knobsize,scalewidth,bgcol,fgcol,knobcol,captcol, caption,pos,scale,0,0,0,0,getFontStruct(hw,font),callback,argptr);
}
HSlider* addHSlider(hWindow *hw, int x1, int y, int x2, int h, char knobsize, char scalewidth, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, unsigned long captcol, float pos, float scale, char *font, void (*callback)(), void* argptr )
{
    return addWidget(hw,HSLIDER,x1,y,x2,h,knobsize,scalewidth,bgcol,fgcol,knobcol,captcol, caption,pos,scale,0,0,0,0,getFontStruct(hw,font),callback,argptr);
}
#endif //HGUI_SLIDERS
#ifdef HGUI_TEXTFIELDS
#ifdef HGUI_TEXTLINES
TextLine* addTextLine(hWindow *hw, int x, int y, char *text, unsigned long color, char *font)
{
    return addWidget(hw,TEXTLINE,x,y,0,0,0,0,0x000000,color,0,0, text,0,0,0,0,0,0,getFontStruct(hw,font),emptyCallback,NULL);
}
TextCentered* addTextCentered(hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long color, char *font)
{
    return addWidget(hw,TEXTCENTER,x1,y1,x2,y2,0,0,0x000000,color,0,0, text,0,0,0,0,0,0,getFontStruct(hw,font),emptyCallback,NULL);
}
#endif //HGUI_TEXTLINES
#ifdef HGUI_TEXTBLOCKS
TextBlock* addTextBlock(hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long color, char wrap, char *font)
{
    return addWidget(hw,TEXTBLOCK,x1,y1,x2,y2,0,0,0x000000,color,0,0, text,0,0,0,wrap,0,0,getFontStruct(hw,font),emptyCallback,NULL);
}
#ifdef HGUI_TEXTBOXES
TextBox* addTextBox(hWindow *hw, int x1, int y1, int x2, int y2, char *content, unsigned long bgcol, unsigned long fgcol, char wrap, char scrollbars, char autofit, long vpos, int hpos, char *font)
{
    if(hw==NULL) return denyWidget(TEXTBOX,content);
    TextBox* wp=addWidget(hw,TEXTBOX,x1,y1,(scrollbars&1)?x2-SCROLLBUTTSIZE-3:x2,(scrollbars&2)?y2-SCROLLBUTTSIZE-3:y2,hpos,vpos,bgcol,fgcol,0,0, content,0,0,0,wrap,scrollbars,autofit,getFontStruct(hw,font),emptyCallback,NULL);
    if (scrollbars&2) {
        Widget *hs=addWidget(hw,HSCROLL,x1,y2-SCROLLBUTTSIZE,x2-SCROLLBUTTSIZE-3,SCROLLBUTTSIZE,0,0,SCROLLBGCOL,SCROLLFGCOL,SCROLLKNOBCOL,0, NULL,0.0,0.0,0+0,0,0,0,NULL,emptyCallback,NULL);
        wp->hscroll=hs;
        hs->parentbox=wp;
    }
    if (scrollbars&1) {
        Widget *vs=addWidget(hw,VSCROLL,x2-SCROLLBUTTSIZE,y1,SCROLLBUTTSIZE,y2,0,0,SCROLLBGCOL,SCROLLFGCOL,SCROLLKNOBCOL,0, NULL,0.0,0.0,0+0,0,0,0,NULL,emptyCallback,NULL);
        wp->vscroll=vs;
        vs->parentbox=wp;
    }
    return wp;
}
#endif //HGUI_TEXTBOXES
#endif //HGUI_TEXTBLOCKS
#endif //HGUI_TEXTFIELDS
#ifdef HGUI_PROGBARS
HProgBar* addHProgBar(hWindow *hw, int x1, int y1, int x2, int y2, char bevel, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long textcol, unsigned long captcol, float pos, float scale, char* font, void (*callback)(), void* argptr )
{
    return addWidget(hw,HPROGBAR,x1,y1,x2,y2,0,bevel,bgcol,fgcol,textcol,captcol, caption,pos,scale,0,0,0,0,getFontStruct(hw,font),callback,argptr);
}
VProgBar* addVProgBar(hWindow *hw, int x1, int y1, int x2, int y2, char bevel, char *caption, unsigned long bgcol, unsigned long fgcol, unsigned long textcol, unsigned long captcol, float pos, float scale, char* font, void (*callback)(), void* argptr )
{
    return addWidget(hw,VPROGBAR,x1,y1,x2,y2,0,bevel,bgcol,fgcol,textcol,captcol, caption,pos,scale,0,0,0,0,getFontStruct(hw,font),callback,argptr);
}
#endif //HGUI_PROGBARS
#ifdef HGUI_SCROLLBARS
VScroll* addVScroll(hWindow *hw, int x, int y1, int w, int y2, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, float pos, float ratio, void (*callback)(), void* argptr )
{
    return addWidget(hw,VSCROLL,x,y1,w,y2,0,0,bgcol,fgcol,knobcol,0, NULL,pos,ratio,0+0,0,0,0,NULL,callback,argptr);
}
HScroll* addHScroll(hWindow *hw, int x1, int y, int x2, int h, unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, float pos, float ratio, void (*callback)(), void* argptr )
{
    return addWidget(hw,HSCROLL,x1,y,x2,h,0,0,bgcol,fgcol,knobcol,0, NULL,pos,ratio,0+0,0,0,0,NULL,callback,argptr);
}
#endif //HGUI_SCROLLBARS
#ifdef HGUI_BLOCKS
Block* addBlock(hWindow *hw, int x1, int y1, int x2, int y2)
{
    return addWidget(hw,BLOCK,x1,y1,x2,y2,0,0,0,0,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
BlockFill* addBlockFill(hWindow *hw, int x1, int y1, int x2, int y2, unsigned int color)
{
    return addWidget(hw,BLOCKFILL,x1,y1,x2,y2,0,0,color,0,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
#endif //HGUI_BLOCKS
#ifdef HGUI_PICTURES
Picture* addPicture(hWindow *hw, int x, int y, int width, int height, int depth, char* pixeldata, void (*callback)(), void* argptr )
{
    return addWidget(hw,PICTURE,x,y,width,height,depth,0,0,0,0,0, pixeldata,0,0,0,0,0,0,NULL,callback,argptr);
}
PictureXPM* addXPM(hWindow *hw, int x, int y, char *xpm[], unsigned long bgcol, void (*callback)(), void* argptr )
{
    return addWidget(hw,XPM,x,y,0,0,0,0,bgcol,0,0,0, (void*)xpm,0,0,0,0,0,0,NULL,callback,argptr);
}
#endif //HGUI_PICTURES
#ifdef HGUI_ANIMATIONS
int addAnimation(hWindow *hw)
{
    if(hw==NULL || hw->AnimAmount>=ANIM_AMOUNT_MAX) {
        printf("%s Cannot add more animations, reached the maximum %d. (Check ANIM_AMOUNT_MAX in hgui.c)\n",ErrPfx,ANIM_AMOUNT_MAX);
        return RET_ERR;
    }
    hw->AnimID[hw->AnimAmount] = hw->WidgetAmount ;
    hw->AnimAmount++;
    return hw->AnimAmount;
}
#ifdef HGUI_PICANIMS
AnimXPM* addAnimXPM(hWindow *hw, int x, int y, void* xpmlist[], unsigned long bgcol, char framedelay, void (*callback)(), void* argptr )
{
    if(hw==NULL) return denyWidget(ANIMXPM,"");
    if(addAnimation(hw)==RET_ERR) return NULL;
    else return addWidget(hw,ANIMXPM,x,y,0,0,0,0,bgcol,0,0,0, (void*)xpmlist, 0,0,0,framedelay,0,0,NULL,callback,argptr);
}
#endif //HGUI_PICANIMS
#ifdef HGUI_TEXTANIMS
TextScroll* addTextScroll(hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long bgcol, unsigned long fgcol, char framedelay, char pixstep, char *font)
{
    if(hw==NULL) return denyWidget(TEXTSCROLL,NULL);
    if(addAnimation(hw)==RET_ERR) return NULL;
    else return addWidget(hw,TEXTSCROLL,x1,y1,x2,y2,0,0,bgcol,fgcol,1,0, text,0,0,0,framedelay,0,pixstep,getFontStruct(hw,font), emptyCallback,NULL);
}
#endif //HGUI_TEXTANIMS
#endif //HGUI_ANIMATIONS
#ifdef HGUI_PRIMITIVES
Primitive* addPoint (hWindow *hw, int x, int y, unsigned long col)
{
    return addWidget(hw,POINT,x,y,0,0,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addLine (hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col)
{
    return addWidget(hw,LINE,x1,y1,x2,y2,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addRect (hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col)
{
    return addWidget(hw,RECT,x1,y1,x2,y2,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addRectFill (hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col)
{
    return addWidget(hw,RECTFILL,x1,y1,x2,y2,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addArc (hWindow *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col)
{
    return addWidget(hw,ARC,x1,y1,x2,y2,startdeg,degree,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addArcFill (hWindow *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col)
{
    return addWidget(hw,ARCFILL,x1,y1,x2,y2,startdeg,degree,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addCircle (hWindow *hw, int x, int y, int r, unsigned long col)
{
    return addWidget(hw,CIRCLE,x,y,r,0,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addCircleFill (hWindow *hw, int x, int y, int r, unsigned long col)
{
    return addWidget(hw,CIRCLEFILL,x,y,r,0,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addEllipse (hWindow *hw, int x, int y, int rx, int ry, unsigned long col)
{
    return addWidget(hw,ELLIPSE,x,y,rx,ry,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
Primitive* addEllipseFill (hWindow *hw, int x, int y, int rx, int ry, unsigned long col)
{
    return addWidget(hw,ELLIPSEFILL,x,y,rx,ry,0,0,0,col,0,0, NULL,0,0,0,0,0,0,NULL,emptyCallback,NULL);
}
#endif //HGUI_PRIMITIVES

//widget short (and automatic width/height/font/etc.) forms of add-routines:
//void addButton(char *title){}
//void addButton(int x, int y, char *title){}




//----------------------- Window/Widget redraw/refresh-routines -----------------------
void correctRange(float *input)
{
    if (*input>1.0) *input=1.0;
    else if (*input<0.0) *input=0.0;
}

void cleanUpRect(hWindow *hw, int x1o, int y1o, int x2o, int y2o,  int x1, int y1, int x2, int y2)
{
    //clears (with window-background-color) the non-intersecting area of two rectangles: the old bouncing box and the new one
//XClearArea(hw->d,hw->w,x1o,y1o,x2o-x1o,y2o-y1o,False);
    if(x1o<x1) XClearArea(hw->d,hw->w,x1o,y1o,x1-x1o,y2o-y1o,False);
    if(y1o<y1) XClearArea(hw->d,hw->w,x1o,y1o,x2o-x1o,y1-y1o,False);
    if(x2o>x2) XClearArea(hw->d,hw->w,x2,y1o,x2o-x2,y2o-y1o,False);
    if(y2o>y2) XClearArea(hw->d,hw->w,x1o,y2,x2o-x1o,y2o-y2,False);
}

#ifdef HGUI_PRIMITIVES
void drawPrimitive(hWindow *hw, Widget *wp, int x1, int y1, int x2, int y2, unsigned long color)
{
    int *x=&x1, *y=&y1;
    switch (wp->type) {
    case POINT:
        drawPoint(hw,*x,*y,color);
        break;
    case LINE:
        drawLine(hw,x1,y1,x2,y2,color);
        break;
    case RECT:
        drawRect(hw,x1,y1,x2,y2,color);
        break;
    case RECTFILL:
        drawRectFill(hw,x1,y1,x2,y2,color);
        break;
    case ARC:
        drawArc(hw,x1,y1,x2,y2,wp->startdeg,wp->degree,color);
        break;
    case ARCFILL:
        drawArcFill(hw,x1,y1,x2,y2,wp->startdeg,wp->degree,color);
        break;
    case CIRCLE:
        drawCircle(hw,*x,*y,wp->r,color);
        break;
    case CIRCLEFILL:
        drawCircleFill(hw,*x,*y,wp->r,color);
        break;
    case ELLIPSE:
        drawEllipse(hw,*x,*y,wp->rx,wp->ry,color);
        break;
    case ELLIPSEFILL:
        drawEllipseFill(hw,*x,*y,wp->rx,wp->ry,color);
        break;
    }
}
#endif //HGUI_PRIMITIVES

void clearWidgetTrails(hWindow *hw, int i, Bool TextChange)
{
    if(hw==NULL) return;
    Widget *wp=&hw->W[i];
    int x1,y1,x2,y2,x1o,y1o,x2o,y2o,brd, *x=&x1,*y=&y1;
    x1o=wp->actx1;
    y1o=wp->acty1;
    x2o=wp->actx2;
    y2o=wp->acty2;
    brd=wp->border;
    x1=dynCoordX(hw,wp->x1);
    y1=dynCoordY(hw,wp->y1);
    x2=dynCoordX(hw,wp->x2);
    y2=dynCoordY(hw,wp->y2);
//if(x1==x1o && y1==y1o && x2==x2o && y2==y2o) return; //if no moving/resizing of widget, no trail to clear //printf("%d,%d,%d,%d, %d,%d,%d,%d\n",x1,y1,x2,y2, x1o,y1o,x2o,y2o);
    if(x1o<=0 || x2o<=0 || x2o<=x1o || y2o<=y1o) return; //invalid rectangle returns
    switch (wp->type) {
#ifdef HGUI_BUTTONS
    case BUTTON:
    case BUTTONXPM:
        cleanUpRect(hw, x1o-brd, y1o-brd, x2o+brd+1, y2o+brd+1, *x-brd,*y-brd,*x+wp->w+brd+1,*y+wp->h+brd+1);
        XClearArea(hw->d,hw->w,*x-brd,*y-brd,1,1,False);
        XClearArea(hw->d,hw->w,*x-brd,*y+wp->h+brd,1,1,False);
        XClearArea(hw->d,hw->w,*x+wp->w+brd,*y-brd,1,1,False);
        XClearArea(hw->d,hw->w,*x+wp->w+brd,*y+wp->h+brd,1,1,False);
        break;
    case BUTTONDYN:
        cleanUpRect(hw, x1o-brd, y1o-brd, x2o+brd+1, y2o+brd+1, x1-brd,y1-brd,x2+brd+1,y2+brd+1);
        XClearArea(hw->d,hw->w,x1-brd,y1-brd,1,1,False);
        XClearArea(hw->d,hw->w,x1-brd,y2+brd,1,1,False);
        XClearArea(hw->d,hw->w,x2+brd,y1-brd,1,1,False);
        XClearArea(hw->d,hw->w,x2+brd,y2+brd,1,1,False);
        break;
#endif //HGUI_BUTTONS
#ifdef HGUI_SLIDERS
    case VSLIDER:
        cleanUpRect(hw, x1o, y1o, x2o, y2o, *x,*y,*x+wp->w,y2);
        clearCaptionTrails(hw,*x,y1,wp->w,wp->caption,wp->font,i,TextChange);
        break;
    case HSLIDER:
        cleanUpRect(hw, x1o, y1o, x2o, y2o, *x,*y,x2,*y+wp->h);
        clearCaptionTrails(hw,x1,*y,x2-x1,wp->caption,wp->font,i,TextChange);
        break;
#endif //HGUI_SLIDERS
#ifdef HGUI_PROGBARS
    case HPROGBAR:
    case VPROGBAR:
        cleanUpRect(hw, x1o, y1o, x2o+1, y2o+1, x1,y1,x2+1,y2+1);
        clearCaptionTrails(hw,x1,y1,x2-x1,wp->caption,wp->font,i,TextChange);
        break;
#endif //HGUI_PROGBARS
#ifdef HGUI_TEXTFIELDS
#if defined(HGUI_TEXTLINES) || defined(HGUI_TEXTBLOCKS)
    case TEXTLINE:
    case TEXTCENTER:
    case TEXTBLOCK:
        XClearArea(hw->d,hw->w,x1o,y1o,x2o-x1o,y2o-y1o,False);
        break;
#endif
#ifdef HGUI_TEXTBOXES
    case TEXTBOX:
        cleanUpRect(hw, x1o-1, y1o-1, x2o+2, y2o+2, x1-1,y1-1,x2+2,y2+2);
        break;
#endif //HGUI_TEXTBOXES
#endif //HGUI_TEXTFIELDS
#ifdef HGUI_SCROLLBARS
    case VSCROLL:
        cleanUpRect(hw, x1o-1, y1o-1, x2o+2, y2o+2, *x-1,y1-1,*x+wp->w+1,y2+1);
        break;
    case HSCROLL:
        cleanUpRect(hw, x1o-1, y1o-1, x2o+2, y2o+2, x1-1,*y-1,x2+1,*y+wp->h+1);
        break;
#endif //HGUI_SCROLLBARS
#ifdef HGUI_BLOCKS
    case BLOCK:
        cleanUpRect(hw,x1o,y1o,x1o+2,y2o+1, x1,y1,x1+2,y2+1);
        cleanUpRect(hw,x1o,y1o,x2o+1,y1o+2, x1,y1,x2+1,y1+2);
        cleanUpRect(hw,x2o,y1o,x2o+2,y2o+1, x2,y1,x2+2,y2+1);
        cleanUpRect(hw,x1o,y2o,x2o+2,y2o+2, x1,y2,x2+2,y2+2);
        break;
    case BLOCKFILL:
        cleanUpRect(hw, x1o-1, y1o-1, x2o+2, y2o+2, x1-1,y1-1,x2+2,y2+2);
        break;
#endif //HGUI_BLOCKS
#ifdef HGUI_PICTURES
    case PICTURE:
    case XPM:
        cleanUpRect(hw, x1o,y1o,x2o,y2o, *x,*y,*x+wp->w,*y+wp->h);
        break;
#endif //HGUI_PICTURES
#ifdef HGUI_PICANIMS
    case ANIMXPM:
        XClearArea(hw->d,hw->w,x1o,y1o,x2o-x1o,y2o-y1o,False);
        break;
#endif //HGUI_PICANIMS
#if defined(HGUI_PRIMITIVES) || defined (HGUI_TEXTANIMS)
    case RECTFILL:
    case TEXTSCROLL:
        cleanUpRect(hw,x1o,y1o,x2o,y2o, x1,y1,x2,y2);
        break;
#endif //HGUI_PRIMITIVES || HGUI_TEXTANIMS
#ifdef HGUI_PRIMITIVES
    case CIRCLE:
    case CIRCLEFILL:
    case ELLIPSE:
    case ELLIPSEFILL:
        if(x1==x1o && y1==y1o) return;
    default:
        drawPrimitive(hw,wp,x1o,y1o,x2o,y2o,hw->bgcol); //GXxor graphic context could be used here to avoid flickering when moving primitives?
#endif //HGUI_PRIMITIVES
    }
}

void drawOnlyWidget(hWindow *hw, int i)
{
    if(hw==NULL) return;
    char cvpos=2,chpos=2;
    Widget *wp=&hw->W[i];
    int x1,y1,x2,y2, *x=&x1,*y=&y1;
    XPMstruct xpms;
    TextDim dim; //cvpos/chpos and other position/value corrections are made inside this function where it makes sense
    x1=dynCoordX(hw,wp->x1);
    y1=dynCoordY(hw,wp->y1);
    x2=dynCoordX(hw,wp->x2);
    y2=dynCoordY(hw,wp->y2); //pos might be zoomed or relative to right/bottom window-edges (negative coordinate-values)
    if(x2<x1) x2=x1;
    if (y2<y1) y2=y1; //cleaning up leftover trail-region of widgets could happen here but better be done in the widget routines instead (to handle complexities by protruding captions/knobs)

    switch (wp->type) {
#ifdef HGUI_BUTTONS
#ifdef HGUI_TEXTBUTTONS
    case BUTTON:
        drawButton(hw,*x,*y,wp->w,wp->h,wp->data,wp->bgcol,wp->fgcol,wp->state,wp->bevel,wp->border,wp->fsp,i);
        break;
    case BUTTONDYN:
        drawButton(hw,x1,y1,x2-x1,y2-y1,wp->data,wp->bgcol,wp->fgcol,wp->state,wp->bevel,wp->border,wp->fsp,i);
        break;
#endif //HGUI_TEXTBUTTONS
#ifdef HGUI_PICBUTTONS
    case BUTTONXPM:
        drawButtonXPM(hw,*x,*y,wp->w,wp->h,(void*)wp->pixdata,wp->bgcol,wp->state,wp->bevel,wp->border,i);
        break;
#endif //HGUI_PICBUTTONS
#endif //HGUI_BUTTONS
#ifdef HGUI_SLIDERS
    case VSLIDER:
        correctRange(&wp->pos);
        correctRange(&wp->ratio); //push these between 0.0 and 1.0
        drawVSlider(hw,*x,y1,wp->w,y2,wp->knobsize,wp->scalewidth,wp->caption,wp->bgcol,wp->fgcol,wp->knobcol,wp->captcol,wp->pos,wp->scale,wp->fsp,i);
        break;
    case HSLIDER:
        correctRange(&wp->pos);
        correctRange(&wp->ratio); //push these between 0.0 and 1.0
        drawHSlider(hw,x1,*y,x2,wp->h,wp->knobsize,wp->scalewidth,wp->caption,wp->bgcol,wp->fgcol,wp->knobcol,wp->captcol,wp->pos,wp->scale,wp->fsp,i);
        break;
#endif //HGUI_SLIDERS
#ifdef HGUI_TEXTFIELDS
#ifdef HGUI_TEXTLINES
    case TEXTLINE:
        dim=drawTextLine(hw,*x,*y,255,wp->text,wp->textcol,wp->fsp);
        setActiveRegion(hw,i,*x,*y,*x+dim.w,*y+dim.h,0,0,0);
        break;
    case TEXTCENTER:
        drawTextCent(hw,x1,y1,x2,y2,wp->text,wp->textcol,wp->fsp, i);
        break;
#endif //HGUI_TEXTLINES
#ifdef HGUI_TEXTBLOCKS
    case TEXTBLOCK:
        drawTextBlock(hw,x1,y1,x2,y2,wp->text,wp->textcol,wp->wrap,0,0,wp->fsp,0,0);
        break;
#endif //HGUI_TEXTBLOCKS
#ifdef HGUI_TEXTBOXES
    case TEXTBOX:
        while(cvpos>0||chpos>0) {
            drawTextBox(hw,x1,y1,x2,y2,wp->text,wp->bgcol,wp->fgcol,wp->wrap,wp->scrollbars,wp->vpos,wp->hpos,wp->fsp,i,wp->vscroll,wp->hscroll);
            if(wp->autofit && wp->vpixsize<=wp->acty2-wp->acty1) {
                wp->vpos=0;
                cvpos--;
            } else cvpos=0;
            if(wp->autofit && wp->hpixsize<=wp->actx2-wp->actx1) {
                wp->hpos=0;
                chpos--;
            } else chpos=0;
        }
        break;
#endif //HGUI_TEXTBOXES
#endif //HGUI_TEXTFIELDS
#ifdef HGUI_PROGBARS
    case HPROGBAR:
        correctRange(&wp->pos);
        correctRange(&wp->ratio); //push these between 0.0 and 1.0
        drawHProgBar(hw,x1,y1,x2,y2,wp->bevel,wp->caption,wp->bgcol,wp->fgcol,wp->progtextcol,wp->captcol,wp->pos,wp->scale,wp->fsp,i);
        break;
    case VPROGBAR:
        correctRange(&wp->pos);
        correctRange(&wp->ratio); //push these between 0.0 and 1.0
        drawVProgBar(hw,x1,y1,x2,y2,wp->bevel,wp->caption,wp->bgcol,wp->fgcol,wp->progtextcol,wp->captcol,wp->pos,wp->scale,wp->fsp,i);
        break;
#endif //HGUI_PROGBARS
#ifdef HGUI_SCROLLBARS
    case VSCROLL:
        correctRange(&wp->pos);
        correctRange(&wp->ratio); //push these between 0.0 and 1.0
        drawVScroll(hw,*x,y1,wp->w,y2,wp->bgcol,wp->fgcol,wp->knobcol,wp->pos,wp->ratio,wp->buttonstate,i);
        break;
    case HSCROLL:
        correctRange(&wp->pos);
        correctRange(&wp->ratio); //push these between 0.0 and 1.0
        drawHScroll(hw,x1,*y,x2,wp->h,wp->bgcol,wp->fgcol,wp->knobcol,wp->pos,wp->ratio,wp->buttonstate,i);
        break;
#endif //HGUI_SCROLLBARS
#ifdef HGUI_BLOCKS
    case BLOCK:
        drawBlock(hw,x1,y1,x2,y2);
        break;
    case BLOCKFILL:
        drawBlockFill(hw,x1,y1,x2,y2,wp->color);
        break;
#endif //HGUI_BLOCKS
#ifdef HGUI_PICTURES
    case PICTURE:
        putPicture(hw,*x,*y,wp->pixdata,wp->w,wp->h,wp->depth,i);
        setActiveRegion(hw,i,*x,*y,*x+wp->w,*y+wp->h,0,0,0);
        break;
    case XPM:
        xpms=putXPM(hw,*x,*y,(void*)wp->xpm,wp->transpcol,i);
        wp->w=xpms.width;
        wp->h=xpms.height;
        setActiveRegion(hw,i,*x,*y,*x+xpms.width,*y+xpms.height,0,0,0);
        break;
#endif //HGUI_PICTURES
#ifdef HGUI_ANIMATIONS
#ifdef HGUI_TEXTANIMS
    case TEXTSCROLL:
        drawTextScrollFrame(hw,wp, x1,y1,x2,y2, wp->text);
        if(wp->scrolloffset>=x2-x1) wp->scrolloffset=x2-x1;
        break;
#endif //HGUI_TEXTANIMS
#ifdef HGUI_PICANIMS
    case ANIMXPM:
        putAnimXPMframe(hw,wp->id,*x,*y,wp->bgcol,wp->xpmlist,wp->framecount);
        break;
#endif //HGUI_PICANIMS
#endif //HGUI_ANIMATIONS
    }
#ifdef HGUI_PRIMITIVES
    drawPrimitive(hw,wp,x1,y1,x2,y2,wp->fgcol);
#endif //HGUI_PRIMITIVES
    switch(wp->type) {
        //bouncing boxes might be needed for callback even for primitives
#ifdef HGUI_PRIMITIVES
    case POINT:
    case CIRCLE:
    case CIRCLEFILL: //setActiveRegion(hw,i,*x-wp->r,*y-wp->r,*x+wp->r,*y+wp->r,0,0,0); break;
    case ELLIPSE:
    case ELLIPSEFILL:
        x2=x1+1;
        y2=y1+1; //setActiveRegion(hw,i,*x-wp->rx,*y-wp->ry,*x+wp->rx,*y+wp->ry,0,0,0); break;
    case LINE:
    case RECT:
    case ARC:
    case ARCFILL:
#endif //HGUI_PRIMITIVES
    case TEXTBLOCK:
    case BLOCK:
    case BLOCKFILL:
    case TEXTSCROLL:
    case RECTFILL:
        setActiveRegion(hw,i,x1,y1,x2,y2,0,0,0);
        break;
    }
//#ifdef DEBUG
    //SETDRAWCOL(0xffffff); DRAWRECT(wp->actx1,wp->acty1,wp->actx2-wp->actx1,wp->acty2-wp->acty1);
//#endif
}

void refreshWidget(Widget *wp)
{
    hWindow *hw=wp->parenthw;
    if(hw==NULL) return;
    drawOnlyWidget(hw,wp->id);
    XFlush(hw->d);
}

void redrawWidget(void *widgetptr)
{
    Widget *wp=widgetptr;
    hWindow *hw=wp->parenthw;
    if(hw==NULL) return;
    hw->width=getWinWidth(hw);
    hw->height=getWinHeight(hw);
    clearWidgetTrails(hw,wp->id,False);
    drawOnlyWidget(hw,wp->id);
    XFlush(hw->d);
}

void changeTextPtr(void *widgetptr, char *textptr)
{
    Widget *wp=widgetptr;
    hWindow *hw=wp->parenthw;
    if(hw==NULL) return;
    switch (wp->type) {
#if defined(HGUI_SLIDERS) || defined(HGUI_PROGBARS)
    case VSLIDER:
    case HSLIDER:
    case HPROGBAR:
    case VPROGBAR:
        clearWidgetTrails(hw,wp->id,True);
        break; //true ensures full clear of previous caption
#endif
    }
    wp->text=textptr; //caption/text/content, all the same mem.location, but under different names depending on widget-type
    drawOnlyWidget(hw,wp->id);
}

/*void drawWidgets(hWindow *hw, XExposeEvent *exp)
{
 printf("%d,%d,%d,%d\n",exp->x,exp->y,exp->width,exp->height);
}*/

//redrawing background is not a good way, it causes flickering. delete/draw only the background between the widgets or send the whole window as PixMap to server
void drawBackground(hWindow *hw)
{
    if(hw==NULL) return;
    XClearWindow(hw->d,hw->w);
} //or XClearArea(hw->d,hw->w,0,0,0,0,False)//SETDRAWCOL(BGCOLOR); DRAWRECTFILL(0,0,getWinWidth(hw),getWinHeight(hw)); }
//XClearWindow ad XClearArea doesn't work if background-pixmap is 'None'

void redrawBackground(hWindow *hw)
{
    if(hw==NULL) return;    //XExposeEvent *exp)  XClearArea(hw->d,hw->w,exp->x,exp->y,exp->width,exp->height,False);
    int i;
    for(i=0; i<hw->WidgetAmount; i++) clearWidgetTrails(hw,i,False);
}

void  clearWindow (hWindow *hw)
{
    XClearWindow(hw->d,hw->w);
    XFlush(hw->d);
};

void drawAllWidgets(hWindow *hw)
{
    if(hw==NULL) return;
    int i=0;
    for(i=0; i<hw->WidgetAmount; i++) drawOnlyWidget(hw,i); //iterate through widgets in widget-array hw->W[] till last widget
    XFlush(hw->d);
}

void refreshWindow(hWindow *hw)
{
    if(hw==NULL) return;
    hw->width=getWinWidth(hw);
    hw->height=getWinHeight(hw);
    drawAllWidgets(hw);
}

void redrawWindow(hWindow *hw)
{
    if(hw==NULL) return;
    XClearWindow(hw->d,hw->w);
    refreshWindow(hw);
}


#ifdef HGUI_ANIMATIONS
#ifdef HGUI_PICANIMS
Bool putAnimXPMframe(hWindow *hw, int id, int x, int y, unsigned long transpcol, void* xpmlist[], int framenumber )
{
    XPMstruct xpms=putXPM(hw,x,y,xpmlist[framenumber],transpcol,id);
    XFlush(hw->d);
    setActiveRegion(hw,id,x,y,x+xpms.width,y+xpms.height,0,0,0); //hw->W[id].w=xpms.width; hw->W[id].h=xpms.height;
    if(xpmlist[framenumber]==NULL || xpmlist[framenumber+1]==NULL) return False;
    else return True;
}
#endif //HGUI_PICANIMS
#ifdef HGUI_TEXTANIMS
TextDim drawTextScrollFrame(hWindow *hw, Widget *wp, int x1, int y1, int x2, int y2, char *text)
{
    int i;
    int w=x2-x1, h=y2-y1;
    SETDRAWCOL(wp->bgcol);
    DRAWRECTFILL(x1,y1,w,h);  //debug: SETDRAWCOL(0x107050); DRAWRECTFILL(x1+w,y1,80,h); DRAWRECTFILL(x1-80,y1,80,h);
    setClipRegion(hw,x1,y1,x2,y2);
    TextDim dim=getTextDimN(text+wp->scrollindex,wp->font,wp->scrolldisplen);
    DRAWSTRINGN(x1+wp->scrollpixcnt+(w-wp->scrolloffset), y1+(h+dim.ascent)/2, text+wp->scrollindex, wp->fgcol, wp->font, wp->scrolldisplen);
    removeClipRegion(hw);
    return dim;
}

void doTextScroll(hWindow *hw, Widget *wp, int x1, int y1, int x2, int y2, char *text)
{
    int i;
    int w=x2-x1, h=y2-y1;
    TextDim dim=drawTextScrollFrame(hw,wp,x1,y1,x2,y2,text);
    if (text[wp->scrollindex]=='\0') {
        wp->scrollindex=wp->scrollpixcnt=wp->scrolloffset=wp->state=0;
        wp->scrolldisplen=1;
    } else {
        if(wp->scrolloffset<w) { //check if 1st phase of scrolltext when its beginning hasn't reached the field's left edge
            if (wp->scrollpixcnt <= 0) {
                wp->scrollpixcnt=getTextWidth(text+wp->scrolldisplen-1,wp->font,1);
                wp->scrolloffset+=wp->scrollpixcnt;
                if(text[wp->scrollindex+wp->scrolldisplen]=='\0') wp->state=1;
                if(!wp->state) wp->scrolldisplen++;
            }
            if(!wp->state) wp->scrollpixcnt-=wp->pixstep;
            else wp->scrolloffset++; //pull scrolltext toward the field's beginning
        } else {
            if( -wp->scrollpixcnt >= getTextWidth(text+wp->scrollindex,wp->font,1) ) { //check if a character passed
                for(i=0; i<SCROLLTEXTWIDTH_MAX; i++) { //determine length of text to fit into the field
                    if ( text[wp->scrollindex+i]=='\0' ) {
                        wp->scrolldisplen=i-1;    //if scrolltext is over, shorten its display
                        break;
                    } else if( getTextWidth(text+wp->scrollindex,wp->font,i) > w+dim.h*4 ) {
                        wp->scrolldisplen=i;    //otherwise exceed the end of field to cover from right
                        break;
                    }
                }
                wp->scrollpixcnt=0;
                wp->scrollindex++; //advance in scroll-string
            }
            wp->scrollpixcnt-=wp->pixstep; //advance a pixel inside current character
        }
    }
    XFlush(hw->d);
}
#endif

void playAnimations(hWindow *hw)
{
    if(hw==NULL) return;
    int h,i,x1,y1,x2,y2, *x=&x1,*y=&y1;
    Widget *aniw;
    for(i=0; i<hw->AnimAmount; i++) {
        aniw=&hw->W[hw->AnimID[i]];
        if (aniw->spdcnt) {
            aniw->spdcnt--;
            continue;
        } else {
            aniw->spdcnt=aniw->framedelay;
            x1=dynCoordX(hw,aniw->x1);
            y1=dynCoordY(hw,aniw->y1);
            x2=dynCoordX(hw,aniw->x2);
            y2=dynCoordY(hw,aniw->y2);
            if(x2<x1) x2=x1;
            if (y2<y1) y2=y1;
            switch (hw->W[hw->AnimID[i]].type) {
#ifdef HGUI_PICANIMS
            case ANIMXPM: {
                if ( putAnimXPMframe(hw,aniw->id,*x,*y,aniw->bgcol,aniw->xpmlist,aniw->framecount) ) aniw->framecount++;
                else aniw->framecount=0;
            }
            break;
#endif
#ifdef HGUI_TEXTANIMS
            case TEXTSCROLL:
                doTextScroll(hw,aniw,x1,y1,x2,y2,aniw->text);
                break; //DRAWSTRING(aniw->x1, aniw->y1, aniw->text, aniw->fgcol, aniw->font);
#endif
            }
        }
    }
}
#endif //HGUI_ANIMATIONS



//----------------------- Event handling routines ----------------------
Bool alwaysTrue()
{
    return True;   //needed fo XCheckIfEvent, as XCheckWindowEvent / XCheckMaskEvent doesn't handle ClientMessage event-type
}

Bool inActRange (hWindow *hw, int i, int x, int y)
{
    if (hw->W[i].actx1<=x && x<=hw->W[i].actx2 && hw->W[i].acty1<=y && y<=hw->W[i].acty2) return True;
    else return False;
}

void safeCallback(Widget *w, void* argptr)
{
    if (w->callback!=NULL) (w->callback)(w,argptr);
}

void setCallback(void *widgetptr, void (*callback)(), void* argptr)
{
    Widget *wp=widgetptr;
    wp->callback=callback;
    wp->argptr=argptr;
}

scrollTextDown(hWindow *hw, Widget *w, int rows)
{
    if (w->vpixpos+(w->acty2-w->acty1) <= w->vpixsize) w->vpos+=rows;
    refreshWidget(w);
}

scrollTextUp(hWindow *hw, Widget *w, int rows)
{
    if(w->vpos>=0) {
        w->vpos-=rows;
        refreshWidget(w);
    }
}

scrollTextRight(hWindow *hw, Widget *w, int pixrows)
{
    if(w->hpos+(w->actx2-w->actx1) <= w->hpixsize) {
        w->hpos+=pixrows;
        refreshWidget(w);
    }
}

scrollTextLeft(hWindow *hw, Widget *w, int pixrows)
{
    if(w->hpos>=0) {
        w->hpos-=pixrows;
        refreshWidget(w);
    }
}

Bool leftButtonAction(hWindow *hw, Widget *w, int x, int y)
{
    int width=w->actx2-w->actx1, height=w->acty2-w->acty1;
    Widget *par; //parent widget pointer for some widgets
    switch (w->type) {
#ifdef HGUI_BUTTONS
    case BUTTON:
    case BUTTONDYN:
    case BUTTONXPM:
        if(hw->dragging)break;
        w->state^=1;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_SLIDERS
    case VSLIDER:
        w->pos = 1.0 - (float)(y-(w->acty1+w->margin)) / (float)(height-w->margin*2);
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case HSLIDER:
        w->pos = 1.0 - (float)(x-(w->actx1+w->margin)) / (float)(width-w->margin*2);
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_PROGBARS
    case HPROGBAR:
    case VPROGBAR:
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_SCROLLBARS
    case VSCROLL:
        par=w->parentbox;
        if(par!=NULL) {
            if(y > w->knoby2) {
                w->state=2;
                scrollTextDown(hw,par,1);
            } else if (y < w->knoby1) {
                w->state=1;
                scrollTextUp(hw,par,1);
            }
        } else {
            {
                if (y >= w->knoby2) {
                    w->pos+=0.02;
                    w->state=2;
                } else if (y < w->knoby1) {
                    w->pos-=0.02;
                    w->state=1;
                }
            } //else if(hw->dragging) {w->pos=(float)(y-w->y1)/(float)height;w->state=0;} }
            refreshWidget(w);
            safeCallback(w,w->argptr);
        }
        break;
    case HSCROLL:
        par=w->parentbox;
        if(par!=NULL) {
            if(x > w->knobx2) {
                w->state=2;
                scrollTextRight(hw,par,SCROLLSPEEDX);
            } else if (x<w->knobx1) {
                w->state=1;
                scrollTextLeft(hw,par,SCROLLSPEEDX);
            }
        } else {
            {
                if (x >= w->knobx2) {
                    if(w->pos+w->ratio<1.0) {
                        w->pos+=0.02;
                        w->state=2;
                    }
                } else if(x < w->knobx1) {
                    if(w->pos>0) {
                        w->pos-=0.02;
                        w->state=1;
                    }
                }
            }
            refreshWidget(w);
            safeCallback(w,w->argptr);
        }
        break;
#endif
#ifdef HGUI_PICTURES
    case PICTURE:
        if(hw->dragging)break;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case XPM:
        if(hw->dragging)break;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
    default:
        return False;
    }
    return True;
}

Bool midButtonAction(hWindow *hw, Widget *w)
{
    switch (w->type) {
#ifdef HGUI_BUTTONS
    case BUTTON:
        w->state^=1;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case BUTTONXPM:
        w->state^=1;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_PICTURES
    case PICTURE:
        safeCallback(w,w->argptr);
        break;
    case XPM:
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
    default:
        return False;
    }
    return True;
}

Bool rightButtonAction(hWindow *hw, Widget *w, int x, int y)
{
    int width=w->actx2-w->actx1, height=w->acty2-w->acty1;
    switch (w->type) {
#ifdef HGUI_BUTTONS
    case BUTTON:
        if(hw->dragging)break;
        w->state^=1;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case BUTTONXPM:
        if(hw->dragging)break;
        w->state^=1;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_SLIDERS
    case VSLIDER:
        w->pos = 1.0 - (float)(y-(w->acty1+w->margin)) / (float)(height-w->margin*2);
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case HSLIDER:
        w->pos = 1.0 - (float)(x-(w->actx1+w->margin)) / (float)(width-w->margin*2);
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_PICTURES
    case PICTURE:
        if(hw->dragging)break;
        safeCallback(w,w->argptr);
        break;
    case XPM:
        if(hw->dragging)break;
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
    default:
        return False;
    }
    return True;
}

Bool wheelUpAction(hWindow *hw, Widget *w)
{
    Widget *par; //parent widget pointer for some widgets
    switch (w->type) {
#ifdef HGUI_SLIDERS
    case VSLIDER:
        w->pos+=w->ratio;
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case HSLIDER:
        w->pos+=w->ratio;
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_TEXTBOXES
    case TEXTBOX:
        scrollTextUp(hw,w,SCROLLSPEEDY);
        break;
#endif
#ifdef HGUI_PROGBARS
    case HPROGBAR:
    case VPROGBAR:
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_SCROLLBARS
    case VSCROLL:
        w->state=0;
        par=w->parentbox;
        if (par!=NULL) scrollTextUp(hw,par,SCROLLSPEEDY);
        else {
            w->pos-=0.02;
            if(w->pos<0.0)w->pos=0.0;
            refreshWidget(w);
            safeCallback(w,w->argptr);
        }
        break;
    case HSCROLL:
        w->state=0;
        par=w->parentbox;
        if (par!=NULL) scrollTextLeft(hw,par,SCROLLSPEEDX);
        else {
            w->pos-=0.02;
            if(w->pos<0.0)w->pos=0.0;
            refreshWidget(w);
            safeCallback(w,w->argptr);
        }
        break;
#endif
    default:
        return False;
    }
    return True;
}

Bool wheelDnAction(hWindow *hw, Widget *w)
{
    Widget *par; //parent widget pointer for some widgets
    switch (w->type) {
#ifdef HGUI_SLIDERS
    case VSLIDER:
        w->pos-=w->ratio;
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
    case HSLIDER:
        w->pos-=w->ratio;
        correctRange(&w->pos);
        refreshWidget(w);
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_TEXTBOXES
    case TEXTBOX:
        scrollTextDown(hw,w,SCROLLSPEEDY);
        break;
#endif
#ifdef HGUI_PROGBARS
    case HPROGBAR:
    case VPROGBAR:
        safeCallback(w,w->argptr);
        break;
#endif
#ifdef HGUI_SCROLLBARS
    case VSCROLL:
        w->state=0;
        par=w->parentbox;
        if (par!=NULL) scrollTextDown(hw,par,SCROLLSPEEDY);
        else {
            w->pos+=0.02;
            if(w->pos>1.0-w->ratio)w->pos=1.0-w->ratio;
            refreshWidget(w);
            safeCallback(w,w->argptr);
        }
        break;
    case HSCROLL:
        w->state=0;
        par=w->parentbox;
        if (par!=NULL) scrollTextRight(hw,par,SCROLLSPEEDX);
        else {
            w->pos+=0.02;
            if(w->pos>1.0-w->ratio)w->pos=1.0-w->ratio;
            refreshWidget(w);
            safeCallback(w,w->argptr);
        }
        break;
#endif
    default:
        return False;
    }
    return True;
}

void eventLoop (hWindow *hw, void (*callback)(), void* argptr ) //a custom function can be injected into event loop. if NULL, nothing is run
{
    hWindow *ewin,*cwin;
    XEvent event;
    int i,x,y,x2,y2; //struct timeval timeout; fd_set rset; int selRes;
    hw->exitsig=0;
    drawAllWidgets(hw);
#ifdef HGUI_CHILDWINDOWS
    for(i=0; i<hw->ChildAmount; i++) {
        cwin=hw->Child[i];    //XFlush(hw->d);  //XSync(hw->d,False);
        if(cwin==NULL) continue;
        cwin->exitsig=0;
        drawAllWidgets(cwin);
    }
#endif

    while(hw->exitsig==0) {
        //FD_ZERO(&rset); FD_SET(hw->fd,&rset); timeout.tv_sec=REFRESHDELAY/1000000; timeout.tv_usec=REFRESHDELAY;
        //selRes=select(hw->fd+1,&rset,0,0,&timeout); //memset(&timeout.tv_usec,0,sizeof(timeout));
        //if ( XPending(hw->d) || FD_ISSET( ConnectionNumber(hw->d), &rset ) )
        {
            //if(XPending(hw->d) || selRes>0)
            {
                hw->exposecnt=hw->resizecnt=0;
#ifdef HGUI_CHILDWINDOWS
                for(i=0; i<hw->ChildAmount; i++) {
                    cwin=hw->Child[i];
                    if(cwin==NULL) continue;
                    cwin->exposecnt=cwin->resizecnt=0;
                }
#endif
                while( XPending(hw->d) ) { // XEventsQueued(hw->d,QueuedAfterFlush) )
                    XNextEvent(hw->d,&event);
                    ewin=hw; //while (XCheckIfEvent(hw->d,&event,alwaysTrue,0)) //( XCheckWindowEvent(hw->d,hw->w,ExposureMask|KeyPressMask|ButtonPressMask,&event) ) //XNextEvent(hw->d,&event);
#ifdef HGUI_CHILDWINDOWS
                    if (event.xany.window==hw->w) ewin=hw;
                    else {
                        ewin=NULL;
                        for(i=0; i<hw->ChildAmount; i++) {
                            cwin=hw->Child[i];
                            if(cwin==NULL) continue;  //find if childwindow matches the currently processedevent
                            if (event.xany.window==cwin->w) {
                                ewin=cwin;
                                break;
                            }
                        }
                        if (ewin==NULL) continue;
                    }
#endif

                    switch (event.type) {
                    case ButtonPress:
                    case MotionNotify:
                        ewin->eventbutton=event.xbutton.button;
                        ewin->dragging=0;
                        ewin->eventx=x=event.xbutton.x;
                        ewin->eventy=y=event.xbutton.y;
                        if(event.type=MotionNotify) {
                            if (event.xmotion.state&Button1Mask) {
                                ewin->eventbutton=LEFTBUTTON;
                                ewin->dragging=1;
                            } else if(event.xmotion.state&Button3Mask) {
                                ewin->eventbutton=RIGHTBUTTON;    //dragging for sliders/scrollbars/etc.
                                ewin->dragging=1;
                            }
                        }
                        switch (ewin->eventbutton) {
                        case LEFTBUTTON:
                            for(i=ewin->WidgetAmount-1; i>=0; i--) {
                                if(inActRange(ewin,i,x,y)) {
                                    if(leftButtonAction(ewin,&ewin->W[i],x,y))break;
                                }
                            }
                            break;
                        case MIDBUTTON:
                            for(i=ewin->WidgetAmount-1; i>=0; i--) {
                                if(inActRange(ewin,i,x,y)) {
                                    if(midButtonAction(ewin,&ewin->W[i]))break;
                                }
                            }
                            break;
                        case RIGHTBUTTON:
                            for(i=ewin->WidgetAmount-1; i>=0; i--) {
                                if(inActRange(ewin,i,x,y)) {
                                    if(rightButtonAction(ewin,&ewin->W[i],x,y))break;
                                }
                            }
                            break;
                        case SCROLLUP:
                            for(i=ewin->WidgetAmount-1; i>=0; i--) {
                                if(inActRange(ewin,i,x,y)) {
                                    if(wheelUpAction(ewin,&ewin->W[i]))break;
                                }
                            }
                            break;
                        case SCROLLDOWN:
                            for(i=ewin->WidgetAmount-1; i>=0; i--) {
                                if(inActRange(ewin,i,x,y)) {
                                    if(wheelDnAction(ewin,&ewin->W[i]))break;
                                }
                            }
                            break;
                        }
                        break;
                    case Expose:
                        ewin->exposecnt++;
                        break; //if (event.xexpose.count==0) //full/partial refresh might be needed when moving this / other windows above
                    case ConfigureNotify:
                        if(event.xconfigure.width!=ewin->width || event.xconfigure.height!=ewin->height) { //if ((event.xconfigure.x!=ewin->x || event.xconfigure.y!=ewin->y)) break;
                            ewin->resizecnt++;    //ewin->x=event.xconfigure.x; ewin->y=event.xconfigure.y;
                            ewin->width=event.xconfigure.width;
                            ewin->height=event.xconfigure.height;
                        }
                        break; //resizing //printf ("%d,%d, %d,%d \n",event.xconfigure.width,event.xconfigure.height,ewin->width,ewin->height);
                    case KeyPress:
                        switch (XLookupKeysym(&event.xkey,0)) {
                        case XK_Escape:
                            ewin->exitsig=1;
                            break;
                            //case XK_space: setIconXPM(ewin->d,ewin->w,ewin->s,icon_xpm); setTitle(ewin->d,ewin->w,"space nmély-szőrke pressed"); break;
                        }
                        break;
                    case ClientMessage:
                        if (event.xclient.message_type==ewin->wm_protocols && event.xclient.data.l[0]==ewin->delete_window) ewin->exitsig=1;
                        break;
                    default:
                        break;  //keeping convention
                    }

                }

                if (hw->exitsig) {
                    if (hw->exitCallback!=NULL) {
                        (hw->exitCallback)(hw);
                        hw->exitsig=0;
                    }
                }
#ifdef HGUI_CHILDWINDOWS
                for(i=0; i<hw->ChildAmount; i++) {
                    cwin=hw->Child[i];
                    if(cwin==NULL) continue; //check if window is destroyed/uninitialized - array-size (ChildAmout) might change dynamically in the future
                    if(cwin->exitsig) {
                        if(cwin->exitCallback!=NULL) (cwin->exitCallback)(cwin);
                        else hideWindow(cwin);
                    }
                } //destroyWindow(cwin); }  }
#endif

                if (hw->resizecnt) {
                    redrawBackground(hw);    //hw->width=getWinWidth(hw); hw->height=getWinHeight(hw); //redrawBackground(ewin,&event.xexpose); //drawBackground(ewin); // XFlush(ewin->d); //redrawing the whole background makes flickering. better if window is sent to X all-at-once as a pixmap
                }
                if (hw->exposecnt || hw->resizecnt) drawAllWidgets(hw);
#ifdef HGUI_ANIMATIONS
                playAnimations(hw); //if(!hw->exposecnt) //if (selRes==0)
#endif //HGUI ANIMATIONS

#ifdef HGUI_CHILDWINDOWS
                for(i=0; i<hw->ChildAmount; i++) {
                    cwin=hw->Child[i];
                    if(cwin==NULL) continue; //check if window is destroyed/uninitialized
                    if (cwin->resizecnt) {
                        redrawBackground(cwin);    //cwin->width=getWinWidth(cwin); cwin->height=getWinHeight(cwin);
                    }
                    if (cwin->exposecnt || cwin->resizecnt) drawAllWidgets(cwin); //drawWidgets(ewin,&event.xexpose);
#ifdef HGUI_ANIMATIONS
                    playAnimations(hw->Child[i]);
#endif //HGUI_ANIMATIONS
                }
#endif //HGUI_CHILDWINDOWS

                if(callback!=NULL) callback(hw,argptr); //custom loop-callback for main program. if NULL, nothing happens here

                usleep(REFRESHDELAY);
            }
            //else if(selRes==-1 && errno!=EINTR){printf("%s select error\n",ErrPfx);exit(EXIT_ERROR);}
        }
    }
}




//----------------------------------------------------------------------
#if defined(__cplusplus)
}
#endif
//=====================================================================================================
