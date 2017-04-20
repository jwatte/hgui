//======================================================================
//simple hgui screensaver demonstration
//=====================================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <hgui.h>

const char* hacknames[] = { "blank","ellipsoid","lissajous","cube","starfield" , NULL };

hWindow *hw; //hacks' global variables can be found in their section (sorry,fast&dirty)
int width,height;
#define REFRESHRATE 100
#define TIMEOUT 1280; //60*REFRESHRATE //hgui refresh-rate is 100Hz -> 1 minute equals to 100*60=6000 frames
int changecnt=TIMEOUT;

void callhack (hWindow *hw, unsigned char *hacknum); //function-prototype of handler loop-callback

// ============================ MAIN ROUTINE ===========================
int main (int argc, char **argv)
{
    unsigned char hacknum=0;
    int i; //default is blanking
    if (argc==2) {
        if (!strcmp(argv[1],"-help")) {
            printf("\nHermitGUI screensaver example. Usage:  screensav [hackname]\n"
                   "If hackname is not given, it's selected randomly, and\n"
                   "hacks are randomly changed automatically in every minutes.\nhacknames: ");
            for(i=0; hacknames[i]!=NULL; i++) printf("%s ",hacknames[i]);
            printf("\n\n");
            exit(0);
        } else {
            for(i=0; hacknames[i]!=NULL; i++) if (!strcmp(argv[1],hacknames[i])) {
                    hacknum=i;
                    break;
                }
        }
    } else hacknum=1; //rand()*16;
    printf("Hgui screensaver-example. (Type 'screensav -help' for info.)\n");
    printf("Running hack %d: '%s'\n",hacknum,hacknames[hacknum]);

    width=getScreenWidth();
    height=getScreenHeight();
    hw=createWindow(0,0,width,height,"hgui screensaver example",0,0x000000,0,0,FULLSCREEN,NULL,NULL);
    setClass(hw,"screensaver","hguiscreensav");
    hideMouseCursor(hw);
    eventLoop(hw,callhack,&hacknum);
    destroyWindow(hw);
    return 0;
}

// ============================= HACKS =================================
#define PI 3.14
int movcnt=0,stripcnt=0;
float poscnt=0.0;
unsigned long anicol=0x405070;
char phase=0;
float angle=0.0,anglecnt=0.0;
float rad=PI/180.0;

void blank() { }

void ellipsoid()
{
//if (!changecnt) anicol=0xffffff;
    drawEllipse(hw,width/2,height/2,movcnt,height/2-movcnt/2,phase?0:anicol);
    if(movcnt<getWinWidth(hw)/2) {
        movcnt++;
        if(!phase && movcnt%50==0) anicol+=0x104A52;
    } else {
        movcnt=0;
        phase^=1;
    }
}

void lissajous()
{
    anicol=0xffffff;
    for (phase=0; phase<2; phase++) {
        for (angle=0.0; angle<360.0; angle+=1.0)
            drawPoint(hw,width/2+sin(rad*angle*4+56+anglecnt*rad)*width*0.46,height/2+cos(rad*angle*7-22+anglecnt*3*rad)*height*0.48*sin(angle*rad),phase?anicol:0);
        if(!phase) anglecnt+=0.1;
    }
}

#define sf 1.0
int x[8]= {-sf,-sf,-sf,-sf, sf, sf, sf, sf};
int y[8]= {-sf,-sf, sf, sf,-sf,-sf, sf, sf};
int z[8]= {-sf, sf,-sf, sf,-sf, sf,-sf, sf};
void cube()
{
    int i,j;
    anicol+=0x010010;
    float focus=10.0, posx,posy,posz,sizex,sizey;
    for (phase=0; phase<2; phase++) {
        posz=10+sin(anglecnt*rad)*5;
        posx=sin(anglecnt*3*rad)*width/3;
        posy=cos(anglecnt*5*rad)*height/3;
        sizex=width/20;
        sizey=height/20;
        for(i=0; i<8; i++) {
            for(j=0; j<8; j++) {
                drawLine(hw, width/2+(posx+x[i]*sizex)/((z[i]+posz)/focus), height/2+(posy+y[i]*sizey)/((z[i]+posz)/focus),
                         width/2+(posx+x[j]*sizex)/((z[j]+posz)/focus), height/2+(posy+y[j]*sizey)/((z[j]+posz)/focus), phase?anicol:0);
            }
        }
        if(!phase) anglecnt+=0.3;
    }
}

void starfield()
{
    for (phase=0; phase<2; phase++) {
        for(poscnt=6; poscnt<width/2; poscnt+=20) {
            anicol=phase?0xffffff:0;
            drawPoint(hw,width/2+poscnt*sin(poscnt+anglecnt),height/2+poscnt,anicol);
            drawPoint(hw,width/2+poscnt*sin(poscnt+anglecnt),height/2-poscnt,anicol);
            drawPoint(hw,width/2-poscnt,height/2+poscnt*cos(poscnt+anglecnt),anicol);
            drawPoint(hw,width/2+poscnt,height/2+poscnt*cos(poscnt+anglecnt),anicol);
        }
        if(!phase) anglecnt+=0.01;
    }
}


// ======================== HACK-CALLER & HANDLER ======================
const void* hackfunc[]= {blank,ellipsoid,lissajous,cube,starfield,NULL}; //function-pointers of hacks above

void changeHack(char *hacknum)
{
    clearWindow(hw);
    *hacknum=*hacknum+1;
    if(hackfunc[*hacknum]==NULL) *hacknum=1; //start over
//width=getWinWidth(hw); height=getWinHeight(hw);
    movcnt=poscnt=stripcnt=phase=0;
    angle=anglecnt=0.0;
}

void callhack (hWindow *hw, unsigned char *hacknum)
{
    if(changecnt<=0) {
        changecnt=TIMEOUT;
        changeHack(hacknum);
    } else changecnt--;

    void (*hack)() = hackfunc[*hacknum];
    hack();
}


//======================================================================
