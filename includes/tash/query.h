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

#ifndef ARANGOPP_QUERY_H
#define ARANGOPP_QUERY_H

#include <string>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <nlohmann/json.hpp>
#include <boost/lexical_cast.hpp>

namespace tash{

    struct null_assignment{
        std::size_t length() const{return 0;}
        std::string _to_string() const{return "";}
        std::string to_string() const{return "";}
    };
    
    template <typename S, typename T>
    struct assignment{
        typedef assignment<S, T> self_type;
        
        S _previous;
        std::string _key;
        T _value;
        
        assignment(const S& previous, const std::string& key, const T& value): _previous(previous), _key(key), _value(value){}
        std::size_t length() const{return _previous.length()+1;}
        template <typename U>
        assignment<self_type, U> assign(const std::string& key, const U& value){
            return assignment<self_type, U>(*this, key, value);
        }
        std::string _to_string() const{
            std::string previous = _previous._to_string();
            if(previous.empty()){
                previous += (boost::format("%1%: %2%") % _key % _value).str();
            }else{
                previous += (boost::format(", %1%: %2%") % _key % _value).str();
            }
            return previous;
        }
        std::string to_string() const{return (boost::format("{%1%}") % _to_string()).str();}
    };
    
    template <typename S, int N>
    struct assignment<S, char[N]>{
        typedef assignment<S, char[N]> self_type;
        
        S _previous;
        std::string _key;
        std::string _value;
        
        assignment(const S& previous, const std::string& key, const char* value): _previous(previous), _key(key), _value(value){}
        std::size_t length() const{return _previous.length()+1;}
        template <typename U>
        assignment<self_type, U> assign(const std::string& key, const U& value){
            return assignment<self_type, U>(*this, key, value);
        }
        std::string _to_string() const{
            std::string previous = _previous._to_string();
            if(previous.empty()){
                previous += (boost::format("%1%: %2%") % _key % _value).str();
            }else{
                previous += (boost::format(", %1%: %2%") % _key % _value).str();
            }
            return previous;
        }
        std::string to_string() const{return (boost::format("{%1%}") % _to_string()).str();}
    };
    
    template <typename S>
    struct assignment<S, std::string>{
        typedef assignment<S, std::string> self_type;
        
        S _previous;
        std::string _key;
        std::string _value;
        
        assignment(const S& previous, const std::string& key, const std::string& value): _previous(previous), _key(key), _value(value){}
        std::size_t length() const{return _previous.length()+1;}
        template <typename U>
        assignment<self_type, U> assign(const std::string& key, const U& value){
            return assignment<self_type, U>(*this, key, value);
        }
        std::string _to_string() const{
            std::string previous = _previous._to_string();
            if(previous.empty()){
                previous += (boost::format("%1%: \"%2%\"") % _key % _value).str();
            }else{
                previous += (boost::format(", %1%: \"%2%\"") % _key % _value).str();
            }
            return previous;
        }
        std::string to_string() const{return (boost::format("{%1%}") % _to_string()).str();}
    };
    
    template <typename S>
    struct assignment<S, std::nullptr_t>{
        typedef assignment<S, std::nullptr_t> self_type;
        
        S _previous;
        std::string _key;
        
        assignment(const S& previous, const std::string& key, std::nullptr_t): _previous(previous), _key(key){}
        std::size_t length() const{return _previous.length()+1;}
        template <typename U>
        assignment<self_type, U> assign(const std::string& key, const U& value){
            return assignment<self_type, U>(*this, key, value);
        }
        std::string _to_string() const{
            std::string previous = _previous._to_string();
            if(previous.empty()){
                previous += (boost::format("%1%: null") % _key).str();
            }else{
                previous += (boost::format(", %1%: null") % _key).str();
            }
            return previous;
        }
        std::string to_string() const{return (boost::format("{%1%}") % _to_string()).str();}
    };
    
    template <typename T>
    assignment<null_assignment, T> assign(const std::string& key, const T& value){
        return assignment<null_assignment, T>(null_assignment(), key, value);
    }
    
    template <typename U, typename V>
    std::ostream& operator<<(std::ostream& stream, const assignment<U, V>& asgn){
        stream << asgn.to_string();
        return stream;
    }
    
