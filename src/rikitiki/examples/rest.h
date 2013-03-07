/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki.h>
#include <rikitiki/mongoose/server.h>
#include <rikitiki/rest/rest.h>
#include <rikitiki/jsoncpp/jsoncpp.h>
#include <utils/config.h>
#include <sqlite3.h>

using namespace rikitiki;
using namespace rikitiki::mongoose;

namespace rikitiki {
  namespace examples {
    /**
       Example implementation of a REST module against sqlite. 
       Demonstrates
       - Rest Features
       - Json Features
     */
    struct RestModule  {
      static const char* create_table,*select_all,*select,*insert;
      sqlite3_stmt *insert_stmt, *select_all_stmt, *select_stmt;
      sqlite3* handle; 

      /// Throw an exception if a sqlite operation returns an unexpected return value
      void throwNQ(ConnContext& ctx, int retval, int desired_retval){
	if(retval != desired_retval){
	  ctx.response.reset();
	  ctx << "Sqlite3 err code: " << retval << "\n"
	      << sqlite3_errmsg(handle);
	  throw HandlerException();
	}
      }

      /// Read a single row from a result set
      Json::Value readRow(sqlite3_stmt* stmt){
	Json::Value row;
	row["id"]   = sqlite3_column_int(stmt, 0);
	row["name"] = std::string((const char*)sqlite3_column_text(stmt, 1));
	row["author"] = std::string((const char*)sqlite3_column_text(stmt, 2));
	row["isbn"] = std::string((const char*)sqlite3_column_text(stmt, 3));
	return row;
      }

      /// Post operations add items to the DB
      void POST(ConnContext& ctx){
	Json::Value val;
	ctx >> val;
	std::string name = val["name"].asString();
	std::string author = val["author"].asString();
	std::string isbn = val["isbn"].asString();

	sqlite3_reset(insert_stmt);
	throwNQ(ctx, sqlite3_bind_text(insert_stmt, 1, &name[0], name.size(), SQLITE_TRANSIENT), SQLITE_OK);
	throwNQ(ctx, sqlite3_bind_text(insert_stmt, 2, &author[0], author.size(), SQLITE_TRANSIENT), SQLITE_OK);
	throwNQ(ctx, sqlite3_bind_text(insert_stmt, 3, &isbn[0], isbn.size(), SQLITE_TRANSIENT), SQLITE_OK);
	throwNQ(ctx, sqlite3_step(insert_stmt), SQLITE_DONE);
	ctx << sqlite3_last_insert_rowid(handle);
      }

      /// Get operations without an ID return a list of all books
      void GET(ConnContext& ctx){
	Json::Value val(Json::arrayValue);
	int retval;

	sqlite3_reset(select_all_stmt);

	while((retval = sqlite3_step(select_all_stmt)) == SQLITE_ROW)
	  val.append(readRow(select_all_stmt));

	throwNQ(ctx, retval, SQLITE_DONE);
	ctx << val;
      }

      void GET(ConnContext& ctx, int id){
	sqlite3_bind_int(select_stmt, 1, id);
	sqlite3_step(select_stmt);
	ctx << readRow(select_stmt);
	sqlite3_reset(select_stmt);
      }

      void DELETE(ConnContext& ctx){
	char* error;
	sqlite3_exec(handle,"DELETE from books", 0, 0, &error);
	if(error){	  
	  ctx << error;
	  throw HandlerException{};
	}
	sqlite3_exec(handle,create_table,0,0,0);
	ctx.handled = true; // If we don't write anything to ctx, it doesn't count as handled. 
      }

      void init_db(){
	sqlite3_exec(handle,create_table,0,0,0);
	sqlite3_prepare_v2(handle, insert, strlen(insert), &insert_stmt, NULL);
	sqlite3_prepare_v2(handle, select_all, strlen(select_all), &select_all_stmt, NULL);
	sqlite3_prepare_v2(handle, select, strlen(select), &select_stmt, NULL);
      }
      
      RestModule() {
	int retval = sqlite3_open_v2(":memory:", &handle, SQLITE_OPEN_READWRITE, 0);
	if(retval){
	  LOG(Rest, Error) << "Sqlite3 err code: " << retval << std::endl << sqlite3_errmsg(handle) << std::endl;
	} else {
	  init_db();
	}
      }      

      void Register(Server& server){
	typedef RestModule T;
	rikitiki::rest::Register(server, "/book", this);
      }
    };

    const char* RestModule::insert = "insert into books (name, author, isbn) values (?, ?, ?)";
    const char* RestModule::create_table = "create table if not exists books (id INTEGER PRIMARY KEY, name TEXT NOT NULL, author TEXT NOT NULL, isbn TEXT NOT NULL)";
    const char* RestModule::select_all = "select id, name, author, isbn from books;";
    const char* RestModule::select = "select id, name, author, isbn from books where id = ?;";
  }
}
