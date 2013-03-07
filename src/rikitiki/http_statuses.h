#pragma once

namespace rikitiki {
  /**
     Basic HttpStatus information -- status code and name of status.

     More usefully, holds static members for every standard status type. 
   */
  class HttpStatus { 
  HttpStatus(int s, const char* n) : status(s), name(n){} 
  HttpStatus(int s) : status(s), name(0){} 
  public:
    int status; 
    const char* name;
    static const HttpStatus Continue;
    static const HttpStatus Switching_Protocols;
    static const HttpStatus Processing;
    static const HttpStatus OK;
    static const HttpStatus Created;
    static const HttpStatus Accepted;
    static const HttpStatus Non_Authoritative_Information;
    static const HttpStatus No_Content;
    static const HttpStatus Reset_Content;
    static const HttpStatus Partial_Content;
    static const HttpStatus Multi_Status;
    static const HttpStatus Already_Reported;
    static const HttpStatus IM_Used;
    static const HttpStatus Multiple_Choices;
    static const HttpStatus Moved_Permanently;
    static const HttpStatus Found;
    static const HttpStatus See_Other;
    static const HttpStatus Not_Modified;
    static const HttpStatus Use_Proxy;
    static const HttpStatus Switch_Proxy;
    static const HttpStatus Temporary_Redirect;
    static const HttpStatus Permanent_Redirect;
    static const HttpStatus Bad_Request;
    static const HttpStatus Unauthorized;
    static const HttpStatus Payment_Required;
    static const HttpStatus Forbidden;
    static const HttpStatus Not_Found;
    static const HttpStatus Method_Not_Allowed;
    static const HttpStatus Not_Acceptable;
    static const HttpStatus Proxy_Authentication_Required;
    static const HttpStatus Request_Timeout;
    static const HttpStatus Conflict;
    static const HttpStatus Gone;
    static const HttpStatus Length_Required;
    static const HttpStatus Precondition_Failed;
    static const HttpStatus Request_Entity_Too_Large;
    static const HttpStatus Request_URI_Too_Long;
    static const HttpStatus Unsupported_Media_Type;
    static const HttpStatus Requested_Range_Not_Satisfiable;
    static const HttpStatus Expectation_Failed;
    static const HttpStatus Im_a_teapot;
    static const HttpStatus Enhance_Your_Calm;
    static const HttpStatus Unprocessable_Entity;
    static const HttpStatus Locked;
    static const HttpStatus Failed_Dependency;
    static const HttpStatus Unordered_Collection;
    static const HttpStatus Upgrade_Required;
    static const HttpStatus Precondition_Required;
    static const HttpStatus Too_Many_Requests;
    static const HttpStatus Request_Header_Fields_Too_Large;
    static const HttpStatus No_Response;
    static const HttpStatus Retry_With;
    static const HttpStatus Blocked_by_Windows_Parental_Controls;
    static const HttpStatus Client_Closed_Request;
    static const HttpStatus Internal_Server_Error;
    static const HttpStatus Not_Implemented;
    static const HttpStatus Bad_Gateway;
    static const HttpStatus Service_Unavailable;
    static const HttpStatus Gateway_Timeout;
    static const HttpStatus HTTP_Version_Not_Supported;
    static const HttpStatus Variant_Also_Negotiates;
    static const HttpStatus Insufficient_Storage;
    static const HttpStatus Bandwidth_Limit_Exceeded;
    static const HttpStatus Not_Extended;
  };
}
