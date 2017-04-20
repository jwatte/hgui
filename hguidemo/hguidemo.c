//======================================================================
// demonstration program for Hermit-GUI
//=====================================
#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdlib.h>     //C standard library
#include <stdio.h>      //printf,sprintf,fprintf,etc.
//#include <string.h>     //operations like strlen()

#include <hgui.h>  //subcomponents can be selected with -D options for gcc (see hgui.h for info)

//defining some defaults to use as 'theme', but you can have custom mechanism for styling of course
#define BGCOLOR 0xCEC8CA
#define FGCOLOR 0x001000
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
#define SLIDSCALEW 20
#define SLIDKNOBSIZE 16
#define SLIDBGCOL 0xC0E0D0
#define SLIDFGCOL 0x20C080
#define SLIDKNOBCOL 0xC020E0
#define SCROLLBGCOL 0xC0A080
#define SCROLLFGCOL 0x624050
#define SCROLLKNOBCOL 0xD8D0D4
#define SCROLLBUTTSIZE 14
#define ANIMDELAY 10  //in frames (check hgui.c for REFRESHRATE definition )

#include "icon.xpm"

static char *questionmark_xpm[] = { "16 16 8 1"," 	c None","1	c #DB0000","2	c #FF0000","3	c #B60000","4	c #6D0000","5	c #FFB400","6	c #FF7A00","7	c #920000",
                                    "     12234      ","   155566234    ","  25622226234   ","  662    26234  ","  22      2234  ","          2334  ","         2234   ","       22344    ",
                                    "      2634      ","      637       ","      234       ","      424       ","                ","       22       ","      2624      ","       24       "
                                  };

static char *mouse_xpm[] = {"20 20 3 1 0 0","0	c #000000","1	c #FFFFFF"," 	c None",
                            "00                  ","010                 ","01100               ","011100              ","0111100             ","01110100            ","010111100           ",
                            "0111111100          ","01111111100         ","01111111100         ","01111111100         ","01111111000         ","0011111000          "," 00000100           ",
                            "  000010            ","      010           ","      010           ","       01           ","         1          ","                    ",
                           };

#include "robodance.anim"

typedef struct twoWidgets {
    hWidget *w1,*w2;
} twoWidgets;

char blabla[]="Bla Bla global"; //this global text doesn't get destructed when function exits

void b1callback(hWidget *w, ButtonDyn *butt2)
{
    setTitle(getWin(w),"Button 1 pressed!");
    butt2->state=0;
    butt2->title="b1press";
    butt2->x1+=1;
    butt2->x2+=1;
    redrawWidget(butt2);
}

void b2callback(ButtonDyn *butt2)
{
    setTitle(getWin(butt2),"Button 2 pressed!");
    butt2->title="Na?";
    redrawWidget(butt2);
}

void b3callback(hWidget *w, hWidget *scr1)
{
    setTitle(getWin(w),"Button 3 pressed!");
    scr1->pos=0.2;
    redrawWidget(scr1);
    toggleFullscreen(getWin(w));
}

void sl1callback(hWidget *slider1, char* constr)
{
    hWindow *win=getWin(slider1);
    setTitle(win,"Slider1");
    setContrast(win,slider1->pos);
    sprintf(constr,"c.:%1.1f",getContrast(win));
    changeTextPtr(slider1,constr);
} //slider1->caption=constr; redrawWidget(slider1); }

void hslid1callback(hWidget *sl1, hWidget* prog3)
{
    hWindow *win=getWin(sl1);
    setTitle(win,"horizontal Slider callback");
    changeTextPtr(sl1,blabla); //remember, this is only a POINTER to a text! Don't use literals for changeTextPtr as they could be destroyed after this function exits
    if (eventButton(win)==SCROLLDOWN) {
        prog3->pos-=0.01;
        if(prog3->pos<=0.0) prog3->pos=0.0;
        redrawWidget(prog3);
    } else if (eventButton(win)==SCROLLUP) {
        prog3->pos+=0.1;
        redrawWidget(prog3);
    }
};

void scrcallback(hWidget *w, hWindow *win)
{
    setTitle(win,"Scrolling ás, ő maga ");
}

void piccallback(hWidget *pic, twoWidgets *tw)
{
    hWidget *prog1=tw->w1;
    hWidget *xpb1=tw->w2;
    setTitle(getWin(pic),"Pictureclick");
    prog1->pos+=0.01;
    if(prog1->pos>1.0) prog1->pos=0.05;
    redrawWidget(prog1);
    xpb1->data=mouse_xpm;
    redrawWidget(xpb1);
}