    struct for_action{
        static std::string label(){return "FOR";}
        static std::string preposition(){return "IN";}
    };
    struct insert_action{
        static std::string label(){return "INSERT";}
        static std::string preposition(){return "INTO";}
    };
    struct update_action{
        static std::string label(){return "UPDATE";}
        static std::string preposition(){return "IN";}
    };
    struct replace_action{
        static std::string label(){return "REPLACE";}
        static std::string preposition(){return "IN";}
    };
    struct collect_action{
        static std::string label(){return "COLLECT";}
        static std::string preposition(){return "INTO";}
    };
    struct erase_action{
        static std::string label(){return "REMOVE";}
        static std::string preposition(){return "IN";}
    };
    
    enum class direction{
        any,
        inbound,
        outbound
    };
    
    template <typename ElementT, typename DomainT, typename ActionT, typename WithT=nlohmann::json>
    struct domain_in {
        typedef domain_in<ElementT, DomainT, ActionT, WithT> self_type;
        ElementT      _element;
        DomainT       _domain;
        WithT         _with;
        std::map<std::string, direction> _edges;
        std::pair<int, int> _depth;
        
        domain_in(const ElementT& element, const DomainT& domain): _element(element), _domain(domain){}
        domain_in(const ElementT& element, const DomainT& domain, const WithT& with): _element(element), _domain(domain), _with(with){}
        std::string to_string() const {
            if(_edges.empty()){
                return (boost::format("%1% %2% WITH %5% %3% %4%") % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string() % _with.to_string()).str();
            }else{
                std::string domain;
                int lc = 0;
                for(auto e: _edges){
                    if(lc != 0){
                        domain += ", ";
                    }
                    std::string direction_str;
                    if(e.second == direction::any){
                        direction_str = "ANY";
                    }else if(e.second == direction::outbound){
                        direction_str = "OUTBOUND";
                    }else{
                        direction_str = "INBOUND";
                    }
                    domain += (boost::format("%1% %2%") % direction_str % e.first).str();
                    ++lc;
                }
                std::string depth_str;
                if(_depth.first == _depth.second && _depth.first > 0){
                    depth_str = (boost::format("%1%") % _depth.first).str();
                }else if(_depth.first > 0 && _depth.second > 0){
                    depth_str = (boost::format("%1%..%2%") % _depth.first % _depth.second).str();
                }
                if(_domain.to_string().find("/") == std::string::npos){
                    return (boost::format("WITH %1% %2% %3% %4% %7% ANY %5% %6%") % _with.to_string() % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string() % domain % depth_str).str();
                }else{
                    return (boost::format("WITH %1% %2% %3% %4% %7% ANY \"%5%\" %6%") % _with.to_string() % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string() % domain % depth_str).str();
                }
            }
        }
        self_type& outbound(const std::string& name){
            _edges.insert(std::make_pair(name, direction::outbound));
            return *this;
        }
        self_type& inbound(const std::string& name){
            _edges.insert(std::make_pair(name, direction::inbound));
            return *this;
        }
        self_type& any(const std::string& name){
            _edges.insert(std::make_pair(name, direction::any));
            return *this;
        }
        self_type& depth(int d){
            _depth = std::make_pair(d, d);
            return *this;
        }
        self_type& depth(int a, int b){
            _depth = std::make_pair(a, b);
            return *this;
        }
    };
    
    template <typename ElementT, typename DomainT, typename ActionT>
    struct domain_in<ElementT, DomainT, ActionT, nlohmann::json>{
        typedef domain_in<ElementT, DomainT, ActionT, nlohmann::json> self_type;
        ElementT       _element;
        DomainT        _domain;
        nlohmann::json _with;
        std::map<std::string, direction> _edges;
        std::pair<int, int> _depth;
        
