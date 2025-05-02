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
            default:{
                break;
            }
        }
    }

    /*
    void parse_cmd(const char* str,int n_bytes){
        std::string data(str,0,n_bytes);
        state_pos_x = pos_x;
        state_pos_y = pos_y;

        for (int idx = 0; idx < n_bytes;++idx){
            switch(data[idx]){
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
                    to_be_colored.push_back({pos_x,pos_y});
                    break;
                }
                default:{
                    return;
                }
            }
            
        }

    }
        */
};