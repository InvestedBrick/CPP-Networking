#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "colors.hpp"

#define GRID_WIDTH 150
#define GRID_HEIGHT 40


class Grid {
public:
    int grid[GRID_HEIGHT][GRID_WIDTH];
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
            grid[y][x] = color;
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
                grid[i][j] = char_to_color(data[i * GRID_WIDTH + j]);
            }
        }
    }
    

    std::string rle_efficient(const std::string& data){
        std::string result;

        uint8_t count = 1;

        for (size_t i = 1; i < data.length();++i){
            if (data[i] == data[i - 1] && count < 255){
                count++;
            }else{
                result += static_cast<char>(count);
                result += data[i-1];
                count = 1;
            }

        }
        result += static_cast<char>(count);
        result += data[data.length() - 1];
        return result;
    }

    std::string rle_decode_efficient(const std::string& data){
        std::string result;
        for (size_t i = 0; i < data.length(); i += 2){
            uint8_t count = static_cast<uint8_t>(data[i]);
            result.append(count,data[i + 1]);
        }

        return result;
    }
    std::string rle_encode(const std::string& data) {
        std::string result;
        int count = 1;
        for (size_t i = 1; i < data.length(); ++i){
            if (data[i] == data[i - 1]) {
                count++;
            } else {
                result += std::to_string(count) + data[i - 1];
                count = 1;
            }
        }
        result += std::to_string(count) + data[data.length() - 1];
        return result;
    }

    std::string rle_decode(const std::string& data) {
        std::string result;
        std::string num_buf;

        for (char c : data){
            if (isdigit(c)){
                num_buf += c;
            }else{
                if (!num_buf.empty()){
                    int count = std::stoi(num_buf);
                    result.append(count, c);
                    num_buf.clear();
                }
            }
            
        }
        return result;
    }

    void save_canvas() {
        std::fstream save_file("save.canv",std::ios::out);
        if (!save_file){
            std::cerr << "Failed to open file\n";
            return;
        }
        std::string encoded = this->encode_long();
        save_file.write(encoded.c_str(),encoded.length());
        save_file.close();
    }

    void restore_canvas() {
        std::fstream save_file("save.canv", std::ios::in);
        if (!save_file){
            std::cerr << "Failed to open file\n";
            return;
        }

        std::string encoded;
        std::getline(save_file,encoded);
        save_file.close();
        this->decode_long(encoded);
    }


};