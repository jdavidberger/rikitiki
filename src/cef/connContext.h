/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#define USING_CEF_SHARED 1
#pragma warning(disable: 4100 4668)
#include <include/cef_request_handler.h>
#include <include/cef_scheme.h>
#include <include/cef_client.h>
#include <include/cef_app.h>
#pragma warning(default: 4100 4668)


#include <rikitiki\connContext.h>
#include <rikitiki\http\incoming\Request.h>
#include <rikitiki\http\helpers\bufferedMessageTypes.h>
namespace rikitiki {
     struct Server;

     namespace cef {

     class Response : public virtual OResponseMemory {
     protected:
          virtual void SetState(MessageState::type newState) OVERRIDE {
               MessageState::type oldState = streamState.streamState;
               OResponseMemory::SetState(newState);

               if (oldState <= MessageState::HEADERS && newState >= MessageState::BODY && headersReady) {
                    headersReady->Continue();
                    headersReady = 0;
               }

               if (newState == MessageState::FINISHED && dataReady) {
                    dataReady->Continue();
                    dataReady = 0;
               }
          }

          virtual size_t WritePayloadData(const char* data, size_t size) OVERRIDE {
               auto rtn = OResponseMemory::WritePayloadData(data, size);
               if (dataReady) {                    
                    dataReady->Continue();
                    dataReady = 0;
               }
               return rtn;
          }
     public:
          
          Response(const CefRefPtr<CefRequest>&){}
          CefRefPtr<CefCallback> headersReady;
          CefRefPtr<CefCallback> dataReady;
     };

     class Request : public IMessage_<rikitiki::http::helpers::BufferedRequest> {
          protected:
               virtual void FillHeaders(HeaderCollection&) const OVERRIDE;
               virtual void FillRequestMethod(Request::Method&) const OVERRIDE;
               virtual void FillQueryString(QueryStringCollection&) const OVERRIDE;
               CefRefPtr<CefRequest> request;
               mutable rikitiki::string url;
          public:
               Request(const CefRefPtr<CefRequest>& req) : request(req) {
                    currentState.streamState = MessageState::FINISHED;
                    if (request->GetPostData()) {
                         CefPostData::ElementVector vector;
                         request->GetPostData()->GetElements(vector);
                         for (auto it : vector) {
                              auto bytes = it->GetBytesCount();
                              char* buffer = (char*)malloc(bytes);
                              it->GetBytes(bytes, buffer);
                              Body().write(buffer, (std::streamsize)bytes);
                         }
                    }
               }

	       virtual const wchar_t* URI() const OVERRIDE;               
          };

     }
}
