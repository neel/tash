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

#ifndef ARANGOPP_COMPOSITOR_H
#define ARANGOPP_COMPOSITOR_H

#include <string>
#include <iostream>

namespace tokai{
    
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
