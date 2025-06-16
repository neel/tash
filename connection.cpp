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


#include "tash/connection.h"
#include "tash/query.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/buffer.hpp>
#include <basen.hpp>

namespace http = boost::beast::http;

tash::connection::connection(const std::string& db, std::string host, unsigned int port, std::string user, std::string pass): _host(host), _port(port), _user(user), _pass(pass), _db(db), _resolver(_io){
    
}

tash::http_request_type tash::connection::url(boost::beast::http::verb method, std::string path) const{
    std::string b64_encoded_credentials;
    boost::trim_left_if(path, boost::is_any_of("/ "));
    tash::http_request_type request(method, "/"+path, 10);
    std::string credentials = (boost::format("%1%:%2%") % _user % _pass).str();
    bn::encode_b64(credentials.begin(), credentials.end(), back_inserter(b64_encoded_credentials));
    request.set(boost::beast::http::field::authorization, "Basic " + b64_encoded_credentials);
    return request;
}

tash::http_request_type tash::connection::db_url(boost::beast::http::verb method, std::string path) const{
    boost::trim_left_if(path, boost::is_any_of("/ "));
    return url(method, (boost::format("/_db/%1%/%2%") % _db % path).str());
}

tash::http_response_type tash::connection::query(const tash::http_request_type& request){
    boost::mutex::scoped_lock lock(_mutex);
    auto const results = _resolver.resolve(_host, boost::lexical_cast<std::string>(_port));
    boost::asio::ip::tcp::socket socket(_io);
    boost::asio::connect(socket, results.begin(), results.end());
    boost::beast::http::write(socket, request);
    boost::beast::flat_buffer buffer;
    tash::http_response_type res;
    boost::beast::http::read(socket, buffer, res);
    return res;
}

tash::http_response_type tash::connection::query(std::string path, boost::beast::http::verb method){
    tash::http_request_type request;
    if(boost::algorithm::starts_with(path, "/")){
        request = url(method, path);
    }else{
        request = db_url(method, path);
    }
    return query(request);
}

tash::http_response_type tash::connection::query(std::string path, const std::string& content, std::string type, boost::beast::http::verb method){
    tash::http_request_type request;
    if(boost::algorithm::starts_with(path, "/")){
        request = url(method, path);
    }else{
        request = db_url(method, path);
    }
    request.set(boost::beast::http::field::content_type, type);
    request.set(boost::beast::http::field::content_length, std::to_string(content.size()));
    request.body() = content;
    return query(request);
}

tash::cursor tash::connection::aql(const std::string& q, int count, int batch){
    nlohmann::json document{
        {"query", q}
    };
    if(count != 0){
        document["count"] = count;
    }
    if(batch != 0){
        document["batchSize"] = batch;
    }
    tash::http_response_type response = query("_api/cursor", document.dump(), "application/json", boost::beast::http::verb::post);
    if(response.result() == http::status::ok || response.result() == http::status::accepted || response.result() == http::status::created){/* No Operation */}
    else{
        std::cout << "AQL Failed: " << q << std::endl;
        std::cout << "result: " << response.result() << std::endl;
        std::cout << response << std::endl;
    }
    tash::cursor cursor(*this);
    cursor.attach(response);
    return cursor;
}

boost::beast::http::status tash::connection::exists() {
    return query("_api/version").result();
}

tash::cursor::cursor(tash::connection& conn, const std::string& id): _conn(conn), _id(id){}
tash::cursor::cursor(tash::connection& conn): _conn(conn), _id(std::string()){}
tash::cursor::cursor(const tash::cursor& other): _conn(other._conn), _id(other._id), _has_more(other._has_more), _error(other._error), _count(other._count), _code(other._code), _results(other._results){}
tash::cursor::~cursor(){
     boost::mutex::scoped_lock lock(_mutex);
}

nlohmann::json tash::cursor::results() const{
    boost::mutex::scoped_lock lock(_mutex);
    return _results;
}


void tash::cursor::fetch(){
    // https://github.com/boostorg/beast/issues/819
    tash::http_response_type response = _conn.query("_api/cursor/"+_id, "", "application/json", boost::beast::http::verb::put);
    attach(response);
}

