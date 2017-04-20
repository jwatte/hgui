
//======================================================================
//HermitGUI header-file to include in programs (or use as documentation)
//======================================================================
#ifndef HGUI_HEADER
#define HGUI_HEADER //used  to prevent double inclusion of this header-file
#if defined(__cplusplus)
extern "C"
{
#endif


//------------------------ defines and enumerations --------------------
#ifndef NULL
#define NULL 0 //in case main program doesn't want to include stdlib
#endif

//define which widgets to include (reduce code size even more):  (unfortunately enum can't be used for this, as it contains 'signed int' constants )
#ifndef HGUI_SELECTWIDGETS //if this is defined for gcc (-D option), the separate needed widgets can be added one-by-one in gcc-commandlin / makefile
#define HGUI_PRIMITIVES    //plot/line/rectangle/circle/etc.
#define HGUI_BUTTONS       //common group of all buttons
#define HGUI_BLOCKS        //rectangle-decorations to group widgets together
#define HGUI_SLIDERS       //adjustable sliders with knobs
#define HGUI_SCROLLBARS    //(depends on HGUI_BLOCKS)
#define HGUI_PROGBARS      //progress bars
#define HGUI_TEXTFIELDS    //common group of all widgets to display/edit text
#define HGUI_PICTURES      //adding pictures (plain pixdata or XPM-image) to the GUI
#define HGUI_ANIMATIONS    //common group for animations
#define HGUI_CHILDWINDOWS  //child- and subwindow support
//#define HGUI_FILEBROWSER   //composite widget
//#define HGUI_COLORCHOOSER  //composite widget
#endif //HGUI_SELECTWIDGETS (#elif defined()...)
//handle common groups - the groupnames define the internal names all at once
#ifdef HGUI_BUTTONS       //involve group-members automatically for BUTTONS
#define HGUI_TEXTBUTTONS   //push-buttons with text in the middle
#define HGUI_PICBUTTONS    //buttons with pictures in the middle (depends on HGUI_BUTTONS and HGUI_PICTURES)
#endif
#ifdef HGUI_TEXTFIELDS    //involve group-members automatically for TEXTFIELDS
#define HGUI_TEXTLINES     //simple/centered one-line texts
#define HGUI_TEXTBLOCKS    //multiline text with wrapping capabilities
#define HGUI_TEXTBOXES     //multiline text(block) with scrolling, selection & editing capabilities
//#define HGUI_TEXTEDITORS //TEXTBOXES with line(inputfield) / block editing capability
//#define HGUI_LISTS       //TEXTBOXES without wrapping, with row-selection capability
//#define HGUI_MENUS       //BUTTON+TEXTBOX with temporary pulldown
//#define HGUI_TABS        //BUTTON + widget-array start/end positions
#endif
#ifdef HGUI_ANIMATIONS    //involve group-members automatically for ANIMATIONS
#define HGUI_PICANIMS      //playing successive (XPM) pictures repeatedly by a given speed
#define HGUI_TEXTANIMS     //scrolling text, etc.
#endif

//window options for createWindow() / createChildWindow() functions (they can be OR-ed if it makes sense)
#define NORESIZE    1  //window can't be maximized or resized by grabbing its endges (and maximize button shouldn't appear in titlebar)
#define NOTASKBAR   2  //window icon & title shouldn't appear in taskbar, window shouldn't be minimized/iconified to taskbar anytime (and minimize button shouldn't appear in titlebar)
#define ALWAYSONTOP 4  //should be above all other windows until closed/hidden
#define NODECOR     8  //no titlebar and border - window-moving/resizing/etc. should be handled from the main program (_WM_MOTIF_HINTS)
#define MODALWIN   16  //for dialog-boxes, appears in front of parent-window and blocks access to parent window until modal window is closed
#define TRAYAPP    32  //window should go into the system-tray/dock
#define FULLSCREEN 64  //occupy tho whole screen including taskbar (useful for e.g. screensaver, maybe combined with NOWINMAN & ALWAYSONTOP to have screen-locking?) 
#define ZOOMABLE  128  //tells createWindow() if widgets should change size automatically to fit window-size. This clashes with NORESIZE option.
#define NOWINMAN  256  //can be an option for window ignored by Window-manager: no titlebar/border, no taksbar-icon, stays on top of other windows (splash screen behaviour)


enum MouseButtons { NOBUTTON,LEFTBUTTON,MIDBUTTON,RIGHTBUTTON,SCROLLUP,SCROLLDOWN } MouseButtons;




//---- Screen & Window & Widget Structures and function-prototypes -----
int   getScreenWidth  (); //in pixels
int   getScreenHeight (); //in pixels


typedef struct hWindow hWindow; //forward declaration, defined later in hgui.c

hWindow* createWindow( int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol,
                       int minwidth, int minheight, short options, char* xpmicon[], void (*exitCallback)() );
#ifdef HGUI_CHILDWINDOWS
hWindow* createChildWindow( hWindow *root, int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol,
                            int minwidth, int minheight, short options, char* xpmicon[], void (*exitCallback)() );
hWindow* createSubWindow( hWindow *parenthw, int x1, int y1, int x2, int y2, char *title, unsigned long bgcol, unsigned long fgcol);
#endif
//(subwindow is inner window, its coordinates can be negative and those are counted from parent-window's right/bottom-edge at creation/startup)

hWindow* destroyWindow (hWindow *hw);

void  eventLoop       (hWindow *hw, void (*callback)(), void* argptr );
//void loopCallback (hWindow *hw, void* argptr) {} //the format for main-loop callback function (both parameters can be omitted if not needed)
//void exitCallback (hWindow *hw) {} //the format fo exit-callback function

char  eventButton     (hWindow *hw); //(mouse) button of latest event, see MouseButtons enumeration for values
int   eventX          (hWindow *hw); //mouse X coordinate of latest mouse click/scroll event (relative to window)
int   eventY          (hWindow *hw); //mouse Y coordinate of latest mouse click/scroll event (relative to window)

int   getWinWidth     (hWindow *hw); //in pixels, not including window-decoration
int   getWinHeight    (hWindow *hw); //in pixels, not including window-decoration
int   getWinX         (hWindow *hw); //in pixels from screen origin
int   getWinY         (hWindow *hw); //in pixels from screen origin
int   getWidgetCount  (hWindow *hw); //current number of added widgets in the window
float getContrast     (hWindow *hw);
char* getTitle        (hWindow *hw); //return window-title

void  setContrast     (hWindow *hw, float contrast); //acceptable values between: 0.0..1.0 (if value is outside range, gets truncated)
void  setTitle        (hWindow *hw, char *title);
void  setIconXPM      (hWindow *hw, char *xpm[]); //only XPM with max. 64 colours is supported
void  setClass        (hWindow *hw, char *class, char *name); //'name' here is not titlebar (WM_NAME) but identification for the X-server
void  setWinCallback  (hWindow *hw, void (*callback)() ); //set window-callback afterwards

void  iconifyWindow   (hWindow *hw);
void  hideWindow      (hWindow *hw);
void  showWindow      (hWindow *hw);
void  refreshWindow   (hWindow *hw);
void  redrawWindow    (hWindow *hw);
void  clearWindow     (hWindow *hw);
void  moveWindow      (hWindow *hw, int x, int y);
void  resizeWindow    (hWindow *hw, int w, int h);
void  toggleFullscreen(hWindow *hw);
void  hideMouseCursor (hWindow *hw);

typedef struct hWidget { //universal widget structure - full version is defined later in hgui.c, their structure should match exactly to the specific widgets below
    union {
        int x,x1;
    } ;
    union {
        int y,y1;
    } ;
    union {
        int x2,w,r,rx;
    } ;
    union {
        int y2,h,ry;
    } ;
    union {
        int hpos,border,knobsize,depth,startdeg;
    } ;
    union {
        long vpos,bevel,scalewidth,degree;
    } ;
    union {
        unsigned long bgcol,color,transpcol;
    } ;
    union {
        unsigned long fgcol,textcol;
    } ;
    union {
        unsigned long knobcol,progtextcol;
    } ;
    unsigned long captcol;
    union {
        void *data,*text,*content,*title,*caption,*pixdata,*xpm,*xpmlist;
    } ;
    float pos ;
    union {
        float ratio,scale;
    } ;
    union {
        char wrap,framedelay;
    } ;
    char scrollbars ;
    union {
        char pixstep,autofit;
    } ;
    char state;
    int actx1,acty1,actx2,acty2; //read-only (writing could fuck up the GUI
} hWidget;

hWindow* getWin        (void *widgetptr); //find hgui-window for the given widget
void     redrawWidget  (void *widgetptr); //involves dynamic coordinates & zooming according to current window-size and clearing movement-trails, and flushing to X-server
void     changeTextPtr (void *widgetptr,char* textptr); //caption/text-content POINTER change.. be careful not to give literal strings in functions as they will be destroyed eventually
void     setCallback   (void *widgetptr, void (*callback)(), void* argptr); //set the callback function an arg/parameter afterwards (e.g. when cross-linking widgets or when widget-adding order requires it)
//void callback (hWidget *widgetptr, void* argptr) {}  //the full format for widget-callback-functions (both parameters can be omitted if not needed)

//these distinct widget-structures are not totally necessary (but strongly advised), the universal 'hWidget' structure can be used in their place:
// Widgetname    //signatures and members/fields to read/set for complex operations:
#ifdef HGUI_TEXTBUTTONS
typedef struct Button {
    int x,y,w,h;
    int border;
    long bevel;
    unsigned long color,textcol,nop1,nop2;
    char* title;
    float nop3,nop4;
    char state;
} Button; // if bevel>1: 3D-effect
Button*         addButton       ( hWindow *hw, int x, int y, int w, int h, char *title, unsigned long bgcol, unsigned long fgcol,
                                  char state, char bevel, char border, char *font, void (*callback)(), void* argptr );

typedef struct ButtonDyn {
    int x1,y1,x2,y2;
    int border;
    long bevel;
    unsigned long color,textcol,nop1,nop2;
    char* title;
    float nop3,nop4;
    char state;
} ButtonDyn; // if bevel>1: 3D-effect
ButtonDyn*      addButtonDyn    ( hWindow *hw, int x1, int y1, int x2, int y2, char *title, unsigned long bgcol, unsigned long fgcol,
                                  char state, char bevel, char border, char *font, void (*callback)(), void* argptr );
#endif

#ifdef HGUI_PICBUTTONS
typedef struct  Widget ButtonXPM;   // x,y,w,h, *pixdata, bgcol, state (0/1), bevel(if>1:3D-effect),border, *callback, (readonly: actx1,acty1,acty1,acty2, id)
ButtonXPM*      addButtonXPM    ( hWindow *hw, int x, int y, int w, int h, char *xpm[], unsigned long bgcol,
                                  char state, char bevel, char border, void (*callback)(), void* argptr );
#endif

#ifdef HGUI_SLIDERS
typedef struct  Widget VSlider;     // x,y1,w,y2, knobsize,scalewidth, *caption, bgcol,fgcol,knobcol,captcol, pos(0.0..1.0),scale(0.0..1.0), *font, *callback, (readonly: actx1,acty1,acty1,acty2,margin, id)
VSlider*        addVSlider      ( hWindow *hw, int x, int y1, int w, int y2, char knobsize, char scalewidth, char *caption,
                                  unsigned long bgcol, unsigned long fgcol, unsigned long knobcol, unsigned long captcol,
                                  float pos, float scale, char *font, void (*callback)(), void* argptr );
typedef struct  Widget HSlider;     // x1,y,x2,h, knobsize,scalewidth, *caption, bgcol,fgcol,knobcol,captcol, pos(0.0..1.0),scale(0.0..1.0), *font, *callback, (readonly: actx1,acty1,acty1,acty2,margin, id)
HSlider*        addHSlider      ( hWindow *hw, int x1, int y, int x2, int h, char knobsize, char scalewidth, char *caption,
                                  unsigned long bgcol, unsigned long fgcol, unsigned long knobcol,
                                  unsigned long captcol, float pos, float scale, char *font, void (*callback)(), void* argptr );
#endif

#ifdef HGUI_TEXTLINES
typedef struct  Widget TextLine;    //            x,     y,      *text,               textcol,    *font, (readonly: id)
TextLine*       addTextLine     ( hWindow *hw, int x, int y, char *text, unsigned long color, char *font);
typedef struct  Widget TextCentered;// x1,y1,x2,y2, *text, textcol, *font, (readonly: id)
TextCentered*   addTextCentered ( hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long color, char *font );
#endif
#ifdef HGUI_TEXTBLOCKS
typedef struct  Widget TextBlock;   // x1,y1,x2,y2, *text, textcol, *font, wrap(0/1), (readonly: id)
TextBlock*      addTextBlock    ( hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long color, char wrap, char *font );
#endif
#ifdef HGUI_TEXTBOXES
typedef struct  Widget TextBox;     // x1,y1,x2,y2, *text, bgcol,fgcol, wrap(0/1), scrollbars(1/2/1+2), autofit(0/1), vpos,hpos,vpixpos,vpixsize, *font, actx1,acty1,acty1,acty2,hpixsize, id,  (readonly: *vscroll,*hscroll (widget-pointers to children-scrollbars of textbox))
TextBox*        addTextBox      ( hWindow *hw, int x1, int y1, int x2, int y2, char *content, unsigned long bgcol, unsigned long fgcol,
                                  char wrap, char scrollbars, char autofit, long vpos, int hpos, char *font );
#endif

#ifdef HGUI_PROGBARS
typedef struct  Widget HProgBar;    // x1,y1,x2,y2, bevel, *caption, bgcol,fgcol,progtextcol,captcol, pos(0.0..1.0),scale(0.0..1.0), *font, *callback, (readonly: actx1,acty1,acty1,acty2, id)
HProgBar*       addHProgBar     ( hWindow *hw, int x1, int y1, int x2, int y2, char bevel, char *caption, unsigned long bgcol, unsigned long fgcol,
                                  unsigned long textcol, unsigned long captcol, float pos, float scale, char* font, void (*callback)(), void* argptr );
typedef struct  Widget VProgBar;    // x1,y1,x2,y2, bevel, *caption, bgcol,fgcol,progtextcol,captcol, pos(0.0..1.0),scale(0.0..1.0), *font, *callback, (readonly: actx1,acty1,acty1,acty2, id)
VProgBar*       addVProgBar     ( hWindow *hw, int x1, int y1, int x2, int y2, char bevel, char *caption, unsigned long bgcol, unsigned long fgcol,
                                  unsigned long textcol, unsigned long captcol, float pos, float scale, char* font, void (*callback)(), void* argptr );
#endif

#ifdef HGUI_SCROLLBARS
typedef struct  Widget VScroll;     // x,y1,w,y2, bgcol,fgcol,knobcol, pos(0.0..1.0),ratio(0.0..1.0), *callback, (readonly: actx1,acty1,acty1,acty2,margin, id, *parentbox)
VScroll*        addVScroll      ( hWindow *hw, int x, int y1, int w, int y2, unsigned long bgcol, unsigned long fgcol,
                                  unsigned long knobcol, float pos, float ratio, void (*callback)(), void* argptr );
typedef struct  Widget HScroll;     // x1,y,x2,h, bgcol,fgcol,knobcol, pos(0.0..1.0),ratio(0.0..1.0), *callback,  (readonly: actx1,acty1,acty1,acty2,margin, id, *parentbox)
HScroll*        addHScroll      ( hWindow *hw, int x1, int y, int x2, int h, unsigned long bgcol, unsigned long fgcol,
                                  unsigned long knobcol, float pos, float ratio, void (*callback)(), void* argptr );
#endif

#ifdef HGUI_BLOCKS
typedef struct  Widget Block;       //            x1,     y1,     x2,     y2,    (readonly: id)
Block*          addBlock        ( hWindow *hw, int x1, int y1, int x2, int y2 );
typedef struct  Widget BlockFill;   //            x1,     y1,     x2,     y2,              color,    (readonly: id)
BlockFill*      addBlockFill    ( hWindow *hw, int x1, int y1, int x2, int y2, unsigned int color );
#endif

#ifdef HGUI_PICTURES
typedef struct  Widget Picture;     // x,y, *pixdata, width,height,depth, *callback, (readonly: actx1,acty1,acty1,acty2, id)
Picture*        addPicture      ( hWindow *hw, int x, int y, int width, int height, int depth, char* pixeldata, void (*callback)(), void* argptr );
typedef struct  Widget PictureXPM;  //            x,     y,      *xpm,   transpcol, *callback, (readonly: actx1,acty1,acty1,acty2, id)
PictureXPM*     addXPM          ( hWindow *hw, int x, int y, char *xpm[], unsigned long bgcol, void (*callback)(), void* argptr );
#endif      // XPM:max 64 colours!

#ifdef HGUI_PICANIMS
//typedef struct Widget Animation;
typedef struct  Widget AnimXPM;     // x,y, *xpmlist, transpcol, framedelay, framecnt,spdcnt, *callback, (readonly:actx1,acty1,acty1,acty2, id)
AnimXPM*        addAnimXPM      ( hWindow *hw, int x, int y, void* xpmlist[], unsigned long bgcol, char framedelay, void (*callback)(), void* argptr );
#endif

#ifdef HGUI_TEXTANIMS
typedef struct  Widget TextScroll;  // x1,y1,x2,y2, *text, bgcol, fgcol, framedelay,pixstep *font, framecnt,spdcnt (readonly:actx1,acty1,acty1,acty2, id)
TextScroll*     addTextScroll   ( hWindow *hw, int x1, int y1, int x2, int y2, char *text, unsigned long bgcol, unsigned long fgcol,
                                  char framedelay, char pixstep, char *font );
#endif //HGUI_TEXTANIMS

#ifdef HGUI_PRIMITIVES
typedef struct  Widget Primitive;
Primitive* addPoint  (hWindow *hw, int x, int y, unsigned long col);
Primitive* addLine     (hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col);
Primitive* addRect     (hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col);
Primitive* addRectFill (hWindow *hw, int x1, int y1, int x2, int y2, unsigned long col);
Primitive* addArc      (hWindow *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col);
Primitive* addArcFill  (hWindow *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col);
Primitive* addCircle      (hWindow *hw, int x, int y, int r, unsigned long col);
Primitive* addCircleFill  (hWindow *hw, int x, int y, int r, unsigned long col);
Primitive* addEllipse     (hWindow *hw, int x, int y, int rx, int ry, unsigned long col);
Primitive* addEllipseFill (hWindow *hw, int x, int y, int rx, int ry, unsigned long col);

typedef struct hWindow hDrawable; //should be able to assign pixmap as drawable destination instead of bare X-window
//primitive drawing routines (if put on window directly they get destructed when window gets resized / etc. -> better drawing into a picture/pixmap widget):
void drawPoint  (hDrawable *hw, int x, int y, unsigned long col);
void drawLine     (hDrawable *hw, int x1, int y1, int x2, int y2, unsigned long col);
void drawRect     (hDrawable *hw, int x1, int y1, int x2, int y2, unsigned long col);
void drawRectFill (hDrawable *hw, int x1, int y1, int x2, int y2, unsigned long col);
void drawArc      (hDrawable *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col);
void drawArcFill  (hDrawable *hw, int x1, int y1, int x2, int y2, int startdeg, int degree, unsigned long col);
void drawCircle      (hDrawable *hw, int x, int y, int r, unsigned long col);
void drawCircleFill  (hDrawable *hw, int x, int y, int r, unsigned long col);
void drawEllipse     (hDrawable *hw, int x, int y, int rx, int ry, unsigned long col);
void drawEllipseFill (hDrawable *hw, int x, int y, int rx, int ry, unsigned long col);
void drawPolygon (hDrawable *hw, unsigned long col, char count, ...); //count:number of points, parameters following 'count' are X and Y coordinates alternating
#endif




//--------------------- Miscellaneous helper routines ------------------
typedef struct XPMstruct {
    char *pixdata;
    int width, height;
} XPMstruct;

XPMstruct XPMtoPixels(char* source[], unsigned long bgcol); //max. 64-colour XPMs are supported! (one base64 char/colour)

unsigned long fadeCol(unsigned int col, float bri); // 0.0 < bri <= 1.0 enlightens ; -1.0 <= bri < 0.0 darkens color

void correctRange(float *input); //sets input between 0.0 and 1.0




//----------------------------------------------------------------------
#if defined(__cplusplus)
}
#endif
#endif //HGUI_HEADER
//======================================================================