        domain_in(const ElementT& element, const DomainT& domain): _element(element), _domain(domain){}
        domain_in(const ElementT& element, const DomainT& domain, const nlohmann::json& with): _element(element), _domain(domain), _with(with){}
        std::string to_string() const{
            if(_edges.empty()){
                if(_with.empty()){
                    return (boost::format("%1% %2% %3% %4%") % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string()).str();
                }else{
                    return (boost::format("%1% %2% WITH %5% %3% %4%") % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string() % (_with.is_string() ? _with.get<std::string>() : _with.dump())).str();
                }
            }else{
                std::string domain;
                int lc = 0;
                for(auto e: _edges){
                    if(lc != 0){
                        domain += ", ";
                    }
                    std::string direction_str;
                    if(e.second == direction::any){
                        direction_str = "ANY";
                    }else if(e.second == direction::outbound){
                        direction_str = "OUTBOUND";
                    }else{
                        direction_str = "INBOUND";
                    }
                    domain += (boost::format("%1% %2% ") % direction_str % e.first).str();
                    ++lc;
                }
                std::string depth_str;
                if(_depth.first == _depth.second && _depth.first > 0){
                    depth_str = (boost::format("%1%") % _depth.first).str();
                }else if(_depth.first > 0 && _depth.second > 0){
                    depth_str = (boost::format("%1%..%2%") % _depth.first % _depth.second).str();
                }
                if(_domain.to_string().find("/") == std::string::npos){
                    return (boost::format("%1% %2% %3% %6% ANY %4% %5%") % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string() % domain % depth_str).str();
                }else{
                    return (boost::format("%1% %2% %3% %6% ANY \"%4%\" %5%") % ActionT::label() % _element.to_string() % ActionT::preposition() % _domain.to_string() % domain % depth_str).str();
                }
            }
        }
        self_type& outbound(const std::string& name){
            _edges.insert(std::make_pair(name, direction::outbound));
            return *this;
        }
        self_type& inbound(const std::string& name){
            _edges.insert(std::make_pair(name, direction::inbound));
            return *this;
        }
        self_type& any(const std::string& name){
            _edges.insert(std::make_pair(name, direction::any));
            return *this;
        }
        self_type& depth(int d){
            _depth = std::make_pair(d, d);
            return *this;
        }
        self_type& depth(int a, int b){
            _depth = std::make_pair(a, b);
            return *this;
        }
    };
    
    template <typename ElementT, typename DomainT, typename ActionT, typename WithT>
    std::ostream& operator<<(std::ostream& stream, const domain_in<ElementT, DomainT, ActionT, WithT>& query){
        stream << query.to_string();
        return stream;
    }
    
    template <typename U, typename V>
    struct domain_assignment {
        assignment<U, V> _assignment;
        
        domain_assignment(const assignment<U, V>& assgnmnt): _assignment(assgnmnt){}
        std::string to_string() const {return _assignment.to_string();}
    };
    
    struct domain_collection {
        std::string _name;
        
        domain_collection(const std::string& name): _name(name){}
        std::string to_string() const {return _name;}
    };
    
    struct domain_range {
        std::pair<int, int> _range;
        
        domain_range(std::pair<int, int> range): _range(range){}
        domain_range(int from, int to): _range(std::make_pair(from, to)){}
        std::string to_string() const {return (boost::format("%1%..%2%") % _range.first % _range.second).str();}
    };
    
    struct domain_json {
        nlohmann::json _json;
        
        domain_json(const nlohmann::json& json): _json(json){}
        std::string to_string() const {return _json.dump();}
    };
    
    template <typename ElementT, typename ActionT, typename WithT>
    struct secondary_proxy_with{
        ElementT _element;
        WithT _with;
        
        secondary_proxy_with(const ElementT& element, const WithT& with): _element(element), _with(with){}
        
        template <typename DomainT>
        domain_in<ElementT, DomainT, ActionT, WithT> _in(const DomainT& domain) const{
            return domain_in<ElementT, DomainT, ActionT, WithT>(_element, domain, _with);
        }
        domain_in<ElementT, domain_collection, ActionT, WithT> in(const char* name) const{
            return in(std::string(name));
        }
        domain_in<ElementT, domain_collection, ActionT, WithT> in(const std::string& name) const{
            return _in(domain_collection(name));
        }
        domain_in<ElementT, domain_range, ActionT, WithT> in(std::pair<int, int> range) const{
            return _in(domain_range(range));
        }
        domain_in<ElementT, domain_range, ActionT, WithT> in(int from, int to) const{
            return _in(domain_range(from, to));
        }
        domain_in<ElementT, domain_json, ActionT, WithT> in(const nlohmann::json& json) const{
            return _in(domain_json(json));
        }
    };
    
    template <typename ElementT, typename ActionT>
    struct secondary_proxy{
        ElementT _element;
        nlohmann::json _with;
        
        secondary_proxy(const ElementT& element): _element(element){}
        
