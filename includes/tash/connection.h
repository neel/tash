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


#ifndef ARANGOPP_CONNECTION_H
#define ARANGOPP_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include "filter.h"

namespace tash{
    typedef boost::beast::http::request<boost::beast::http::string_body>    http_request_type;
    typedef boost::beast::http::response<boost::beast::http::dynamic_body>   http_response_type;
    
    class connection;
    class shell;
    class collection;
    class cursor;
    
    class cursor{
        friend class connection;
        friend class shell;
        
        mutable boost::mutex _mutex;
        connection&     _conn;
        std::string     _id;
        bool            _has_more;
        bool            _error;
        int             _count;
        int             _code;
        nlohmann::json  _results;
      public:
        cursor(connection& conn, const std::string& id);
        cursor(const cursor& other);
        void fetch();
        bool has_more() const{return _has_more;}
        bool error() const{return _error;}
        int count() const{return _count;}
        int code() const {return _code;}
        nlohmann::json results() const;
      private:
        cursor(connection& conn);
        void attach(const http_response_type& response);
      public:
        ~cursor();
    };
    
    class connection: boost::noncopyable{
      private:
        boost::mutex                    _mutex;
        boost::asio::io_context         _io;
        boost::asio::ip::tcp::resolver  _resolver;

        std::string _host;
        unsigned    _port;
        std::string _user;
        std::string _pass;
        std::string _db;
      public:
        connection(const std::string& db, std::string host = "localhost", unsigned port = 8529, std::string user = "root", std::string pass = "");
        std::string host() const{return _host;}
        unsigned    port() const{return _port;}
        std::string username() const{return _user;}
        std::string password() const{return _pass;}
        std::string database() const{return _db;}
        
        http_request_type url(boost::beast::http::verb method = boost::beast::http::verb::get, std::string path = "") const;
        http_request_type db_url(boost::beast::http::verb method = boost::beast::http::verb::get, std::string path = "") const;
        
        boost::beast::http::status exists();
        boost::beast::http::status create();
        
        http_response_type query(const http_request_type& request);
        http_response_type query(std::string path, boost::beast::http::verb method = boost::beast::http::verb::get);
        http_response_type query(std::string path, const std::string& content, std::string type = "application/json", boost::beast::http::verb method = boost::beast::http::verb::post);
        cursor aql(const std::string& q, int count = 0, int batch = 0);
        template <typename AqlT>
        cursor aql(const AqlT& q, int count = 0, int batch = 0){
            std::string query = boost::lexical_cast<std::string>(q);
            return aql(query, count, batch);
        }
    };
    
    class shell: public connection{
      boost::shared_ptr<cursor> _last;
      public:
        shell(const std::string& db, std::string host = "localhost", unsigned port = 8529, std::string user = "root", std::string pass = "");
      public:
        bool has_more() const {return _last->has_more();}
        bool is_error() const {return _last->error();}
        int count() const {return _last->count();}
        int code() const {return _last->code();}
        nlohmann::json results() const{return _last->results();}
      public:
        template <typename AqlT>
        shell& run(const AqlT& query){
            _last = boost::make_shared<cursor>(aql(query));
            return *this;
        }
        template <typename AqlT>
        shell& operator<<(const AqlT& query){
            return run(query);
        }
        shell& operator>>(nlohmann::json& res);
    };
    
    class collection{
      public:
        enum class type{
            vertex = 2,
            edge   = 3
        };
      private:
        connection& _conn;
        std::string _name;
        type        _type;
      public:
        collection(connection& conn, const std::string& name, type t);
        std::string name() const{return _name;}
        type collection_type() const{return _type;}
        bool is_vertex() const {return _type == type::vertex;}
        bool is_edge() const {return _type == type::edge;}
        boost::beast::http::status exists();
        boost::beast::http::status create();
        boost::beast::http::status add(const nlohmann::json& document);
        boost::beast::http::status add(nlohmann::json& document);
        http_response_type update(const std::string key, const nlohmann::json& document);
        http_response_type replace(const std::string key, const nlohmann::json& document);
        http_response_type read(const std::string key);
        http_response_type read(int limit = 0, int skip = 0);
      protected:
        nlohmann::json by_id(const std::string& id);
        nlohmann::json by_key(const std::string& key);
    };
    
    struct vertex: public collection{
        using collection::by_key;
        vertex(connection& conn, const std::string& name): collection(conn, name, collection::type::vertex){}
    };
    struct edge: public collection{
        using collection::add;
        using collection::by_key;
        edge(connection& conn, const std::string& name): collection(conn, name, collection::type::edge){}
        boost::beast::http::status add(const std::string& from, const std::string& to, nlohmann::json& document);
        boost::beast::http::status add(const std::string& from, const std::string& to, const nlohmann::json& document);
        boost::beast::http::status add(const std::string& from, const std::string& to);
    };
}

#endif // ARANGOPP_ARANGO_H
