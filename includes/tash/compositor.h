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


#ifndef ARANGOPP_COMPOSITOR_H
#define ARANGOPP_COMPOSITOR_H

#include <string>
#include <iostream>

namespace tash{
    
struct null_composition{
    std::string to_string() const {return "";}
};

template <typename U, typename V = null_composition>
struct composition{
    typedef U head_type;
    typedef V tail_type;
    
    U _head;
    V _tail;
    composition(const U& head, const V& tail): _head(head), _tail(tail){}
};

template <typename U, typename V, typename T>
composition<composition<U, V>, T> operator/(const composition<U, V>& head, const T& tail){
    typedef composition<composition<U, V>, T> composition_type;
    return composition_type(head, tail);
}

template <typename U, typename V>
composition<composition<U>, V> operator/(const U& head, const V& tail){
    composition<U> composition_head(head, null_composition());
    return composition<composition<U>, V>(composition_head, tail);
}

template <typename U, typename V>
struct serializer{
    static std::ostream& serialize(std::ostream& stream, const composition<U, V>& cmp){
        serializer<typename U::head_type, typename U::tail_type>::serialize(stream, cmp._head);
        stream << "\n\t";
        stream << cmp._tail;
        return stream;
    }
};

template <typename U>
struct serializer<U, null_composition>{
    static std::ostream& serialize(std::ostream& stream, const composition<U, null_composition>& cmp){
        stream << cmp._head;
        return stream;
    }
};

template <typename U, typename V>
static std::ostream& serialize(std::ostream& stream, const composition<U, V>& cmp){
    return serializer<U, V>::serialize(stream, cmp);
}

template <typename U, typename V>
std::ostream& operator<<(std::ostream& stream, const composition<U, V>& cmp){
    serialize(stream, cmp);
    return stream;
}

}

#endif // ARANGOPP_COMPOSITOR_H