        template <typename DomainT>
        domain_in<ElementT, DomainT, ActionT> _in(const DomainT& domain) const{
            return domain_in<ElementT, DomainT, ActionT>(_element, domain, _with);
        }
        domain_in<ElementT, domain_collection, ActionT> in(const char* name) const{
            return in(std::string(name));
        }
        domain_in<ElementT, domain_collection, ActionT> in(const std::string& name) const{
            return _in(domain_collection(name));
        }
        domain_in<ElementT, domain_range, ActionT> in(std::pair<int, int> range) const{
            return _in(domain_range(range));
        }
        domain_in<ElementT, domain_range, ActionT> in(int from, int to) const{
            return _in(domain_range(from, to));
        }
        domain_in<ElementT, domain_json, ActionT> in(const nlohmann::json& json) const{
            return _in(domain_json(json));
        }
        secondary_proxy<ElementT, ActionT>& with(const nlohmann::json& json){
            _with = json;
            return *this;
        }
        template <typename U, typename V>
        secondary_proxy_with<ElementT, ActionT, assignment<U, V>> with(const assignment<U, V>& asgn){
            return secondary_proxy_with<ElementT, ActionT, assignment<U, V>>(_element, asgn);
        }
    };
    
    template <typename ActionT>
    struct primary_proxy{
        template <typename ElementT>
        static secondary_proxy<ElementT, ActionT> secondary(const ElementT& element){
            return secondary_proxy<ElementT, ActionT>(element);
        }
    };
    
    template <typename ActionT>
    secondary_proxy<domain_collection, ActionT> primary(const std::string& element){
        return secondary_proxy<domain_collection, ActionT>(domain_collection(element));
    }
    template <typename ActionT>
    secondary_proxy<domain_collection, ActionT> primary(const char* element){
        return primary<ActionT>(std::string(element));
    }
    template <typename ActionT>
    secondary_proxy<domain_range, ActionT> primary(std::pair<int, int> range){
        return secondary_proxy<domain_range, ActionT>(domain_range(range));
    }
    template <typename ActionT>
    secondary_proxy<domain_range, ActionT> primary(int from, int to){
        return primary<ActionT>(std::make_pair(from, to));
    }
    template <typename ActionT>
    secondary_proxy<domain_json, ActionT> primary(const nlohmann::json& json){
        return secondary_proxy<domain_json, ActionT>(domain_json(json));
    }
    template <typename ActionT, typename U, typename V>
    secondary_proxy<domain_assignment<U, V>, ActionT> primary(const assignment<U, V>& element){
        return secondary_proxy<domain_assignment<U, V>, ActionT>(domain_assignment<U, V>(element));
    }
    
    template <typename... T>
    auto select(const T&... elems){
        return primary<for_action>(elems...);
    }
    template <typename... T>
    auto update(const T&... elems){
        return primary<update_action>(elems...);
    }
    template <typename... T>
    auto insert(const T&... elems){
        return primary<insert_action>(elems...);
    }
    template <typename... T>
    auto replace(const T&... elems){
        return primary<replace_action>(elems...);
    }
    template <typename... T>
    auto collect(const T&... elems){
        return primary<collect_action>(elems...);
    }
    secondary_proxy<domain_collection, collect_action> collect();
    template <typename... T>
    auto erase(const T&... elems){
        return primary<erase_action>(elems...);
    }
    
    template <typename T>
    struct let_declaration {
        std::string _lhs;
        T _rhs;
        
        let_declaration(const std::string& lhs, const T& rhs): _lhs(lhs), _rhs(rhs){}
        std::string to_string() const{
            return (boost::format("LET %1% = (%2%)") % _lhs % _rhs).str();
        }
    };
    
    template <>
    struct let_declaration<std::string>{
        std::string _lhs;
        std::string _rhs;
        
        let_declaration(const std::string& lhs, const std::string& rhs): _lhs(lhs), _rhs(rhs){}
        std::string to_string() const{
            return (boost::format("LET %1% = \"%2%\"") % _lhs % _rhs).str();
        }
    };
    
    template <>
    struct let_declaration<const char*>{
        std::string _lhs;
        std::string _rhs;
        
        let_declaration(const std::string& lhs, const char* rhs): _lhs(lhs), _rhs(rhs){}
        std::string to_string() const{
            return (boost::format("LET %1% = %2%") % _lhs % _rhs).str();
        }
    };
    
    template <int N>
    struct let_declaration<char[N]>{
        std::string _lhs;
        std::string _rhs;
        
        let_declaration(const std::string& lhs, const char* rhs): _lhs(lhs), _rhs(rhs){}
        std::string to_string() const{
            return (boost::format("LET %1% = %2%") % _lhs % _rhs).str();
        }
    };
    
    struct let_declaration_proxy{
        std::string _lhs;
        
        let_declaration_proxy(const std::string& lhs): _lhs(lhs){}
        template <typename T>
        let_declaration<T> operator=(const T& rhs){
            return let_declaration<T>(_lhs, rhs);
        }
    };
    
