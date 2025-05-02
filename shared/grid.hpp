#include <string>
#include <iostream>
#include "colors.hpp"

#define GRID_WIDTH 100
#define GRID_HEIGHT 20


class Grid {
public:
    int grid[GRID_WIDTH][GRID_HEIGHT];
    Grid() {
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                grid[i][j] = ANSI_COLOR_BLACK;
            }
        }
    }

    std::string to_string() {
        std::string result;
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                result += color_to_string(grid[i][j]) + PIXEL_CHAR + color_to_string(ANSI_COLOR_RESET);
            }
            result += "\n";
        }
        return result;
    }

    void set_pixel(int x, int y, const int color) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            grid[x][y] = color;
        }
    }

    std::string encode_long(){
        std::string result;
        for (int i = 0; i < GRID_HEIGHT; ++i) {
            for (int j = 0; j < GRID_WIDTH; ++j) {
                result += color_to_char(grid[i][j]);
            }
        }
        return result;
    }

    void decode_long(const std::string& data) {
        for (int i = 0; i < GRID_HEIGHT;++i){
            for(int j = 0; j < GRID_WIDTH; ++j){
                grid[i][j] = char_to_color(data[i * GRID_HEIGHT + j]);
            }
        }
    }
    
};