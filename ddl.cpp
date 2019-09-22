/*
 * Copyright 2019 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tash/ddl.h"

tash::ddl::edge_definition::edge_definition(const std::string& name): _collection(name){}

tash::ddl::edge_definition & tash::ddl::edge_definition::add_from(const std::string& name){
    _from.push_back(name);
    return *this;
}
tash::ddl::edge_definition & tash::ddl::edge_definition::add_to(const std::string& name){
    _to.push_back(name);
    return *this;
}


nlohmann::json tash::ddl::edge_definition::json() const{
    nlohmann::json from = nlohmann::json::array();
    for(const std::string& v: _from){
        from.push_back(v);
    }
    nlohmann::json to = nlohmann::json::array();
    for(const std::string& v: _to){
        to.push_back(v);
    }
    return nlohmann::json({
        {"collection", _collection},
        {"from", from},
        {"to", to}
    });
}
tash::ddl::graph_definition::graph_definition(const std::string& name): _name(name){}

tash::ddl::graph_definition& tash::ddl::graph_definition::add_edge_definition(const tash::ddl::edge_definition& def){
    _edge_definitions.push_back(def);
    return *this;
}
nlohmann::json tash::ddl::graph_definition::json() const{
    nlohmann::json edge_defs = nlohmann::json::array();
    for(auto def: _edge_definitions){
        edge_defs.push_back(def.json());
    }
    return nlohmann::json({
        {"name", _name},
        {"edgeDefinitions", edge_defs}
    });
}
