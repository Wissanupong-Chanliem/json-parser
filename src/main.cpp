#include <iostream>
#include "./modules/json.h"


int main(){
    json::JSON my_json = json::JSON::open("config.json");
    try{
        json::jsonViewer v = my_json["src"];
        json::json_object obj = my_json["src"].get<json::json_object>();
        for(auto& it: obj){
            std::cout << it.first << '\n';
        }
    }
    catch(std::runtime_error err){
        std::cout << err.what() << '\n';
    }
}