/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 

#include <vector>
#include "mongoose/mongoose.h"
#include <sstream>
#include "content_types.h"
#include "utils/macros.h"
#include <tuple>
#include "utils/tuple_ext.h"
#include "utils/log.h"

#if USE_CTEMPLATE
#include <rikitiki/ctemplate/templatePreprocessor.h>
#endif

namespace rikitiki {
  class ConnContext;

  struct Handler {
    virtual bool Handle(ConnContext& ctx) = 0;
    virtual bool visible() const = 0;
    virtual std::string name() const = 0;
    virtual std::string desc() const;
  };

  struct CommandHandler : public Handler {
    std::string command;
    bool handleChildren;
    CommandHandler(std::string _command, std::string _desc="");
    virtual void Process(ConnContext& ctx) = 0;
    virtual bool Handle(ConnContext& ctx);
    virtual std::string name() const;
  };

  class Server {
  protected:
  public:
    std::vector<Handler*> handlers;

    #ifdef USE_CTEMPLATE
    void AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor*);
    std::vector<ctemplates::TemplatePreprocessor*> templatePreprocessors;
    #endif

    typedef bool (*handle_t)(ConnContext& ctx);
 
    bool Handle(ConnContext& ctx);

    void AddHandler( Handler& handler);
    void AddHandler( Handler* handler);
    void AddHandler( handle_t handler );

    template <typename T>
      void Register( T& t ){
      t.Register(*this);
    }
  };

}