void xpmcallback(PictureXPM *xpm, twoWidgets *t)
{
    hWidget *prog1=t->w1;
    hWidget *xpb1=t->w2;
    prog1->pos-=0.02;
    if(prog1->pos<0) prog1->pos=0.9;
    redrawWidget(prog1);
    xpb1->data=icon_xpm;
    redrawWidget(xpb1);
}

void butxpmcallback(hWidget *w, char* toggle)
{
    setIconXPM(getWin(w),*toggle?questionmark_xpm:mouse_xpm);
    *toggle^=1;
    setTitle(getWin(w),"ProgramIcon changed");
}

void prog2callback(hWidget *pr2, hWindow *win)
{
    if(eventButton(win)==SCROLLUP) pr2->pos+=0.1;
    if(eventButton(win)==SCROLLDOWN) pr2->pos-=0.2;
    if(eventButton(win)==LEFTBUTTON) pr2->pos=(float)(eventX(win)-pr2->actx1)/(float)(pr2->actx2-pr2->actx1);
    correctRange(&pr2->pos);
    redrawWidget(pr2);
}

void prog3callback(hWidget *prog3, hWindow *win)
{
    if(eventButton(win)==SCROLLUP) prog3->pos+=prog3->ratio;
    if(eventButton(win)==SCROLLDOWN) prog3->pos-=prog3->ratio;
    redrawWidget(prog3);
}

void subwbcallback()
{
    printf("Subwindow button pressed\n");
}

void loopcallback(hWindow *hw, char* fullscrswitch)
{
    if(*fullscrswitch==0 && getWinWidth(hw)>720) {
        setTitle(hw,"Bigger than 720p!");
        *fullscrswitch=1;
    }
}

void exitcallback(hWindow *hw)
{
    printf("Exit button on main window pressed: Exiting by this custom callback-function...\n");
    destroyWindow(hw);
    exit(0);
}
void childExit(hWindow *hw)
{
    printf("Exit button on child-window pressed. Killing child-window \"%s\" by custom callback.\n",getTitle(hw));
    destroyWindow(hw);
}


//====================== Main program & entry-point ================================================