void tash::cursor::attach(const tash::http_response_type& response){
    boost::mutex::scoped_lock lock(_mutex);
    nlohmann::json json = nlohmann::json::parse(boost::beast::buffers_to_string(response.body().data()));
    _has_more = json.value("hasMore", false);
    _error    = json.value("error",   false);
    _count    = json.value("count",   0);
    _code     = json.value("code",    0);
    _results  = json["result"];
    if(_id.empty()){
        _id = json.value("id", std::string());
    }
}

tash::shell::shell(const std::string& db, std::string host, unsigned int port, std::string user, std::string pass): connection(db, host, port, user, pass){}
tash::shell& tash::shell::operator>>(nlohmann::json& res){
    res = results();
    return *this;
}


tash::collection::collection(tash::connection& conn, const std::string& name, type t): _conn(conn), _name(name), _type(t){}

boost::beast::http::status tash::connection::create(){
    nlohmann::json document = {
        {"name", _db}
    };
    tash::http_response_type response = query("/_api/database", document.dump());
    return response.result();
}

boost::beast::http::status tash::collection::exists(){
    tash::http_response_type response = _conn.query("_api/collection/"+_name);
    return response.result();
}

boost::beast::http::status tash::collection::create(){
    nlohmann::json document = {
        {"name", _name},
        {"type", _type}
    };
    tash::http_response_type response = _conn.query("_api/collection", document.dump());
    return response.result();
}

boost::beast::http::status tash::collection::add(const nlohmann::json& document){
    tash::http_response_type response = _conn.query("_api/document/"+_name, document.dump());
    return response.result();
}

boost::beast::http::status tash::collection::add(nlohmann::json& document){
    tash::http_response_type response = _conn.query("_api/document/"+_name, document.dump());
    nlohmann::json json = nlohmann::json::parse(boost::beast::buffers_to_string(response.body().data()));
    document = json;
    return response.result();
}

tash::http_response_type tash::collection::update(const std::string key, const nlohmann::json& document){
    return _conn.query((boost::format("_api/document/%1%/%2%") % _name % key).str(), document.dump(), "application/json", boost::beast::http::verb::patch);
}

tash::http_response_type tash::collection::replace(const std::string key, const nlohmann::json& document){
    return _conn.query((boost::format("_api/document/%1%/%2%") % _name % key).str(), document.dump(), "application/json", boost::beast::http::verb::put);
}

tash::http_response_type tash::collection::read(const std::string key){
    return _conn.query((boost::format("_api/document/%1%/%2%") % _name % key).str());
}

tash::http_response_type tash::collection::read(int limit, int skip){
    nlohmann::json document{
        {"collection", _name}
    };
    if(limit != 0){
        document["limit"] = limit;
    }
    if(skip != 0){
        document["skip"] = skip;
    }
    return _conn.query("_api/simple/all", document.dump(), "application/json", boost::beast::http::verb::put);
}

boost::beast::http::status tash::edge::add(const std::string& from, const std::string& to, nlohmann::json& document){
    document["_from"] = from;
    document["_to"]   = to;
    return tash::collection::add(document);
}

boost::beast::http::status tash::edge::add(const std::string& from, const std::string& to, const nlohmann::json& document){
    nlohmann::json request(document);
    request["_from"] = from;
    request["_to"]   = to;
    return tash::collection::add(request);
}

boost::beast::http::status tash::edge::add(const std::string& from, const std::string& to){
    return add(from, to, nlohmann::json());
}


nlohmann::json tash::collection::by_id(const std::string& id){
    tash::http_response_type response = _conn.query("_api/document/"+id);
    if(response.result() == boost::beast::http::status::ok){
        nlohmann::json json = nlohmann::json::parse(boost::beast::buffers_to_string(response.body().data()));
        return json;
    }
    return nlohmann::json(nullptr);
}

nlohmann::json tash::collection::by_key(const std::string& key){
    return by_id((boost::format("%1%/%2%") % _name % key).str());
}
