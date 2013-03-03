template <class T>
std::ostream& ConnContext::operator <<(const T& obj){
  handled = true;
  response.status = true;	
  return response << obj;
}
