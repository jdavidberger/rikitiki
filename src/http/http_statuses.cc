#include <mxcomp/utf.h>
#include <rikitiki/http/http_statuses.h>

namespace rikitiki {    
  HttpStatus::HttpStatus(uint16_t s, const std::string& n, bool _managed) : status(s), name(n), managed(_managed) {}
  HttpStatus::HttpStatus(uint16_t s, const std::wstring& n, bool _managed) : status(s), managed(_managed) {
    name = mxcomp::utf::convert(n);
    }

  const HttpStatus HttpStatus::Continue(100, "Continue",true);
  const HttpStatus HttpStatus::Switching_Protocols(101, "Switching Protocols",true);
  const HttpStatus HttpStatus::Processing(102, "Processing",true);
  const HttpStatus HttpStatus::OK(200, "OK",true);
  const HttpStatus HttpStatus::Created(201, "Created",true);
  const HttpStatus HttpStatus::Accepted (202, "Accepted",true);
  const HttpStatus HttpStatus::Non_Authoritative_Information (203, "Non-Authoritative Information",true);
  const HttpStatus HttpStatus::No_Content (204, "No Content", true);
  const HttpStatus HttpStatus::Reset_Content (205, "Reset Content", true);
  const HttpStatus HttpStatus::Partial_Content (206, "Partial Content", true);
  const HttpStatus HttpStatus::Multi_Status (207, "Multi-Status", true);
  const HttpStatus HttpStatus::Already_Reported (208, "Already Reported", true);
  const HttpStatus HttpStatus::IM_Used (226, "IM Used", true);
  const HttpStatus HttpStatus::Multiple_Choices (300, "Multiple Choices", true);
  const HttpStatus HttpStatus::Moved_Permanently (301, "Moved Permanently", true);
  const HttpStatus HttpStatus::Found(302, "Found", true);
  const HttpStatus HttpStatus::See_Other (303, "See Other", true);
  const HttpStatus HttpStatus::Not_Modified (304, "Not Modified", true);
  const HttpStatus HttpStatus::Use_Proxy (305, "Use Proxy", true);
  const HttpStatus HttpStatus::Switch_Proxy (306, "Switch Proxy", true);
  const HttpStatus HttpStatus::Temporary_Redirect (307, "Temporary Redirect", true);
  const HttpStatus HttpStatus::Permanent_Redirect (308, "Permanent Redirect", true);
  const HttpStatus HttpStatus::Bad_Request (400, "Bad Request", true);
  const HttpStatus HttpStatus::Unauthorized (401, "Unauthorized", true);
  const HttpStatus HttpStatus::Payment_Required (402, "Payment Required", true);
  const HttpStatus HttpStatus::Forbidden (403, "Forbidden", true);
  const HttpStatus HttpStatus::Not_Found (404, "Not Found", true);
  const HttpStatus HttpStatus::Method_Not_Allowed (405, "Method Not Allowed", true);
  const HttpStatus HttpStatus::Not_Acceptable (406, "Not Acceptable", true);
  const HttpStatus HttpStatus::Proxy_Authentication_Required (407, "Proxy Authentication Required", true);
  const HttpStatus HttpStatus::Request_Timeout (408, "Request Timeout", true);
  const HttpStatus HttpStatus::Conflict (409, "Conflict", true);
  const HttpStatus HttpStatus::Gone (410, "Gone", true);
  const HttpStatus HttpStatus::Length_Required (411, "Length Required", true);
  const HttpStatus HttpStatus::Precondition_Failed (412, "Precondition Failed", true);
  const HttpStatus HttpStatus::Request_Entity_Too_Large (413, "Request Entity Too Large", true);
  const HttpStatus HttpStatus::Request_URI_Too_Long (414, "Request-URI Too Long", true);
  const HttpStatus HttpStatus::Unsupported_Media_Type (415, "Unsupported Media Type", true);
  const HttpStatus HttpStatus::Requested_Range_Not_Satisfiable (416, "Requested Range Not Satisfiable", true);
  const HttpStatus HttpStatus::Expectation_Failed (417, "Expectation Failed", true);
  const HttpStatus HttpStatus::Im_a_teapot (418, "I'm a teapot", true);
  const HttpStatus HttpStatus::Enhance_Your_Calm (420, "Enhance Your Calm", true);
  const HttpStatus HttpStatus::Unprocessable_Entity (422, "Unprocessable Entity", true);
  const HttpStatus HttpStatus::Locked(423, "Locked", true);
  const HttpStatus HttpStatus::Failed_Dependency (424, "Failed Dependency", true);
  const HttpStatus HttpStatus::Unordered_Collection (425, "Unordered Collection", true);
  const HttpStatus HttpStatus::Upgrade_Required (426, "Upgrade Required", true);
  const HttpStatus HttpStatus::Precondition_Required (428, "Precondition Required", true);
  const HttpStatus HttpStatus::Too_Many_Requests (429, "Too Many Requests", true);
  const HttpStatus HttpStatus::Request_Header_Fields_Too_Large (431, "Request Header Fields Too Large", true);
  const HttpStatus HttpStatus::No_Response (444, "No Response", true);
  const HttpStatus HttpStatus::Retry_With (449, "Retry With", true);
  const HttpStatus HttpStatus::Blocked_by_Windows_Parental_Controls (450, "Blocked by Windows Parental Controls", true);
  const HttpStatus HttpStatus::Client_Closed_Request (499, "Client Closed Request", true);
  const HttpStatus HttpStatus::Internal_Server_Error (500, "Internal Server Error", true);
  const HttpStatus HttpStatus::Not_Implemented (501, "Not Implemented", true);
  const HttpStatus HttpStatus::Bad_Gateway (502, "Bad Gateway", true);
  const HttpStatus HttpStatus::Service_Unavailable (503, "Service Unavailable", true);
  const HttpStatus HttpStatus::Gateway_Timeout (504, "Gateway Timeout", true);
  const HttpStatus HttpStatus::HTTP_Version_Not_Supported (505, "HTTP Version Not Supported", true);
  const HttpStatus HttpStatus::Variant_Also_Negotiates (506, "Variant Also Negotiates", true);
  const HttpStatus HttpStatus::Insufficient_Storage (507, "Insufficient Storage", true);
  const HttpStatus HttpStatus::Bandwidth_Limit_Exceeded (509, "Bandwidth Limit Exceeded", true);
  const HttpStatus HttpStatus::Not_Extended(510, "Not Extended", true);    
}
