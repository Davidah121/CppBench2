#include "IOTools.h"

#ifdef _WIN32
    #include <conio.h>
#endif


bool getCharNoBlock(int& c)
{
    bool ascii = true;
    #ifdef _WIN32
    c = getch();
    
    if(c == 0 || c == 0xE0)
    {
        ascii = false;
        c = getch();
    }

    #endif
    return ascii;
}


void setConsoleColor(int colorCode, int backgroundColorCode)
{
    int actualBackgroundColor = backgroundColorCode+10;
    if(backgroundColorCode == COLOR_CODE_DEFAULT)
        actualBackgroundColor = 0;
    printf("\033[%dm\033[%dm", colorCode, actualBackgroundColor);
}

void clearConsole(bool clearScrollBuffer)
{
    if(clearScrollBuffer)
        printf("\033[2J\033[3J\033[H");
    else
        printf("\033[2J\033[H");
}

void moveConsoleCursor(int horizontal, int vertical, bool absolute)
{
    if(absolute)
    {
        int realHVal = (horizontal>=1) ? horizontal : 1;
        int realVVal = (vertical>=1) ? vertical : 1;
        
        printf("\033[%d;%dH", realVVal, realHVal);
    }
    else
    {
        if(horizontal>0)
            printf("\033[%dC", horizontal);
        else if(horizontal<0)
            printf("\033[%dD", std::abs(horizontal));

        if(vertical>0)
            printf("\033[%dB", vertical);
        else if(vertical<0)
            printf("\033[%dA", std::abs(vertical));
    }
}

void eraseConsoleLine(bool eraseFromCursor)
{
    if(eraseFromCursor)
        printf("\033[K");
    else
        printf("\033[2K\r");
}