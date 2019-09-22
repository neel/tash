/*
 * Copyright (c) 2018, Sunanda Bose (Neel Basu) (neel.basu.z@gmail.com) 
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE. 
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
