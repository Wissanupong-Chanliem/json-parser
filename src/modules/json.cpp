#include "json.h"
using namespace json;

JSON::JSON(){
    this->root=nullptr;
}

JSON::JSON(std::ifstream& json_file){
    std::string json_string = "";
    char letter;
    if(json_file.is_open()){
        while(!json_file.get(letter).eof()){
            json_string +=letter;
        }
    }
    jsonParser parser = jsonParser(json_string);
    this->root = parser.parse();
}

JSON::JSON(std::string json_string){
    jsonParser parser = jsonParser(json_string);
    this->root = parser.parse();
}

JSON::jsonParser::jsonParser(std::string json_string){
    this->json_string=json_string;
    this->tokenizer.add_json_string(json_string);
}

jsonViewer* JSON::jsonParser::parse(){
    look_ahead = this->tokenizer.get_next_token();
    return new jsonViewer(this->parseObject());
}

json_object JSON::jsonParser::parseObject(){
    this->eat(json_tokenizer::OPEN_BRACE);
    json_object fields;
    while(this->look_ahead.type!=json_tokenizer::CLOSE_BRACE){
        fields.insert(this->parseField());
        if(this->look_ahead.type!=json_tokenizer::CLOSE_BRACE){
            this->eat(json_tokenizer::COMMA);
        }
    }
    this->eat(json_tokenizer::CLOSE_BRACE);
    return fields;
}

std::pair<std::string,jsonViewer*> JSON::jsonParser::parseField(){
    
    json_tokenizer::token field_name = this->eat(json_tokenizer::STRING_LITERAL);
    this->eat(json_tokenizer::COLON);
    jsonViewer* value = this->parseLiteral();
    return std::make_pair(std::get<std::string>(field_name.val),value);
}

jsonViewer* JSON::jsonParser::parseLiteral(){
    jsonViewer* new_node = nullptr;
    switch(this->look_ahead.type){
        case json_tokenizer::OPEN_BRACE:{
            return new jsonViewer(this->parseObject());
        }
        case json_tokenizer::OPEN_BRACKET:{
            json_array values;
            this->eat(json_tokenizer::OPEN_BRACKET);
            while (!this->tokenizer.reach_the_end() && this->look_ahead.type != json_tokenizer::CLOSE_BRACKET){
                values.push_back(this->parseLiteral());
                if (this->look_ahead.type == json_tokenizer::COMMA)
                {
                    this->eat(json_tokenizer::COMMA);
                }
            }
            this->eat(json_tokenizer::CLOSE_BRACKET);
            return new jsonViewer(values);
        }
        case json_tokenizer::FLOAT:
            new_node = new jsonViewer(std::get<float>(this->look_ahead.val));
            this->eat(json_tokenizer::FLOAT);
            return new_node;
        case json_tokenizer::DOUBLE:
            new_node = new jsonViewer(std::get<double>(this->look_ahead.val));
            this->eat(json_tokenizer::DOUBLE);
            return new_node;
        case json_tokenizer::INT:
            new_node = new jsonViewer(std::get<int>(this->look_ahead.val));
            this->eat(json_tokenizer::INT);
            return new_node;
        case json_tokenizer::LONG:
            new_node = new jsonViewer(std::get<long long>(this->look_ahead.val));
            this->eat(json_tokenizer::LONG);
            return new_node;
        default:
            new_node = new jsonViewer(std::get<std::string>(this->look_ahead.val));
            this->eat(json_tokenizer::STRING_LITERAL);
            return new_node;
    }
}

json_tokenizer::token JSON::jsonParser::eat(json_tokenizer::t_type type){
    json_tokenizer::token current = this->look_ahead;
    if (current.type != type){

        throw std::runtime_error("Unexpected Token of type " + this->look_ahead.get_type_str() + ". Expected: " + json_tokenizer::token::tokentype_to_string(type) + ".");
    }
    
    this->look_ahead = this->tokenizer.get_next_token();
    return current;
}


JSON::~JSON(){
    delete this->root;
}

jsonViewer::~jsonViewer(){
    if(std::holds_alternative<json_object>(this->val)){
        json_object child_map = std::get<json_object>(this->val);
        for (auto &it : child_map){
            delete it.second;
        }
    }
    else if(std::holds_alternative<json_array>(this->val)){
        json_array Array = std::get<json_array>(this->val);
        for (int i=0;i<Array.size();i++){
            delete Array[i];
        }
    }
}

