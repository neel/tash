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
