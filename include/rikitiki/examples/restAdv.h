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

using namespace rikitiki;
using namespace rikitiki::mongoose;

namespace rikitiki {

  struct Character {
    int id; 
    std::string name;
  };

  template <> struct MetaClass_<Character> { 
    static auto fields() 
      STATIC_RETURN(make_fields(make_field("id", &Character::id),
				make_field("name", &Character::name)
				));
      };

  struct Book {
    int id;
    double cost; 
    std::string name;
    std::string author;
    std::string isbn;
    std::vector<Character> characters;
    Book() {}
  Book(int _id, double _cost, const std::string& _name, const std::string& _author, const std::string& _isbn) :
    id(_id), cost(_cost), name(_name), author(_author), isbn(_isbn) {}
  };

  template <> struct MetaClass_<Book> { 
    static auto fields() STATIC_RETURN(make_fields(make_field("id", &Book::id),
						   make_field("cost", &Book::cost),
						   make_field("name", &Book::name),
						   make_field("author", &Book::author),
						   make_field("characters", &Book::characters),
						   make_field("isbn", &Book::isbn)))
      };
  
  namespace examples {
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
	  MetaClass_<Book>::fields().findAndRun(prop, PrintFunctor(ctx, books[id]));
	}
      }

      void Register(Server& server){
	typedef RestAdvModule T;
	rikitiki::rest::Register(server, "/book-adv", this);
	server.AddHandler(CreateRoute<int, std::string>::With(this, "/book-adv/{id}/{property}"));
	books.push_back(Book{0, 1.0, "Test","This","thing"});
	books.push_back(Book{1, 3.0, "A", "B", "C"});
	books[0].characters.push_back( Character{0, "Ahab" });
      }
    };

  }
}
