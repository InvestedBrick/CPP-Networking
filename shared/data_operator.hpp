#include <string>
#include <memory>
class Data_Writer {
private:
    std::string& data;
    uint32_t byte_idx = 0;
    uint8_t bit_idx = 0x0;


public:    

    Data_Writer(std::string& str_ref) : data(str_ref) {}
    Data_Writer(const Data_Writer& other) : data(other.data), byte_idx(other.byte_idx), bit_idx(other.bit_idx){}
    //writes a bit 'bit' into the n-th bit of byte 'byte'
    void write_bit_n(uint8_t& byte, uint8_t n,bool bit){
        if (n > 0x7) return;
        if (bit)
            byte |= (0x1 << n);
        else
            byte &= ~(0x1 << n);
    }
    
    void write_byte_n(std::string& string, uint32_t n,uint8_t byte_){
        if (n >= string.length()) return;
        string[n] = byte_;
    }


    void write_byte(uint8_t byte){
        if (bit_idx == 0x0){
            write_byte_n(data,byte_idx,byte);
            byte_idx++;
        }else{
            // byte gets split up between multiple string bytes
            for (u_int8_t bit = 0x0; bit < 0x8 ; bit++){
                write_bit((byte >> bit ) & 0x1);
            }
        }
    }

    void write_bit(bool bit) {
        uint8_t& byte = reinterpret_cast<uint8_t&>(data[byte_idx]);
        write_bit_n(byte,bit_idx,bit);
        if (bit_idx == 0x7){
            byte_idx++;
            bit_idx = 0x0;
        }else{
            bit_idx++;
        }
    }

    void write_uint32_t(uint32_t value){
        for (int i = 0; i < 4; i++){
            write_byte(static_cast<uint8_t>(value >> (i * 8)));
        }
    }

};


class Data_Reader {
private:
    std::string& data;
    uint32_t byte_idx = 0;
    uint8_t bit_idx = 0x0;

public:

    Data_Reader(std::string& str_ref) : data(str_ref) {}
    Data_Reader(const Data_Reader& other) : data(other.data), byte_idx(other.byte_idx), bit_idx(other.bit_idx){}

    // returns the n-th bit of byte 'byte'
    // returns 0 on invalid bit
    bool read_bit_n(uint8_t byte, uint8_t n){
        if (n > 0x7) return false;
        return (byte >> n) & 0x1;
    }
    bool read_bit(){
        uint8_t byte = static_cast<uint8_t>(data[byte_idx]);
        bool bit = read_bit_n(byte,bit_idx);
        if (bit_idx == 0x7){
            byte_idx++;
            bit_idx = 0x0;
        }else{
            bit_idx++;
        }
        return bit;
    }

    // reads n-th byte of string
    // returns 0 if invalid byte
    uint8_t read_byte_n(std::string& string, uint32_t n){
        if (n >= string.length()) return 0;
        else return static_cast<uint8_t> (string[n]);
    }

    uint8_t read_byte() {
        if (bit_idx == 0x0){
            uint8_t byte = read_byte_n(data,byte_idx);
            byte_idx++;
            return byte;
        }else{
            uint8_t byte = 0x0;

            for (uint8_t bit = 0x0; bit < 0x8; bit++){
                if(read_bit()){
                    byte |= (0x1 << bit);
                }
            }
            return byte;

        }
    }

    uint32_t read_uint32_t(){
        uint32_t value = 0;
        for (int i = 0; i < 4; i++){
            value |= (static_cast<uint32_t>(read_byte()) << (i * 8));
        }
        return value;
    }

};