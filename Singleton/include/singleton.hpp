
#ifndef  __SINGLETON_H__
#define  __SINGLETON_H__

#include <pthread.h>
#include "singleton_details.hpp"

template<typename T, bool use_make_instance>
struct MakeInstanceHelper
{
    static T* MakeInstance() { return T::MakeInstance(); }
};

template<typename T>
struct MakeInstanceHelper<T, false>
{
    static T* MakeInstance() { return new T(); }
};

template<typename T>
class Singleton
{
    static void MakeInstance();
    static T* pInstance;
    public:
        static T* Instance();
};
template<typename T>
T* Singleton<T>::pInstance = NULL;

template<typename T>
T* Singleton<T>::Instance()
{
    pthread_once(&T::_pthread_once_flag_, Singleton<T>::MakeInstance);
    return Singleton::pInstance;
}

template<typename T>
void Singleton<T>::MakeInstance()
{
    Singleton::pInstance = MakeInstanceHelper<T, TypeHasMakeInstance<T>::value >::MakeInstance();
}

#define ENABLE_SINGLETON(T) \
    private: \
            T(const T&); \
            T& operator=(const T&); \
            static pthread_once_t _pthread_once_flag_; \
            friend class Singleton<T>; \
            friend class MakeInstanceHelper<T, true>; \
            friend class MakeInstanceHelper<T, false>

#define SINGLETON_IMPL(T) \
        pthread_once_t T::_pthread_once_flag_ = PTHREAD_ONCE_INIT

#endif   /* ----- #ifndef __SINGLETON_H__  ----- */
