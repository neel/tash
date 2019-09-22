#include <iostream>
#include <tash/arango.h>
#include <boost/format.hpp>
#include <boost/beast/http/status.hpp>

int main(){
    using namespace tash;
    
    std::string name("Hijibijbij");
    
    tash::shell shell("school"); // shell("school", "localhost", 8529, "root", "root")
    shell <<  select("s").in("students")  // use select instead of FOR because for is a C++ keyword
            / filter((clause("s.name") == "s.fathers_name")) // using std::string puts quotes around the value
            / sort().asc("s._key")
            / yield("s"); // use yield instead of RETURN because return is a C++ keyword
    nlohmann::json result;
    shell >> result;
    std::cout << result << std::endl;
    
    std::cout << insert(nlohmann::json{
        {"name", "tash"},
        {"fathers_name", "tash"}
    }).in("students") << std::endl;
    
    std::cout << select("u").in({
        {"name", "tash"},
        {"fathers_name", "tash"}
    }) / insert("u").in("students") << std::endl;
    
    std::cout << select("i").in(1, 10) / insert(assign("name", "CONCAT('test', i)").assign("gender", "(i % 2 == 0 ? 'f' : 'm')").assign("fathers_name", std::string("tash"))).in("users") << std::endl;
    std::cout << update(nlohmann::json{
        {"_key", 1234}
    }).with({
        {"uncles_name", "tash"}
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
        {"name", "tash"},
        {"uncles_name", "tash"}
    }).in("students") << std::endl;
    
    std::cout << erase(assign("_key", "1")).in("students") << std::endl;
    
    std::cout << upsert(nlohmann::json{
        {"name", "tokai"}
    }).insert({
        {"name", "tokai"},
        {"fathers_name", "tokai"}
    }).update({
        {"name", "tokai"},
        {"fathers_name", "tokai"}
    }).in("students") << std::endl;
    
    return 0;
}