std::string JSON::to_string_helper(jsonViewer* curr){
    std::string out = "";
    if (std::holds_alternative<json_object>(curr->val)){
        json_object child_map = std::get<json_object>(curr->val);
        int counter = 0;
        out+="{";
        for (auto& it:child_map){
            out+=it.first+":"+to_string_helper(it.second);
            if(counter<child_map.size()-1){
                out+=",";
            }
            counter++;
        }
        out+="}";
    }
    else if (std::holds_alternative<json_array>(curr->val)){
        json_array Array = std::get<json_array>(curr->val);
        out+="[";
        for (int i=0;i<Array.size();i++){
            out+=to_string_helper(Array[i]);
            if(i<Array.size()-1){
                out+=",";
            }
        }
        out+="]";
    }
    else if (std::holds_alternative<float>(curr->val)){
        float number = std::get<float>(curr->val);
        out+=std::to_string(number);
    }
    else if (std::holds_alternative<double>(curr->val)){
        double number = std::get<double>(curr->val);
        out+=std::to_string(number);
    }
    else if (std::holds_alternative<int>(curr->val)){
        int number = std::get<int>(curr->val);
        out+=std::to_string(number);
    }
    else if (std::holds_alternative<long long>(curr->val)){
        long long number = std::get<long long>(curr->val);
        out+=std::to_string(number);
    }
    else if (std::holds_alternative<std::string>(curr->val)){
        out+="\""+std::get<std::string>(curr->val)+"\"";
    }
    return out;
}

std::string JSON::to_string(){
    return to_string_helper(this->root);
}

std::string JSON::to_stringf(){
    std::string json_string = to_string_helper(this->root);
    std::string fstring = "";
    int level = 0;
    bool in_array = false;
    for(int i=0;i<json_string.length();i++){
        switch(json_string[i]){
            case '[':
                in_array=true;
                fstring+=json_string[i];
                break;
            case ']':
                in_array=false;
                fstring+=json_string[i];
                break;
            case '{':
                if(!in_array){
                    fstring+=json_string[i];
                    fstring+='\n';
                    level++;
                    for(int j=0;j<level;j++){
                        fstring+="    ";
                    }
                }
                else{
                    fstring+=json_string[i];
                }
                break;
            case '}':
                
                if(!in_array){
                    fstring+='\n';
                    level--;
                    for(int j=0;j<level;j++){
                        fstring+="    ";
                    }
                }
                fstring+=json_string[i];
                
                break;
            case ',':
                fstring+=json_string[i];
                if(!in_array){
                    fstring+='\n';
                    for(int j=0;j<level;j++){
                        fstring+="    ";
                    }
                }
                break;
            default:

                fstring+=json_string[i];
                break;
        }
    }
    
    return fstring;
}

JSON JSON::from_str(std::string json_string){
    jsonParser parser = jsonParser(json_string);
    return JSON(json_string);
}

JSON JSON::open(std::filesystem::path json_file){
    std::string json_string = "";
    std::ifstream file(json_file);
    char letter;
    if(file.is_open()){
        while(!file.get(letter).eof()){
            json_string +=letter;
        }
    }
    return JSON(json_string);
}

jsonViewer& JSON::operator[](std::string key){
    if (std::holds_alternative<json_object>(this->root->val)){
        auto map = std::get<json_object>(this->root->val);
        if(map.count(key)>0){
            return *(map[key]);
        }
        throw std::runtime_error("error: object does not contains key \""+key+"\".");
    }
    throw std::runtime_error("error: use of key on non-object type value.");
}

jsonViewer::jsonViewer(json_object val){
    this->val = val;
}
jsonViewer::jsonViewer(json_array val){
    this->val = val;
}
jsonViewer::jsonViewer(std::string val){
    this->val = val;
}
jsonViewer::jsonViewer(double val){
    this->val = val;
}
jsonViewer::jsonViewer(float val){
    this->val = val;
}
jsonViewer::jsonViewer(long long val){
    this->val = val;
}
jsonViewer::jsonViewer(int val){
    this->val = val;
}

template <typename T>
T& jsonViewer::get(){
    if(std::holds_alternative<T>(this->val)){
        return std::get<T>(this->val);
    }
    const std::string type_map[7] = {
        "Object",
        "Array",
        "String",
        "Int",
        "Long Long",
        "Float",
        "Double"
    };
    throw std::runtime_error("Mismatched type: Field contain value of type "+type_map[this->val.index()]+" Not "+ typeid(T).name());
}
template int& jsonViewer::get<int>();
template long long& jsonViewer::get<long long>();
template float& jsonViewer::get<float>();
template double& jsonViewer::get<double>();
template std::string& jsonViewer::get<std::string>();
template json_array& jsonViewer::get<json_array>();
template json_object& jsonViewer::get<json_object>();

jsonViewer& jsonViewer::operator[](std::string key){
    if (std::holds_alternative<json_object>(this->val)){
        auto map = std::get<json_object>(this->val);
        if(map.count(key)>0){
            return *(map[key]);
        }
        throw std::runtime_error("error: object does not contains key \""+key+"\".");
    }
    throw std::runtime_error("error: use of key on non-object type value.");
}
jsonViewer& jsonViewer::operator[](uint32_t index){
    if (std::holds_alternative<json_array>(this->val)){
        auto vec = std::get<json_array>(this->val);
        if(index>=0&&index<vec.size()){
            return *(vec[index]);
        }
        throw std::runtime_error("error: use of out of bound index.");
    }
    throw std::runtime_error("error: use of index on non-array type value.");
}