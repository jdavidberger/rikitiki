#define foreach(var, iterable) for(auto var = (iterable).begin(); var != (iterable).end();var++)

#define COMMA ,
#define ACCESSOR(type, name)			\
  type& name;					\
  const type& name const;

#define ACCESSOR_IMPL(type, name, code)	\
  type& name {					\
    code;					\
  }						\
  const type& name const {			\
    code;					\
  }						
