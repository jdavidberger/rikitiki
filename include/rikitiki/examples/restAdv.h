/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/rikitiki>
#include <rikitiki/mongoose/mongoose>
#include <rikitiki/rest/rest>
#include <rikitiki/jsoncpp/jsoncpp>
#include <rikitiki/configuration/configuration>
#include <sqlite3.h>

using namespace rikitiki;
using namespace rikitiki::mongoose;

namespace rikitiki {
  struct Book {
    std::string name; 
    std::string author;
    std::string isbn;
  };
  
  static Json::Value& operator >>(Json::Value& jv, Book& b){
    b.name = jv["name"].asString();
    b.author = jv["author"].asString();
    b.isbn = jv["isbn"].asString();
    return jv;
  }  

  static Json::Value& operator <<(Json::Value& jv, Book& b){
    jv["name"] = b.name;
    jv["author"] = b.author;
    jv["isbn"] = b.isbn;
    return jv;
  }  

  template <>
    struct valid_conversions<Book> {
    typedef TypeConversions<Book, InProvider, Json::Value> In;
    typedef TypeConversions<Book, OutProvider, Json::Value> Out;
  };

  namespace examples {
    /**
       Example implementation of a REST module against sqlite. 
       Demonstrates
       - Rest Features
       - Json Features
    */    
    struct RestAdvModule  {      
      std::vector<Book> books; 
      /// Post operations add items to the DB
      void POST(ConnContext& ctx){
	Book newBook;
	ctx >> newBook;
	books.push_back(newBook);
	ctx << books.size() - 1;
      }

      /// Get operations without an ID return a list of all books
      void GET(ConnContext& ctx){
	ctx << books;
      }

      void GET(ConnContext& ctx, int id){
	if(id < 0 || id >= (int)books.size())
	  ctx << HttpStatus::Bad_Request;
	else 
	  ctx << books[id];
      }

      void DELETE(ConnContext& ctx){
	books.clear();
	ctx.handled = true; // If we don't write anything to ctx, it doesn't count as handled. 
      }
      
      void Register(Server& server){
	typedef RestAdvModule T;
	rikitiki::rest::Register(server, "/book-adv", this);
	books.push_back(Book{"Test","This","thing"});
	books.push_back(Book{"A", "B", "C"});
      }
    };

  }
}
