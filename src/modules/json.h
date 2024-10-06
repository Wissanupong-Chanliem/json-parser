#ifndef JSON_H
#define JSON_H

#include <filesystem>
#include <stdexcept>
#include <variant>
#include <unordered_map>
#include "tokenizer/tokenizer.h"
#include <vector>

namespace json{
    class jsonViewer;
    typedef std::unordered_map<std::string,jsonViewer*> json_object;
    typedef std::vector<jsonViewer*> json_array;

    class JSON{
        private:
            class jsonParser{
                json_tokenizer::Tokenizer tokenizer;
                std::string json_string;
                json_tokenizer::token look_ahead;

                public:
                    jsonParser(std::string json_string);
                    jsonViewer *parse();
                    json_object parseObject();
                    std::pair<std::string, jsonViewer *> parseField();
                    jsonViewer *parseLiteral();
                    json_tokenizer::token eat(json_tokenizer::t_type type);
            };
            
            static std::string to_string_helper(jsonViewer* curr);
            jsonViewer* root;

        public:
            
            JSON();
            JSON(std::ifstream& json_file);
            JSON(std::string json_string);
            ~JSON();
            std::string to_string();
            std::string to_stringf();
            static JSON from_str(std::string json_string);
            static JSON open(std::filesystem::path json_file);
            //Use to access field in object using key and array element using index
            //Error when used on object that's not compatible with used index type
            jsonViewer& operator[](std::string key);
    };

    class jsonViewer{
        public:
            std::variant<json_object, json_array, std::string, int, long long, float, double> val;
            jsonViewer();
            jsonViewer(json_object val);
            jsonViewer(json_array val);
            jsonViewer(std::string val);
            jsonViewer(double val);
            jsonViewer(float val);
            jsonViewer(long long val);
            jsonViewer(int val);
            ~jsonViewer();
            template <typename T>
            T &get();
            // Use to access field in object using key and array element using index
            // Error when used on object that's not compatible with used index type
            jsonViewer& operator[](std::string key);
            jsonViewer& operator[](uint32_t index);
    };
}




#endif