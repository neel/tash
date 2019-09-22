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

#ifndef DDL_H
#define DDL_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace tash{
namespace ddl{
    
    struct edge_definition{
        std::string              _collection;
        std::vector<std::string> _from;
        std::vector<std::string> _to;
        
        edge_definition(const std::string& name);
        edge_definition& add_from(const std::string& name);
        edge_definition& add_to(const std::string& name);
        
        nlohmann::json json() const;
    };
    
    struct graph_definition{
        std::string _name;
        std::vector<edge_definition> _edge_definitions;
        
        graph_definition(const std::string& name);
        graph_definition& add_edge_definition(const edge_definition& def);
        
        nlohmann::json json() const;
    };

}
}


#endif // DDL_H
