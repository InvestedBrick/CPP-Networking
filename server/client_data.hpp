#include <string>
#include <vector>
#include "../shared/grid.hpp"

class client_data {

private:
    std::string buf;
    int state_pos_x;
    int state_pos_y;
    
public:
    int pos_x;
    int pos_y;
    int current_color;
    bool color_current_cell;
    
    client_data(){
        this->state_pos_x = 0;
        this-> state_pos_y = 0;
        this->pos_x = 0;
        this->pos_y = 0;
        this->current_color = ANSI_COLOR_WHITE;
        this->color_current_cell = false;
    }

    void parse_char(char c){
        switch(c){
            case 'w': {
                if (pos_y == 0){
                    pos_y = GRID_HEIGHT - 1;
                }else{
                    pos_y--;
                }
                break;
            }
            case 'a':{
                if (pos_x == 0){
                    pos_x = GRID_WIDTH - 1;
                }else{
                    pos_x--;
                }
                break;
            }
            case 's': {
                if (pos_y == GRID_HEIGHT - 1){
                    pos_y = 0;
                }else{
                    pos_y++;
                }
                break;
            }
            case 'd': {
                if (pos_x == GRID_WIDTH - 1){
                    pos_x = 0;
                }else{
                    pos_x++;
                }
                break;
            }
            case 'c': {
                color_current_cell = true;
                break;
            }
            case '0':{
                current_color = ANSI_COLOR_BLACK;
                break;
            }
            case '1': {
                current_color = ANSI_COLOR_WHITE;
                break;
            }
            case '2': {
                current_color = ANSI_COLOR_RED;
                break;
            }
            case '3': {
                current_color = ANSI_COLOR_GREEN;
                break;
            }
            case '4': {
                current_color = ANSI_COLOR_YELLOW;
                break;
            }
            case '5': {
                current_color = ANSI_COLOR_BLUE;
                break;
            }
            case '6': {
                current_color = ANSI_COLOR_MAGENTA;
                break;
            }
            case '7': {
                current_color = ANSI_COLOR_CYAN;
                break;
            }
            default:{
                break;
            }
        }
    }

    
};