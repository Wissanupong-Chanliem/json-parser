#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <fstream>
#include <stdexcept>
#include <variant>
#include <unordered_map>
#include <string>

namespace json_tokenizer{
    enum t_type{
        OPEN_BRACE,
        CLOSE_BRACE,
        OPEN_BRACKET,
        CLOSE_BRACKET,
        COLON,
        COMMA,
        STRING_LITERAL,
        FLOAT,
        DOUBLE,
        INT,
        LONG,
        END,
        UNDEFINED
    };


    class token{
        
        public:
            static std::string tokentype_to_string(t_type type);
            t_type type;
            std::variant<std::string,float,double,int,long long> val;
            token();
            token(t_type token_type,std::string value);
            token(t_type token_type,double value);
            token(t_type token_type,char value);
            token(t_type token_type,float value);
            token(t_type token_type,int value);
            token(t_type token_type,long long value);
            template<typename T> T get_val();
            template<typename A> bool val_is();
            std::string get_type_str();
    };



    class Tokenizer {
        private:
            std::string content;
            int cursor;
            
        public:
            Tokenizer();
            Tokenizer(std::string json_string);
            void add_json_string(std::string json_string);
            bool reach_the_end();
            token get_next_token();
    };

}


#endif
