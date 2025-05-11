#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>
#include "data_operator.hpp"
class Huffman {
private:

    struct Node{
        Node(){}
        Node(uint weight_, std::unique_ptr<Node> left_, std::unique_ptr<Node> right_, char c_)
        : weight(weight_), left(std::move(left_)), right(std::move(right_)), c(c_) {}
        uint weight;
        std::unique_ptr<Node> left = nullptr;
        std::unique_ptr<Node> right = nullptr;
        char c;

        bool operator < (const Node& other) {
            return ( this->weight < other.weight);
        }
    };

    std::string& data;
    std::vector<std::unique_ptr<Node>> freqs {};
    std::vector<uint8_t> bytes {};
    std::map<char,std::string> code_map {};
    

    void calculate_frequencies(){
        for (char c : data){
            //Maybe vec of Nodes
            auto find = std::find_if(freqs.begin(), freqs.end(), [&] (const std::unique_ptr<Node>& pair) {
                return pair->c == c;
            }); 
            //char is not in set
            if (find == freqs.end()){
                freqs.push_back(std::make_unique<Node>(1,nullptr,nullptr,c));
            }else{
                (*find)->weight++;
            }
        }
    }


    static bool cmp(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b){
        return a->weight > b->weight;
    }
    //sort the freqs in reverse order
    void sort_frequencies(){
        std::sort(freqs.begin(),freqs.end(),cmp);
    }

    void build_map(std::unique_ptr<Node>& node,std::string running_encoding){
        if(node->left == nullptr && node->right == nullptr){
           //leaf
           code_map.insert({node->c,running_encoding}); 
           return;
        }

        if(node->left != nullptr){
            build_map(node->left,running_encoding + "0");
        }
        if(node->right != nullptr){
            build_map(node->right,running_encoding + "1");
        }
    }

public:
    Huffman(std::string& data_) : data(data_) {}

    void encode(){
        this->calculate_frequencies();
        this->sort_frequencies();
        
        //build tree
        while(freqs.size() > 1){
            auto left = std::move(freqs.back()); freqs.pop_back();
            auto right = std::move(freqs.back()); freqs.pop_back();
            auto parent = std::make_unique<Node>();
            parent->weight = left->weight + right->weight;
            parent->left  = std::move(left);
            parent->right = std::move(right);

            freqs.push_back(std::move(parent));

            this->sort_frequencies();

        }

        //encode string
        auto root = std::move(freqs.back());
        build_map(root,""); 
        for(auto const& item : code_map){
            std::cout << "{" << item.first << " : " << item.second << "}" << std::endl;
        }
    }

};