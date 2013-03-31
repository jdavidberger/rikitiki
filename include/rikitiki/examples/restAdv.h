/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/rikitiki>
#include <rikitiki/mongoose/mongoose>
#include <rikitiki/rest/rest>
#include <rikitiki/jsoncpp/jsoncpp>
#include <rikitiki/configuration/configuration>
#include <tuple>
#include <sqlite3.h>
#include <mxcomp/tuples.h>
#include <cxxabi.h>
using namespace rikitiki;
using namespace rikitiki::mongoose;


  inline static std::string prettyName(const std::type_info& ti){
    char buf[1024];
    size_t size=1024;
    int status;
    char* res = abi::__cxa_demangle (ti.name(),
				     buf,
				     &size,
				     &status);
    return std::string(res);
  }

namespace rikitiki {
  struct Book {

    std::string name;
    std::string author;
    std::string isbn;
    Book() {}
  Book(const std::string& _name, const std::string& _author, const std::string& _isbn) :
    name(_name), author(_author), isbn(_isbn) {}
  };

}

namespace mxcomp {
  METACLASS(Book){
    MEMBERS(FIELD(name),
	    FIELD(author),
	    FIELD(isbn));
  };
}
namespace rikitiki {  
  namespace examples {
      struct GetType {
	template <typename T> 
	auto operator()(const T& t) const -> typename std::enable_if<std::is_base_of<Field, T>::value, std::string>::type
	{ return prettyName(typeid(T)); }

	template <typename T> 
	auto operator()(const T& t) const -> typename std::enable_if<std::is_base_of<Field, T>::value == false, std::string>::type
	{ return prettyName(typeid(T)) + "!!!"; }
      };

    /**
       Example implementation of a REST module with object reflection
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
      
      struct PrintFunctor {
	ConnContext& ctx;
	Book& book; 
	
      PrintFunctor(ConnContext& _ctx, Book& _book) : ctx(_ctx), book(_book) {}
	template <typename S>
	inline void operator()( const Field_<Book, S>& field){
	  ctx << field.get(book);
	}
      };

      void operator()(ConnContext& ctx, int id, const std::string& prop){
	if(id < 0 || id >= (int)books.size())
	  ctx << HttpStatus::Bad_Request;
	else {
	  auto mt = mxcomp::tuples::make_mapped([](const Member& m) { return m.name; },
						MetaClass_<Book>::fields());				      
	  mt.findAndRun(prop, PrintFunctor(ctx, books[id]));
	}
      }

      void Register(Server& server){
	typedef RestAdvModule T;
	rikitiki::rest::Register(server, "/book-adv", this);
	server.AddHandler(CreateRoute<int, std::string>::With(this, "/book-adv/{id}/{property}"));
      }
    };

  }
}
