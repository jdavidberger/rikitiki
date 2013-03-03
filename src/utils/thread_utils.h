#include <pthread.h>

struct Guard {
  pthread_mutex_t* mut;
 Guard(pthread_mutex_t& _mut) : mut(&_mut){
    pthread_mutex_lock(mut);
  }
  ~Guard(){
    pthread_mutex_unlock(mut);
  }
};
