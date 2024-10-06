#include <iostream>
#include "./modules/json.h"


int main(){
    // Testing code
    json::JSON my_json = json::JSON::open("config.json");
    std::cout << my_json.to_stringf();
    try{
        json::jsonViewer v = my_json["src"];
        json::json_object obj = v.get<json::json_object>();
        for(auto& it: obj){
            std::string module_name = it.first;
            std::string module_path = it.second->get<json::json_object>()["path"]->get<std::string>();
            std::cout << '\n' << module_name << ": " << module_path;
        }
    }
    catch(std::runtime_error err){
        std::cout << err.what() << '\n';
    }
}