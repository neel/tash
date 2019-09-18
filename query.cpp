#include "tokai/query.h"

tokai::secondary_proxy<tokai::domain_collection, tokai::collect_action> tokai::collect(){
    return tokai::primary<tokai::collect_action>("");
}


tokai::let_declaration_proxy tokai::let(const std::string& lhs){
    return let_declaration_proxy(lhs);
}

std::ostream& tokai::operator<<(std::ostream& stream, const tokai::sort& fields){
    stream << fields.to_string();
    return stream;
}

