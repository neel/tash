#Tokai(তকাই) 
Tokai(তকাই)  is a C++ library for ArangoDB Database which includes APIs for HTTP based document access and a query builder for AQL (Arango Query Language)

---

## Example 1

```cpp
#include <iostream>
#include <tokai/arango.h>
#include <boost/format.hpp>
#include <boost/beast/http/status.hpp>

int main(){
    tokai::shell school("school"); // shell("school", "localhost", 8529, "root", "root")
    if(school.exists() == boost::beast::http::status::not_found){
        school.create();
        std::cout << boost::format("created database %1%") % school.database() << std::endl;
    }else{
        std::cout << boost::format("database %1% exists") % school.database() << std::endl;
    }
    tokai::vertex students(school, "students");
    if(students.exists() == boost::beast::http::status::not_found){
        students.create();
        std::cout << boost::format("created vertex %1%") % students.name() << std::endl;
    }else{
        std::cout << boost::format("vertex %1% exists") % students.name() << std::endl;
    }
    nlohmann::json document = {
        {"name", "Hijibijbij"},
        {"fathers_name", "Hijibijbij"},
        {"uncles_name", "Hijibijbij"}
    };
    boost::beast::http::status status = students.add(document);
    if(status == boost::beast::http::status::accepted){
        std::cout << boost::format("document created with key %1%") % document["_key"] << std::endl;
    }else{
        std::cout << "Failed to create document" << std::endl;
    }
    nlohmann::json hijibijbij = students.by_key(document["_key"].get<std::string>());
    std::cout << "retrieved document " << std::endl;
    std::cout << hijibijbij << std::endl;
    return 0;
}


```