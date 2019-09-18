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

#include "tokai/filter.h"
#include <boost/format.hpp>

tokai::junction::junction(tokai::junction::junction_type type, const std::string& op): _type(type), _operator(op){}
tokai::compound_junction::compound_junction(const std::string& op, junction::base_ptr left, junction::base_ptr right): tokai::junction(tokai::junction::compound, op), _left(left), _right(right){}
tokai::mixed_junction::mixed_junction(const std::string& op, junction::base_ptr left, const tokai::clause& right): tokai::junction(tokai::junction::mixed, op), _left(left), _right(right){}
tokai::leaf_junction::leaf_junction(const std::string& op, const tokai::clause& left, const tokai::clause& right): tokai::junction(tokai::junction::leaf, op), _left(left), _right(right){}

tokai::junction::base_ptr tokai::compound_junction::clone() const{return tokai::junction::base_ptr(new tokai::compound_junction(*this));}
tokai::junction::base_ptr tokai::mixed_junction::clone() const{return tokai::junction::base_ptr(new tokai::mixed_junction(*this));}
tokai::junction::base_ptr tokai::leaf_junction::clone() const{return tokai::junction::base_ptr(new tokai::leaf_junction(*this));}

tokai::compound_junction tokai::operator&&(const tokai::junction& left, const tokai::junction& right){return tokai::compound_junction("AND", left.clone(), right.clone());}
tokai::mixed_junction tokai::operator&&(const tokai::junction& left, const tokai::clause& right){return tokai::mixed_junction("AND", left.clone(), right);}
tokai::mixed_junction tokai::operator&&(const tokai::clause& left, const tokai::junction& right){return tokai::operator&&(right, left);}
tokai::leaf_junction tokai::operator&&(const tokai::clause& left, const tokai::clause& right){return tokai::leaf_junction("AND", left, right);}

tokai::compound_junction tokai::operator||(const tokai::junction& left, const tokai::junction& right){return tokai::compound_junction("OR", left.clone(), right.clone());}
tokai::mixed_junction tokai::operator||(const tokai::junction& left, const tokai::clause& right){return tokai::mixed_junction("OR", left.clone(), right);}
tokai::mixed_junction tokai::operator||(const tokai::clause& left, const tokai::junction& right){return tokai::operator||(right, left);}
tokai::leaf_junction tokai::operator||(const tokai::clause& left, const tokai::clause& right){return tokai::leaf_junction("OR", left, right);}

std::string tokai::clause::to_string() const{return (boost::format("%1% %2% %3%") % _key % _operator % (_quoted ? "\""+_value+"\"" : _value)).str();}
std::string tokai::compound_junction::to_string() const{return (boost::format("(%1% %2% %3%)") % _left->to_string() % _operator % _right->to_string()).str();}
std::string tokai::mixed_junction::to_string() const{return (boost::format("(%1% %2% %3%)") % _left->to_string() % _operator % _right.to_string()).str();}
std::string tokai::leaf_junction::to_string() const{return (boost::format("%1% %2% %3%") % _left.to_string() % _operator % _right.to_string()).str();}

std::ostream& tokai::operator<<(std::ostream& stream, const tokai::junction& jnc){
    stream << jnc.to_string();
    return stream;
}
