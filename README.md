# Tokai(তকাই) 
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
    }
    tokai::vertex students(school, "students");
    if(students.exists() == boost::beast::http::status::not_found){
        students.create();
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
        std::cout << "Failed to create document with error " << status << std::endl;
    }
    nlohmann::json hijibijbij = students.by_key(document["_key"].get<std::string>());
    std::cout << "retrieved document " << std::endl;
    std::cout << hijibijbij << std::endl;
    return 0;
}

```

## AQL (Arango Query Language) Builder

### Retrieve / Filter / Sort

```cpp
tokai::shell shell("school"); // shell("school", "localhost", 8529, "root", "root")
shell <<  select("s").in("students")  // use select instead of FOR because for is a C++ keyword
        / filter((clause("s.name") == name) && (clause("s.fathers_name") == name)) // using std::string puts quotes around the value
        / sort().asc("s._key")
        / yield("s"); // use yield instead of RETURN because return is a C++ keyword
nlohmann::json result;
shell >> result;
```
```aql
FOR s IN students
    FILTER s.name == "Hijibijbij" AND s.fathers_name == "Hijibijbij"
    SORT s._key ASC 
    RETURN s
```

```cpp
  select("s").in("students") 
/ filter((clause("s.name") == "s.fathers_name")) // using C string doesn't put quotes around the value
/ sort().asc("s._key")
/ yield("s"); 
```
```aql
FOR s IN students
    FILTER s.name == s.fathers_name
    SORT s._key ASC 
    RETURN s
```

### insert

#### insert single row

```cpp
insert(nlohmann::json{
    {"name", "tokai"},
    {"fathers_name", "tokai"}
}).in("students");
```
```aql
INSERT {"fathers_name":"tokai","name":"tokai"} INTO students
```

#### insert multiple rows

```cpp
select("u").in({
    {"name", "tokai"},
    {"fathers_name", "tokai"}
}) / insert("u").in("students")
```
```aql
FOR u IN {"fathers_name":"tokai","name":"tokai"}
    INSERT u INTO students
```

#### generate rows

* in `nlohmann::json` string values are always quoted
* `tokai::assign` generates non-nested key value pairs (non-nested json)
* C style strings are unquoted, `std::string` is quoted


```cpp
  select("i").in(1, 10) 
/ insert(
    assign("name", "CONCAT('tokai', i)")
   .assign("gender", "(i % 2 == 0 ? 'f' : 'm')")
   .assign("fathers_name", std::string("tokai"))
  ).in("users")
```
```aql
FOR i IN 1..10
    INSERT {
        name: CONCAT('test', i), 
        gender: (i % 2 == 0 ? 'f' : 'm'),
        fathers_name: "tokai"
    } INTO users
```

#### update

```cpp
update(nlohmann::json{
    {"_key", 1234}
}).with({
    {"uncles_name", "tokai"}
}).in("students")
```
```aql
UPDATE {"_key":1234} WITH {"uncles_name":"tokai"} IN students
```

#### let

```cpp
  (let("date") = "DATE_NOW()")
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
    .assign("uncles_name", std::string("tokai"))
).in("users")
```
```aql
LET date = DATE_NOW()
    FOR user IN users
        FILTER user.isImportantUser == null
        LET numberOfLogins = (
            FOR login IN logins
                FILTER login.user == user._key
                COLLECT  WITH COUNT INTO numLogins
                RETURN numLogins
        )
        FILTER numberOfLogins > 50
        UPDATE user WITH {
            isImportantUser: 1, 
            dateBecameImportant: date,
            uncles_name: "tokai"
        } IN users
```