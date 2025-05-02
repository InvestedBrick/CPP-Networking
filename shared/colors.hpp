#pragma once
#include <string>

#define ANSI_COLOR_BLACK    30
#define ANSI_COLOR_RED      31
#define ANSI_COLOR_GREEN    32
#define ANSI_COLOR_YELLOW   33
#define ANSI_COLOR_BLUE     34
#define ANSI_COLOR_MAGENTA  35
#define ANSI_COLOR_CYAN     36
#define ANSI_COLOR_WHITE    37
#define ANSI_COLOR_RESET    0

#define ANSI_COLOR_BACKGROUND_WHITE 47
#define PIXEL_CHAR "#"

std::string color_to_string(int color) {
    switch (color) {
        case ANSI_COLOR_BLACK:            return "\033[1;30m";
        case ANSI_COLOR_RED:              return "\033[1;31m";
        case ANSI_COLOR_GREEN:            return "\033[1;32m";
        case ANSI_COLOR_YELLOW:           return "\033[1;33m";
        case ANSI_COLOR_BLUE:             return "\033[1;34m";
        case ANSI_COLOR_MAGENTA:          return "\033[1;35m";
        case ANSI_COLOR_CYAN:             return "\033[1;36m";
        case ANSI_COLOR_WHITE:            return "\033[1;37m";
        case ANSI_COLOR_BACKGROUND_WHITE: return "\033[1;47m";
        default:                          return "\033[0m"; 
    }
}

char color_to_char(int color) {
    switch (color) {
        case ANSI_COLOR_BLACK:            return 'B';
        case ANSI_COLOR_RED:              return 'R';
        case ANSI_COLOR_GREEN:            return 'G';
        case ANSI_COLOR_YELLOW:           return 'Y';
        case ANSI_COLOR_BLUE:             return 'U';
        case ANSI_COLOR_MAGENTA:          return 'M';
        case ANSI_COLOR_CYAN:             return 'C';
        case ANSI_COLOR_WHITE:            return 'W';
        case ANSI_COLOR_BACKGROUND_WHITE: return 'X';
        default:                          return ' '; 
    }
}

int char_to_color(char c){
    switch(c) {
        case 'B':   return ANSI_COLOR_BLACK;
        case 'R':   return ANSI_COLOR_RED;
        case 'G':   return ANSI_COLOR_GREEN;
        case 'Y':   return ANSI_COLOR_YELLOW;
        case 'U':   return ANSI_COLOR_BLUE;
        case 'M':   return ANSI_COLOR_MAGENTA;
        case 'C':   return ANSI_COLOR_CYAN;
        case 'W':   return ANSI_COLOR_WHITE;
        case 'X':   return ANSI_COLOR_BACKGROUND_WHITE;
        default:    return ANSI_COLOR_RESET;
    }

}