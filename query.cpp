#include "tash/query.h"

tash::secondary_proxy<tash::domain_collection, tash::collect_action> tash::collect(){
    return tash::primary<tash::collect_action>("");
}


tash::let_declaration_proxy tash::let(const std::string& lhs){
    return let_declaration_proxy(lhs);
}

std::ostream& tash::operator<<(std::ostream& stream, const tash::sort& fields){
    stream << fields.to_string();
    return stream;
}

