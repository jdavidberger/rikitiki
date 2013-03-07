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
    struct RestModule  {
      static const char* create_table,
	* select_all,
	* select,
	* insert;

      sqlite3* handle; 
      sqlite3_stmt *insert_stmt, *select_all_stmt, *select_stmt;

      Json::Value readRow(sqlite3_stmt* stmt){
	Json::Value row;
	row["id"]   = sqlite3_column_int(stmt, 0);
	row["name"] = std::string((const char*)sqlite3_column_text(stmt, 1));
	row["author"] = std::string((const char*)sqlite3_column_text(stmt, 2));
	row["isbn"] = std::string((const char*)sqlite3_column_text(stmt, 3));
	return row;
      }

      void POST(ConnContext& ctx){
	Json::Value val;
	ctx >> val;
	std::string name = val["name"].asString();
	std::string author = val["author"].asString();
	std::string isbn = val["isbn"].asString();
	sqlite3_bind_text(insert_stmt, 1, &name[0], name.size(), SQLITE_TRANSIENT);
	sqlite3_bind_text(insert_stmt, 2, &author[0], author.size(), SQLITE_TRANSIENT);
	sqlite3_bind_text(insert_stmt, 3, &isbn[0], isbn.size(), SQLITE_TRANSIENT);
	sqlite3_step(insert_stmt);
	sqlite3_reset(insert_stmt);
	ctx << sqlite3_last_insert_rowid(handle);
      }

      void GET(ConnContext& ctx){
	Json::Value val(Json::arrayValue);
	int retval;
	while((retval = sqlite3_step(select_all_stmt)) == SQLITE_ROW)
	  val.append(readRow(select_all_stmt));
	
	if(retval != SQLITE_DONE){
	  ctx << "Sqlite3 err code: " << retval << "\n"
	      << sqlite3_errmsg(handle);
	  throw HandlerException();
	}
	sqlite3_reset(select_all_stmt);
	ctx << val;
      }

      void GET(ConnContext& ctx, int id){
	sqlite3_bind_int(select_stmt, 1, id);
	sqlite3_step(select_stmt);
	Json::Value val = readRow(select_stmt);
	sqlite3_reset(select_stmt);
	ctx << val;
      }

      void DELETE(ConnContext& ctx){
	char* error;
	sqlite3_exec(handle,"DELETE from books", 0, 0, &error);
	if(error){
	  ctx << HttpStatus::Internal_Server_Error 
	      << error;
	}
	sqlite3_exec(handle,create_table,0,0,0);
	ctx.handled = true;
      }

      void init_db(){
	sqlite3_exec(handle,create_table,0,0,0);
	sqlite3_prepare(handle, insert, strlen(insert), &insert_stmt, NULL);
	sqlite3_prepare(handle, select_all, strlen(select_all), &select_all_stmt, NULL);
	sqlite3_prepare(handle, select, strlen(select), &select_stmt, NULL);
      }
      
      RestModule() {
	int retval = sqlite3_open("restModule.sqlite3", &handle);
	assert(!retval);
	init_db();
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


using namespace rikitiki::examples;
#ifdef USE_MONGOOSE

int main(){
  MongooseServer server(5000);
  RestModule module;

  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}

#endif
