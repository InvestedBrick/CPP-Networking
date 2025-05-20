#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>
#include <queue>
#include "data_operator.hpp"
class Huffman {
private:

    struct Node{
        Node(){}
        Node(uint weight_, std::shared_ptr<Node> left_, std::shared_ptr<Node> right_, char c_)
        : weight(weight_), left(std::move(left_)), right(std::move(right_)), c(c_) {}
        uint weight;
        std::shared_ptr<Node> left = nullptr;
        std::shared_ptr<Node> right = nullptr;
        char c;

        bool operator < (const Node& other) {
            return ( this->weight < other.weight);
        }
    };

    std::string& data;
    std::vector<std::shared_ptr<Node>> freqs {};
    std::map<char,uint> freqs_copy {}; //copy for later use
    std::vector<uint8_t> bytes {};
    std::map<char,std::string> code_map {};
    

    void calculate_frequencies(){
        for (char c : data){
            //Maybe vec of Nodes
            auto find = std::find_if(freqs.begin(), freqs.end(), [&] (const std::shared_ptr<Node>& pair) {
                return pair->c == c;
            }); 
            //char is not in set
            if (find == freqs.end()){
                freqs.push_back(std::make_shared<Node>(1,nullptr,nullptr,c));
            }else{
                (*find)->weight++;
            }
        }
    }


    static bool cmp(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b){
        return a->weight > b->weight;
    }
    //sort the freqs in reverse order
    void sort_frequencies(){
        std::sort(freqs.begin(),freqs.end(),cmp);
    }

    void build_map(std::shared_ptr<Node>& node,std::string running_encoding){
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


    void save_freqs() {
        for (const auto& freq : this->freqs){
            freqs_copy.insert({freq->c,freq->weight});
        }
    }

    void encode_node(std::shared_ptr<Node>& node,Data_Writer& writer){
        if (node->left == nullptr && node->right == nullptr){
            writer.write_bit(1);
            writer.write_byte(node->c);
        }else{
            writer.write_bit(0);
            encode_node(node->left,writer);
            encode_node(node->right,writer);
        }
    }

    std::shared_ptr<Node> decode_tree(Data_Reader& reader){
        if (reader.read_bit() == 1){
            return std::make_shared<Node>(0,nullptr,nullptr,static_cast<char>(reader.read_byte()));
        }else{
            std::shared_ptr<Node> left = decode_tree(reader);
            std::shared_ptr<Node> right = decode_tree(reader);
            return std::make_shared<Node>(0,left,right,0);
        }
    }

    std::pair<char,uint32_t> decode_char(std::shared_ptr<Node> node, Data_Reader& reader, uint32_t bits_read = 0){
        if (node->right == nullptr && node->left == nullptr){
            return std::pair<char,uint32_t>({node->c,bits_read});
        }

        if (reader.read_bit() == 0){
            return decode_char(node->left,reader,bits_read + 1);
        }else{
            return decode_char(node->right,reader,bits_read + 1);
        }
    }
public:
    Huffman(std::string& data_) : data(data_) {}
    void print_tree_structure(const std::shared_ptr<Node>& node, const std::string& prefix = "", bool is_left = true) {
        if (!node) {
            return;
        }
    
        // Print the current node
        std::cout << prefix;
    
        // Add a connector to indicate left or right child
        std::cout << (is_left ? "├── " : "└── ");
    
        // Print node details
        if (node->left == nullptr && node->right == nullptr) {
            // Leaf node
            std::cout << "Leaf '" << node->c << "'" << std::endl;
        } else {
            // Internal node
            std::cout << "Internal"  << std::endl;
        }
    
        // Recurse for left and right children
        print_tree_structure(node->left, prefix + (is_left ? "│   " : "    "), true);
        print_tree_structure(node->right, prefix + (is_left ? "│   " : "    "), false);
    }

    std::string encode(){
        this->calculate_frequencies();
        this->sort_frequencies();
        this->save_freqs();
        
        //build tree
        while(freqs.size() > 1){
            auto left = std::move(freqs.back()); freqs.pop_back();
            auto right = std::move(freqs.back()); freqs.pop_back();
            auto parent = std::make_shared<Node>();
            parent->weight = left->weight + right->weight;
            parent->left  = std::move(left);
            parent->right = std::move(right);

            freqs.push_back(std::move(parent));

            this->sort_frequencies();

        }

        //encode string
        auto root = std::move(freqs.back());
        build_map(root,""); 
        //for(auto const& item : code_map){
        //    std::cout << "{" << item.first << " : " << item.second << "}" << std::endl;
        //}

        uint32_t bit_len = 0;
        for(const auto& item : freqs_copy){
            bit_len += code_map[item.first].length() * item.second; // length of encoded char * freq
        }
        u_int8_t rounded_up_by = 8 - (bit_len % 8);
        bit_len = bit_len + 8 - (bit_len % 8) ; // round to nearest byte size
        std::string encoded_string((bit_len / 8) + 1,'\0');
        
        //std::cout << "Encoded string size: " << encoded_string.length() << " bytes; original length: " << data.length() << " bytes" << std::endl;
        Data_Writer writer(encoded_string);
        writer.write_byte(rounded_up_by);
        for (const char& c : data){
            std::string code = code_map[c];

            for (const char& _c : code){
                writer.write_bit(_c == '1');
            }
        }

        uint32_t tree_size = 10 * freqs_copy.size() - 1;
        tree_size += 8 - (tree_size % 8);

        std::string encoded_tree(tree_size / 8, '\0');
        Data_Writer tree_writer(encoded_tree);
        encode_node(root,tree_writer);

        std::string tree_offset(4,'\0');
        Data_Writer offset_writer(tree_offset);
        offset_writer.write_uint32_t(encoded_string.length());


        //print_tree_structure(root);
        return tree_offset + encoded_string + encoded_tree;
    }

    std::string decode(std::string& encoded_string) {
        Data_Reader reader(encoded_string);

        uint32_t tree_offset = reader.read_uint32_t();
        uint8_t extra_bits = reader.read_byte();

        std::string encoded_data(tree_offset - 1,'\0');
        for (uint32_t i = 0; i < tree_offset - 1;++i){
            encoded_data[i] = static_cast<char>(reader.read_byte());
        }

        
        std::shared_ptr<Node> root = decode_tree(reader);

        Data_Reader encoded_reader(encoded_data);

        uint32_t total_bits = (tree_offset - 1) * 8 - extra_bits;
        uint32_t bits_read = 0;

        std::string decoded_string;
        while (bits_read < total_bits){
            std::pair<char,uint32_t> pair = decode_char(root,encoded_reader);
            decoded_string += pair.first;
            bits_read += pair.second;
        }
        //print_tree_structure(root);

        return decoded_string;

    }

};