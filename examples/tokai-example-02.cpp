#include <iostream>
#include <tokai/arango.h>
#include <boost/format.hpp>
#include <boost/beast/http/status.hpp>

int main(){
    using namespace tokai;
    
    std::string name("Hijibijbij");
    
    tokai::shell shell("school"); // shell("school", "localhost", 8529, "root", "root")
    shell <<  select("s").in("students")  // use select instead of FOR because for is a C++ keyword
            / filter((clause("s.name") == "s.fathers_name")) // using std::string puts quotes around the value
            / sort().asc("s._key")
            / yield("s"); // use yield instead of RETURN because return is a C++ keyword
    nlohmann::json result;
    shell >> result;
    std::cout << result << std::endl;
    
    std::cout << insert(nlohmann::json{
        {"name", "tokai"},
        {"fathers_name", "tokai"}
    }).in("students") << std::endl;
    
    std::cout << select("u").in({
        {"name", "tokai"},
        {"fathers_name", "tokai"}
    }) / insert("u").in("students") << std::endl;
    
    std::cout << select("i").in(1, 10) / insert(assign("name", "CONCAT('test', i)").assign("gender", "(i % 2 == 0 ? 'f' : 'm')").assign("fathers_name", std::string("tokai"))).in("users") << std::endl;
    std::cout << update(nlohmann::json{
        {"_key", 1234}
    }).with({
        {"uncles_name", "tokai"}
    }).in("students") << std::endl;
    
    auto query = (let("date") = "DATE_NOW()")
      / select("user").in("users")
        / filter(clause("user.isImportantUser") == "null")
        / (let("numberOfLogins")  = 
            select("login").in("logins")
            / filter(clause("login.user") == "user._key")
            / collect().with("COUNT").in("numLogins")
            / yield("numLogins")
        )
        / filter(clause("numberOfLogins") > 50)
        / update("user").with(
             assign("isImportantUser", 1)
            .assign("dateBecameImportant", "date")
        ).in("users");
    
    std::cout << query << std::endl;
        
    std::cout << replace(nlohmann::json{
        {"_key", 1234}
    }).with({
        {"name", "tokai"},
        {"uncles_name", "tokai"}
    }).in("students") << std::endl;
    
    std::cout << erase(assign("_key", "1")).in("students") << std::endl;
    
//     std::cout << upsert(
//         assign("name", std::string("superuser"))
//     ).insert(
//          assign("name", std::string("superuser"))
//         .assign("logins", "1")
//         .assign("dateCreated", "DATE_NOW()")
//     ).update(
//         assign("logins", "OLD.logins + 1")   
//     ).in("students") << std::endl;
    
    return 0;
}
