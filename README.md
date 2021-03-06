# Tash (ট্যাশ) 
Tash (ট্যাশ) is a small Open Source (FreeBSD License) C++ library for ArangoDB Database which includes APIs for HTTP based document access and a query builder for AQL (Arango Query Language). 

[![License](https://img.shields.io/badge/License-BSD%202--Clause-orange.svg)](https://opensource.org/licenses/BSD-2-Clause)
[![pipeline status master](https://gitlab.com/neel.basu/tash/badges/master/pipeline.svg)](https://gitlab.com/neel.basu/tash/commits/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/b5fb5f2fd8f8476eb7bcfd1ec4fe6e93)](https://www.codacy.com/manual/neel.basu.z/tash?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=neel/tash&amp;utm_campaign=Badge_Grade)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/neel/tash.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/neel/tash/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/neel/tash.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/neel/tash/context:cpp)

![tashgoru](tash-goru.png "ট্যাঁশগরু সুকুমার রায়")

* [Building](#building)
* [Basic Example](#basic-example)
* [Query builder](#aql-arango-query-language-builder)

---

## Building

### prerequisites

* C++ compiler
* CMake
* boost library
* nlohmann::json [OPTIONAL] (tash ships with a single file version of nlohmann::json)

### compiling

```bash
git clone https://gitlab.com/neel.basu/tash.git
mkdir build
cd build
cmake ..
make
```

## Basic Example

```cpp
#include <iostream>
#include <tash/arango.h>
#include <boost/format.hpp>
#include <boost/beast/http/status.hpp>

int main(){
    tash::shell school("school"); // shell("school", "localhost", 8529, "root", "root")
    if(school.exists() == boost::beast::http::status::not_found){
        school.create();
    }
    tash::vertex students(school, "students");
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
tash::shell shell("school"); // shell("school", "localhost", 8529, "root", "root")
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
    {"name", "tash"},
    {"fathers_name", "tash"}
}).in("students");
```
```aql
INSERT {"fathers_name":"tash","name":"tash"} INTO students
```

#### insert multiple rows

```cpp
select("u").in({
    {"name", "tash"},
    {"fathers_name", "tash"}
}) / insert("u").in("students")
```
```aql
FOR u IN {"fathers_name":"tash","name":"tash"}
    INSERT u INTO students
```

### generate rows

* in `nlohmann::json` string values are always quoted
* `tash::assign` generates non-nested key value pairs (non-nested json)
* C style strings are unquoted, `std::string` is quoted


```cpp
  select("i").in(1, 10) 
/ insert(
    assign("name", "CONCAT('tash', i)")
   .assign("gender", "(i % 2 == 0 ? 'f' : 'm')")
   .assign("fathers_name", std::string("tash"))
  ).in("users")
```
```aql
FOR i IN 1..10
    INSERT {
        name: CONCAT('test', i), 
        gender: (i % 2 == 0 ? 'f' : 'm'),
        fathers_name: "tash"
    } INTO users
```

### update

```cpp
update(nlohmann::json{
    {"_key", 1234}
}).with({
    {"uncles_name", "tash"}
}).in("students")
```
```aql
UPDATE {"_key":1234} WITH {"uncles_name":"tash"} IN students
```

### let

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
    .assign("uncles_name", std::string("tash"))
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
            uncles_name: "tash"
        } IN users
```

### replace

```cpp
replace(nlohmann::json{
    {"_key", 1234}
}).with({
    {"name", "tash"},
    {"uncles_name", "tash"}
}).in("students")
```
```aql
REPLACE {"_key":1234} 
    WITH {
        "name":"tash",
        "uncles_name":"tash"
    } IN students
```

### remove

```cpp
erase(assign("_key", "1")).in("students")
```
```aql
REMOVE {_key: 1} IN students
```

### upsert

```cpp
upsert(nlohmann::json{
    {"name", "tokai"}
}).insert({
    {"name", "tokai"},
    {"fathers_name", "tokai"}
}).update({
    {"name", "tokai"},
    {"fathers_name", "tokai"}
}).in("students")
```
```aql
UPSERT {"name":"tokai"} 
    INSERT {"fathers_name":"tokai","name":"tokai"} 
    UPDATE {"fathers_name":"tokai","name":"tokai"}
    IN students
```

## create graph

```cpp
auto graph = ddl::graph_definition("corpus")
    .add_edge_definition(ddl::edge_definition("StructuralE").add_from("Word").add_to("Word").add_to("Sentence"))
    .add_edge_definition(ddl::edge_definition("WNeighbourhoodE").add_from("Word").add_to("Word"))
    .add_edge_definition(ddl::edge_definition("InstanceE").add_from("Word").add_to("Vocabulary"));
query("_api/gharial", graph.json().dump());
```

## create aql function

```cpp
std::string function_str = R"(
    function(p1, p2){
        // function body
    }
)";
auto fnc = nlohmann::json({
    {"name", "NS::FNAME"},
    {"code", function_str},
    {"isDeterministic", true}
});
query("_api/aqlfunction", fnc.dump());
```
