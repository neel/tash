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

#include "tash/filter.h"
#include <boost/format.hpp>

tash::junction::junction(tash::junction::junction_type type, const std::string& op): _type(type), _operator(op){}
tash::compound_junction::compound_junction(const std::string& op, junction::base_ptr left, junction::base_ptr right): tash::junction(tash::junction::compound, op), _left(left), _right(right){}
tash::mixed_junction::mixed_junction(const std::string& op, junction::base_ptr left, const tash::clause& right): tash::junction(tash::junction::mixed, op), _left(left), _right(right){}
tash::leaf_junction::leaf_junction(const std::string& op, const tash::clause& left, const tash::clause& right): tash::junction(tash::junction::leaf, op), _left(left), _right(right){}

tash::junction::base_ptr tash::compound_junction::clone() const{return tash::junction::base_ptr(new tash::compound_junction(*this));}
tash::junction::base_ptr tash::mixed_junction::clone() const{return tash::junction::base_ptr(new tash::mixed_junction(*this));}
tash::junction::base_ptr tash::leaf_junction::clone() const{return tash::junction::base_ptr(new tash::leaf_junction(*this));}

tash::compound_junction tash::operator&&(const tash::junction& left, const tash::junction& right){return tash::compound_junction("AND", left.clone(), right.clone());}
tash::mixed_junction tash::operator&&(const tash::junction& left, const tash::clause& right){return tash::mixed_junction("AND", left.clone(), right);}
tash::mixed_junction tash::operator&&(const tash::clause& left, const tash::junction& right){return tash::operator&&(right, left);}
tash::leaf_junction tash::operator&&(const tash::clause& left, const tash::clause& right){return tash::leaf_junction("AND", left, right);}

tash::compound_junction tash::operator||(const tash::junction& left, const tash::junction& right){return tash::compound_junction("OR", left.clone(), right.clone());}
tash::mixed_junction tash::operator||(const tash::junction& left, const tash::clause& right){return tash::mixed_junction("OR", left.clone(), right);}
tash::mixed_junction tash::operator||(const tash::clause& left, const tash::junction& right){return tash::operator||(right, left);}
tash::leaf_junction tash::operator||(const tash::clause& left, const tash::clause& right){return tash::leaf_junction("OR", left, right);}

std::string tash::clause::to_string() const{return (boost::format("%1% %2% %3%") % _key % _operator % (_quoted ? "\""+_value+"\"" : _value)).str();}
std::string tash::compound_junction::to_string() const{return (boost::format("(%1% %2% %3%)") % _left->to_string() % _operator % _right->to_string()).str();}
std::string tash::mixed_junction::to_string() const{return (boost::format("(%1% %2% %3%)") % _left->to_string() % _operator % _right.to_string()).str();}
std::string tash::leaf_junction::to_string() const{return (boost::format("%1% %2% %3%") % _left.to_string() % _operator % _right.to_string()).str();}

std::ostream& tash::operator<<(std::ostream& stream, const tash::junction& jnc){
    stream << jnc.to_string();
    return stream;
}
