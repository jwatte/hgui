#include <stdlib.h>
#include <stdio.h>
//#include <math.h>
#include <hgui.h>

#define SBHEIGHT 30
#define MAX_DATA_LEN 32768
#define DATA_HEIGHT 256

int data[MAX_DATA_LEN];
int datalen=0;


void drawData(hWindow *hw, long int pos, int* data, int width)
{
    int i,x=0;
    for (i=0; i<256; i+=16) drawLine(hw,0,DATA_HEIGHT-i,width-1,DATA_HEIGHT-i,i%0x40?0x000080:0x0000FF);
    for (i=0; i<width && data[pos]!=EOF; i++) {
        if ((i+pos)%16==0) {
            drawLine(hw,i,0,i,DATA_HEIGHT-1,(i+pos)%64?0x000080:0x0000FF);
            drawLine(hw,i,DATA_HEIGHT-1,i,DATA_HEIGHT-4,0xFF0020);
        }
        if ((i+pos)%64==0) drawLine(hw,i,DATA_HEIGHT-1,i,DATA_HEIGHT-8,0xFF8080);
        if ((i+pos)%256==0) drawLine(hw,i,DATA_HEIGHT-1,i,DATA_HEIGHT-16,0xFFFFFF);
        drawPoint(hw,i,DATA_HEIGHT-1-data[pos+i],0x80FFFF);
    }
}

void refreshDiagram(hWindow *hw, hWidget *w)
{
//drawRectFill(hw,0,0,getWinWidth(hw),DATA_HEIGHT,0);
//drawData( hw, w->pos*(datalen), data, getWinWidth(hw) );
}

void sbCallback(hWidget *w)
{
    drawRectFill(getWin(w),0,0,getWinWidth(getWin(w)),DATA_HEIGHT,0); //clearWindow(getWin(w));
    drawData( getWin(w), w->pos*(datalen), data, getWinWidth(getWin(w)) );
}

int main (int argc, char **argv)
{
    hWindow *hw;
    int i, width, readata=0;
    FILE *InputFile;

    if (argc<2) {
        printf("Usage: diagram <inputfile>\n");
        return 1;
    }

    InputFile = fopen(argv[1],"rb");
    printf("\n");
    datalen=0;
    do {
        readata=fgetc(InputFile);
        data[datalen++]=readata;
        if(readata!=EOF) printf("%2.2X ",readata);
    } while (readata!=EOF && datalen<MAX_DATA_LEN);
    datalen--;

    printf("\n%d bytes read \n",datalen);
    fclose(InputFile);

    width=getScreenWidth();
    hw=createWindow(0,0,(datalen<width)?datalen:width,DATA_HEIGHT+SBHEIGHT+4,"diagram drawing example",0,0x000000,0,0,0,NULL,NULL);
    drawData(hw,0,data,width);
    HScroll *hscroll = addHScroll(hw,0,DATA_HEIGHT+2,getWinWidth(hw),SBHEIGHT,0x254500,0x335533,0xcccccc,0.0,(float)width/(float)(datalen),sbCallback,NULL);


    eventLoop(hw,refreshDiagram,&hscroll);
    destroyWindow(hw);
    return 0;
}
