/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <assert.h>
#include <iostream>
#include <stdio.h>

struct RefCount {
  int* count;
  
RefCount(int s) : count(0){
  if(s){
    count = (int*)malloc(sizeof(int));
    *count = s;
  }
}  
  
RefCount() : count(0){
  count = (int*)malloc(sizeof(int));
  *count = 1;
}
  
  RefCount(const RefCount& cp){    
    count = cp.count;
    inc();
  }
  
  bool valid() const {
    return count != NULL;
  }

  RefCount& operator=(const RefCount& cp){
    assert(count == 0);  
    count = cp.count;
    inc();
    return *this;
  }
    
    inline void inc(){
      assert(count);
      *count += 1;
    }
  inline bool dec(){
    if(count){
      assert(*count != 0);
      *count -= 1;
      if(*count == 0){
	free(count);
	count = 0;
	return true;
      }
    }
    return false;
  }
};