int main (int argc, char **argv)    //note: literals (buttun-names,progressbar-captions,etc.) get destroyed when main exits.
{
    int i, GUIwidth=600, GUIheight=400, ChildWidth=300, ChildHeight=200;

    char textample[]="Addig nyújtózkodj, ameddig a takaród ér! \n Aki másnak vermet ás, ő maga esik bele!";
    char scrolltext[]="ő á  I wonder why the grass is green, and why the wind is never seen. Who taught the birds to build a nest, and told the trees to take a rest? " \
                      "O, when the Moon is not quite round, where can the missing bit be found? Who lights the stars when they blow out, and makes the lightning flash about? " \
                      "Who paints the rainbow in the sky, and hangs the fluffy clouds so high? Why is it now, do you suppose, that Dad won't tell me if he knows?  (Connie Amarel)";
    char constr[16]="contr:xx";  //note: this string doesn't get destroyed when the callback functions using it exits, but it IS destroyed upon exiting this main() function
    char defont[]="-*-*vera sans-bold-r-*-*-14-*-*-*-p-*-*-*";

    hWindow *hwin, *childwin=NULL, *subwin;
    ButtonDyn *butt2;
    HProgBar *prog2;
    ButtonXPM *xpb1;
    hWidget *text1,*slider1,*prog1,*prog3,*scr1;
    PictureXPM *xpmw;
    char fullscrswitch=0,toggle;

    printf("\nScreen Resolution: %d x %d\n",getScreenWidth(),getScreenHeight());
    hwin=createWindow( (getScreenWidth()-GUIwidth)/2, (getScreenHeight()-GUIwidth)/2, GUIwidth, GUIheight,
                       "HermitGUI!", BGCOLOR, FGCOLOR, 512, 384, ZOOMABLE, icon_xpm, exitcallback);
//resizeWindow(hwin,GUIwidth-10,GUIheight-5);

    butt2=addButtonDyn(hwin,90,30,140,56,"adgButton",BUTTBGCOL,BUTTFGCOL,1,2,2,"6x13",b2callback,NULL);
    addButton(hwin,20,30,50,26,"addButt",BUTTBGCOL,BUTTFGCOL,0,4,2,"6x13",b1callback,butt2);
    scr1=(hWidget*)addVScroll(hwin,366,20,16,-20,SCROLLBGCOL,SCROLLFGCOL,SCROLLKNOBCOL,0.1,0.3,scrcallback,hwin);
    addButton(hwin,-90,-60,70,26,"ődgBétt2",BGCOLOR,FGCOLOR,0,1,1,"6x13",b3callback,scr1);
    addTextLine(hwin,210,-20,"TextLine\nNoLinefeedSupport",0x305015,defont);
    addTextBlock(hwin,160,30,270,155,textample,0x001000,1,defont);
    addTextBlock(hwin,400,10,-20,-360,textample,0x001000,0,"-*-*vera sans-medium-r-*-*-12-*-*-*-p-*-*-*");
    addBlock(hwin,160-10,30-10,270+10,150+10); //addBlock(400-2,20-2,-20,80);
    addTextBox(hwin,400,100,-20,200,"blaab\noaiaos ő maga  ao\nse hiao \n se tohe\naisoa ith ioód ér! he hae\nsteho a",TEXTBGCOL,TEXTFGCOL,1,0,1,0,0,"fixed");
    addTextBox(hwin,400,210,-20,-70,"gipszjakab oea\niieo aooi ouiola\nmély\nszőr  aoe\noei a a\noei u keo	eoae iio \na\noabsol\noiaoeioiu",0x506020,0xc0e0a0,0,1+2,1,1,4,defont);
    slider1=(hWidget*)addVSlider(hwin,20,100,50,200,SLIDKNOBSIZE,SLIDSCALEW,"Contrast",SLIDBGCOL,SLIDFGCOL,SLIDKNOBCOL,0x0000ff,getContrast(hwin),0.05,defont,sl1callback,constr);
    addVSlider(hwin,90,100,56,-200,48,SLIDSCALEW,"Speaker",0x303030,0x504050,0xC0C0C0,0x003000,0.2,0.04,"fixed",NULL,NULL);
    prog3=(hWidget*)addVProgBar(hwin,65,232,82,-80,PROGBEVEL,"Vert.pr",0x00C040,0xd0F050,0xFFFFFF,0x00c000,0.76,0.05,NULL,prog3callback,hwin);
    addHSlider(hwin,160,179,272,20,10,4,"horizontal slider",fadeCol(BGCOLOR,0.1),0x101010,0xc0c0c0,0x101010,0.3,0.1,"fixed",hslid1callback,prog3);
    prog1=(hWidget*)addHProgBar(hwin,80,-60,200,-20,PROGBEVEL,"Process...",PROGBGCOL,PROGFGCOL,PROGTEXTCOL,0xFF0000,0.63,0.0,"fixed",NULL,NULL);
    addBlock(hwin,5,5,-6,-6); //separator decoration lines between blocks of widgets
    prog2=addHProgBar(hwin,10,-40,72,-20,1,NULL,0x000000,0x00c030,PROGTEXTCOL,0xFF0000,0.83,0.1,"fixed",prog2callback,hwin);
    addVProgBar(hwin,30,232,55,290,PROGBEVEL,"Vert.pr",PROGBGCOL,PROGFGCOL,PROGTEXTCOL,0x00c000,0.76,0.0,defont,NULL,NULL);
    addBlockFill(hwin,250,270,330,-50,0x251506);
    addTextCentered(hwin,250,270,330,-50,"CenText ógé",0xf0e0a0,"fixed");
    addHScroll(hwin,440,-25,-12,16,0x3a6302,0x002000,0x40c020,0.18,0.74,scrcallback,hwin);
    int w=124,h=98;
    char pixeldata[w*h*32/8];
    for(i=0; i<w*h*4; i++) pixeldata[i]=rand()*255;
    addBlock(hwin,100-3,-180-3,100+w+2,-180+h+2);
    xpmw=addXPM(hwin,260,220,icon_xpm,BGCOLOR,NULL,NULL);
    xpb1=addButtonXPM(hwin,290,80,50,30,questionmark_xpm,0x502040,0,4,6,butxpmcallback,&toggle);
    twoWidgets twoW= {prog1,(hWidget*)xpb1};
    addPicture(hwin,100,-180,w,h,32,pixeldata,piccallback,&twoW);
    addButtonXPM(hwin,-125,-60,28,26,questionmark_xpm,BGCOLOR,0,1,1,NULL,NULL);
    void* xpmlist1[]= {a0_xpm,a1_xpm,a2_xpm,a3_xpm,a4_xpm,a5_xpm,a6_xpm,a7_xpm,NULL};
    addAnimXPM(hwin,300,200,xpmlist1,BGCOLOR,ANIMDELAY,NULL,NULL); //0xDEDEDE
    addTextScroll(hwin,220,-43,320,-20,scrolltext,0x0000a0,0xa0e5f0,1,1,defont);
    setCallback(xpmw,xpmcallback,&twoW);

    addPoint(hwin,1,1,0x305020);
    drawLine(hwin,10,10,36,26,0xf00010);
    addLine(hwin,20,10,50,26,0x0000F0);
    addCircle(hwin,320,150,26,0x150260);
    addCircleFill(hwin,320,150,15,FGCOLOR);
    addEllipse(hwin,-90,77,32,16,0x558260);
    addEllipseFill(hwin,-90,77,17,12,0x35A240);
    addRect(hwin,-40,65,-10,83,0);
    addRectFill(hwin,-36,70,-15,79,0xc08090);
    addArc(hwin,-50,-50,-1,-1,270,90,0x0000c0);
    addArcFill(hwin,310,-76,350,-25,300,50,0x80a0c0);
    drawPolygon(hwin,0x40C021,4, 40,10, 65,15, 90,10, 65,25);

    int GuiX=getWinX(hwin), GuiY=getWinY(hwin), ChildX=GuiX+(getWinWidth(hwin)-ChildWidth)/2, ChildY=GuiY+(getWinHeight(hwin)-ChildHeight)/2; //hideWindow(hwin);
    printf("Main-window-X:%d,Main-window-Y:%d,Main-window-Width:%d,Main-Window-Height:%d\n",getWinX(hwin),getWinY(hwin),getWinWidth(hwin),getWinHeight(hwin));
    childwin=createChildWindow( hwin, ChildX, ChildY, ChildWidth, ChildHeight, "hgui child", BGCOLOR, FGCOLOR,
                                ChildWidth/2, ChildHeight/2, NORESIZE|NOTASKBAR|ALWAYSONTOP|MODALWIN, questionmark_xpm, childExit);
    printf("Child-window-X:%d,Child-window-Y:%d,Child-window-Width:%d,Child-Window-Height:%d\n",getWinX(childwin),getWinY(childwin),getWinWidth(childwin),getWinHeight(childwin));
    setContrast(childwin,0.15);
    moveWindow(childwin,ChildX-60,ChildY-40); //iconifyWindow(childwin); //hideWindow(childwin); //showWindow(hwin);
    setClass(hwin,"HermitGUI_demo","hguidemo");
    setClass(childwin,"HermitGUI_demo","hguidemo_childwin");
    addTextBox(childwin,10,10,-80,80,"\niieuiola\nmély\nszőr  aoe\noei a a\noei u keo iio \na\noabsol\n",fadeCol(BGCOLOR,0.2),0x404040,1,1,1,1,4,"fixed");
    addTextScroll(childwin,1,-30,-1,-1,scrolltext,BGCOLOR,0,2,1,"fixed");
    addButton(childwin,100,100,100,60,"ChildButton",BGCOLOR,FGCOLOR,0,1,1,"6x13",b3callback,scr1);

    subwin=createSubWindow(hwin,-70,10,-10,40,"HermitGUI subwindow",fadeCol(BGCOLOR,0.4),FGCOLOR);
    printf("Subwindow-X:%d,Subwindow-Y:%d,Subwindow-Width:%d,Subwindow-Height:%d\n",getWinX(subwin),getWinY(subwin),getWinWidth(subwin),getWinHeight(subwin));
    setContrast(subwin,0.8);
    addButton(subwin,5,5,40,20,"subwin",BGCOLOR,FGCOLOR,0,1,1,"6x13",subwbcallback,NULL);

    printf("Main-window widget-count=%d, Child-window widget-count:%d, Subwindow widget-count:%d \n",getWidgetCount(hwin),getWidgetCount(childwin),getWidgetCount(subwin));

    eventLoop(hwin,loopcallback,&fullscrswitch);

//childwin=destroyWindow(childwin); subwin=destroyWindow(subwin);
    hwin=destroyWindow(hwin);
    return(0);
}

#if defined(__cplusplus)
}
#endif
