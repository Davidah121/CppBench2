#pragma once
#include <iostream>
#include <stdio.h>

static const int SCAN_CODE_F1 = 0x3B;
static const int SCAN_CODE_F2 = 0x3C;
static const int SCAN_CODE_F3 = 0x3D;
static const int SCAN_CODE_F4 = 0x3E;
static const int SCAN_CODE_F5 = 0x3F;
static const int SCAN_CODE_F6 = 0x40;
static const int SCAN_CODE_F7 = 0x41;
static const int SCAN_CODE_F8 = 0x42;
static const int SCAN_CODE_F9 = 0x43;
static const int SCAN_CODE_F10 = 0x44;
static const int SCAN_CODE_F11 = 0x85;
static const int SCAN_CODE_F12 = 0x86;

static const int SCAN_CODE_LEFT = 0x4B;
static const int SCAN_CODE_RIGHT = 0x4D;
static const int SCAN_CODE_UP = 0x48;
static const int SCAN_CODE_DOWN = 0x50;

static const int SCAN_CODE_ESC = 0x1B;

static const int COLOR_CODE_DEFAULT = 0;
static const int COLOR_CODE_BLACK = 30;
static const int COLOR_CODE_DARK_RED = 31;
static const int COLOR_CODE_DARK_GREEN = 32;
static const int COLOR_CODE_DARK_YELLOW = 33;
static const int COLOR_CODE_DARK_BLUE = 34;
static const int COLOR_CODE_DARK_MAGENTA = 35;
static const int COLOR_CODE_DARK_CYAN = 36;
static const int COLOR_CODE_DARK_WHITE = 37;
static const int COLOR_CODE_BRIGHT_BLACK = 90;
static const int COLOR_CODE_BRIGHT_RED = 91;
static const int COLOR_CODE_BRIGHT_GREEN = 92;
static const int COLOR_CODE_BRIGHT_YELLOW = 93;
static const int COLOR_CODE_BRIGHT_BLUE = 94;
static const int COLOR_CODE_BRIGHT_MAGENTA = 95;
static const int COLOR_CODE_BRIGHT_CYAN = 96;
static const int COLOR_CODE_WHITE = 97;

bool getCharNoBlock(int& c);

void setConsoleColor(int colorCode, int backgroundColorCode);

void clearConsole(bool clearScrollBuffer);
void moveConsoleCursor(int horizontal, int vertical, bool absolute);
void eraseConsoleLine(bool eraseFromCursor);