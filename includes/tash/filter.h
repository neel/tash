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


#ifndef ARANGOPP_FILTER_H
#define ARANGOPP_FILTER_H

#include <string>
#include <nlohmann/json.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

/**
 * @todo write docs
 */
namespace tash{
    template <typename T>
    struct quote_policy{
        static constexpr bool quoted = false;
    };
    template <>
    struct quote_policy<std::string>{
        static constexpr bool quoted = true;
    };
//     template <>
//     struct quote_policy<const char*>: quote_policy<std::string>{};
//     template <int N>
//     struct quote_policy<const char[N]>: quote_policy<std::string>{};
    
    /**
     * clause("u.name") == "Neel"
     */
    struct clause{
        std::string _key;
        std::string _value;
        std::string _operator;
        bool        _quoted;
        
        clause(){}
        clause(const std::string& key): _key(key){}
        template <typename T>
        clause& _set(const std::string& op, T value){
            _value = boost::lexical_cast<std::string>(value);
            _quoted = quote_policy<T>::quoted;
            _operator = op;
            return *this;
        }
        template <typename T> clause& like(T value){return _set("LIKE", value);}
        template <typename T> clause& in(T value){return _set("IN", value);}
        template <typename T> clause& not_in(T value){return _set("NOT IN", value);}
        template <typename T> clause& operator==(T value){return _set("==", value);}
        template <typename T> clause& operator!=(T value){return _set("!=", value);}
        template <typename T> clause& operator< (T value){return _set("<",  value);}
        template <typename T> clause& operator> (T value){return _set(">",  value);}
        template <typename T> clause& operator<=(T value){return _set("<=", value);}
        template <typename T> clause& operator>=(T value){return _set(">=", value);}
        bool is_valid() const{return !_key.empty() && !_value.empty();}
        std::string to_string() const;
    };
    
    /**
     * (((clause("x") == "Hallo") && (clause("y") <= 3) || (clause("z") >= 4)) && (clause("p") >= 8 || clause("q") != "world") || clause("t") < 100)
     */
    struct junction{
        typedef boost::shared_ptr<junction> base_ptr;
        enum junction_type{
            unknown,
            compound,
            mixed,
            leaf
        };
        std::string   _operator;
        junction_type _type;
        
        junction(junction_type type, const std::string& op);
        virtual base_ptr clone() const = 0;
        virtual std::string to_string() const = 0;
    };
    
    struct compound_junction: public junction{
        junction::base_ptr _left;
        junction::base_ptr _right;
        
        compound_junction(const std::string& op, junction::base_ptr left, junction::base_ptr right);
        tash::junction::base_ptr clone() const override;
        std::string to_string() const override;
    };
    
    struct mixed_junction: public junction{
        junction::base_ptr  _left;
        clause              _right;
        
        mixed_junction(const std::string& op, junction::base_ptr left, const clause& right);
        tash::junction::base_ptr clone() const override;
        std::string to_string() const override;
    };
    
    struct leaf_junction: public junction{
        clause _left;
        clause _right;
        
        leaf_junction(const std::string& op, const clause& left, const clause& right);
        tash::junction::base_ptr clone() const override;
        std::string to_string() const override;
    };
    
    leaf_junction     operator&&(const clause& left,   const clause& right);
    mixed_junction    operator&&(const junction& left, const clause& right);
    mixed_junction    operator&&(const clause& left,   const junction& right);
    compound_junction operator&&(const junction& left, const junction& right);
    leaf_junction     operator||(const clause& left,   const clause& right);
    mixed_junction    operator||(const junction& left, const clause& right);
    mixed_junction    operator||(const clause& left,   const junction& right);
    compound_junction operator||(const junction& left, const junction& right);
    
    template <typename T>
    struct filter_compound {
        T _compound;
        
        filter_compound(const T& compound): _compound(compound){}
        std::string to_string() const{
            return (boost::format("FILTER %1%") % _compound.to_string()).str();
        }
    };
    
    template <typename T>
    filter_compound<T> filter(const T& compound){
        return filter_compound<T>(compound);
    }
    
    std::ostream& operator<<(std::ostream& stream, const junction& jnc);
    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const filter_compound<T>& fltr){
        stream << fltr.to_string();
        return stream;
    }
}

#endif // ARANGOPP_FILTER_H
