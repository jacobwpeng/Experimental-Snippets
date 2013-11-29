
#ifndef  __SINGLETON_DETAILS_H__
#define  __SINGLETON_DETAILS_H__

template <typename Type>
class TypeHasMakeInstance
{
    typedef char yes;
    typedef struct {char dummy[2];} no;

    template<typename U, U> struct TypeCheckEqual;

    template<typename T> struct TypeHelper
    {
        //无参且返回指向该类型指针
        typedef Type* (*pf)();
    };

    template<typename C> static yes HasMakeInstance( TypeCheckEqual< typename TypeHelper<Type>::pf, &C::MakeInstance >* );
    template<typename C> static no HasMakeInstance(...);

    public:
    enum { value = sizeof(HasMakeInstance<Type>(0)) == sizeof(yes) };
};

#endif   /* ----- #ifndef __SINGLETON_DETAILS_H__  ----- */