    let_declaration_proxy let(const std::string& lhs);

    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const let_declaration<T>& declaration){
        stream << declaration.to_string();
        return stream;
    }
    
    template <typename T>
    struct ret_value{
        T _retval;
        
        ret_value(const T& val): _retval(val){}
        std::string to_string() const{
            return (boost::format("RETURN %1%") % _retval).str();
        }
    };
    
    template <int N>
    struct ret_value<char[N]>: ret_value<std::string>{
        ret_value(const char* val): ret_value<std::string>(val){}
    };
    
    template <typename T>
    struct ret_value<std::map<std::string, T>>{
        std::map<std::string, T> _retval;
        
        ret_value(const std::map<std::string, T>& val): _retval(val){}
        std::string to_string() const{
            std::string str = "RETURN {";
            int i = 0;
            for(auto pair: _retval){
                if(i != 0){
                    str += (boost::format(", %1%: %2%") % pair.first % pair.second).str();
                }else{
                    str += (boost::format("%1%: %2%") % pair.first % pair.second).str();
                }
                ++i;
            }
            str += "}";
            return str;
        }
    };
    
    template <typename U, typename V>
    struct ret_value<assignment<U, V>>{
        assignment<U, V> _retval;
        
        ret_value(const assignment<U, V>& val): _retval(val){}
        std::string to_string() const{
            return "RETURN "+ _retval.to_string();
        }
    };
    
    template <typename T>
    ret_value<T> yield(const T& val){
        return ret_value<T>(val);
    }
    
    template <typename T>
    struct upsert_{
        struct upsert_update{
            std::string _in;
            T _value;
            upsert_<T>& _master;
            
            upsert_update(upsert_<T>& master, T val): _master(master), _value(val){}
            upsert_<T>& in(std::string collection){
                _in = collection;
                return _master;
            }
        };
        struct upsert_insert{
            T _value;
            upsert_<T>& _master;
            upsert_update  _update;
            
            upsert_insert(upsert_<T>& master, T val): _master(master), _value(val), _update(_master, val){}
            upsert_update& update(nlohmann::json value){
                _update._value = value;
                return _update;
            }
        };
        
        T _search;
        upsert_insert  _insert;
        
        upsert_(T search): _search(search), _insert(*this, search){}
        upsert_insert& insert(T value){
            _insert._value = value;
            return _insert;
        }
//         void in(std::string collection){
//             _insert._update._in = collection;
//         }
        std::string to_string() const{
            return (boost::format("UPSERT %1% INSERT %2% UPDATE %3% IN %4%") % _search % _insert._value % _insert._update._value % _insert._update._in).str();
        }
    };
    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const upsert_<T>& upsert_val){
        stream << upsert_val.to_string();
        return stream;
    }
    template <typename T>
    upsert_<T> upsert(T value){
        return upsert_<T>(value);
    }
    
    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const ret_value<T>& retval){
        stream << retval.to_string();
        return stream;
    }
    
    
    template <typename T>
    struct options_declaration{};
    
    template <>
    struct options_declaration<nlohmann::json>{
        nlohmann::json _store;
        
        options_declaration(const nlohmann::json& store): _store(store){}
        std::string to_string() const{return "OPTIONS "+_store.dump();}
    };
    
    template <typename U, typename V>
    struct options_declaration<assignment<U, V>>{
        assignment<U, V> _store;
        
        options_declaration(const assignment<U, V>& store): _store(store){}
        std::string to_string() const{return "OPTIONS "+_store.to_string();}
    };
    
    template <typename T>
    options_declaration<T> options(const T& store){
        return options_declaration<T>(store);
    }
    
    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const options_declaration<T>& store){
        stream << store.to_string();
        return stream;
    }
    
    struct sort{
        std::map<std::string, bool> _fields; // true is ASC, false is DESC
        
        sort& asc(const std::string& field){
            _fields.insert(std::make_pair(field, true));
            return *this;
        }
        sort& desc(const std::string& field){
            _fields.insert(std::make_pair(field, false));
            return *this;
        }
        
        std::string to_string() const{
            std::string str = "SORT ";
            for(auto pair: _fields){
                str += (boost::format("%1% %2% ") % pair.first % (pair.second ? std::string("ASC") : std::string("DESC"))).str();
            }
            return str;
        }
    };
    
    std::ostream& operator<<(std::ostream& stream, const sort& fields);
}

#endif // ARANGOPP_QUERY_H
