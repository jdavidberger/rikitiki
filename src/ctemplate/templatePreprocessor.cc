#include <server.h>
using namespace rikitiki;
using namespace rikitiki::ctemplates;
void Server::AddPreprocessor( TemplatePreprocessor* tp){
    templatePreprocessors.push_back(tp);
}
