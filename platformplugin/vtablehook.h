#ifndef VTABLEHOOK_H
#define VTABLEHOOK_H

#include <QObject>
#include <QSet>
#include <QDebug>

class VtableHook
{
public:
    static bool copyVtable(quintptr **obj);
    static bool clearGhostVtable(void *obj);

    template <typename List1, typename List2> struct CheckCompatibleArguments { enum { value = false }; };
    template <typename List> struct CheckCompatibleArguments<List, List> { enum { value = true }; };
    template<typename Fun1, typename Fun2>
    static bool overrideVfptrFun(const typename QtPrivate::FunctionPointer<Fun1>::Object *t1, Fun1 fun1,
                      const typename QtPrivate::FunctionPointer<Fun2>::Object *t2, Fun2 fun2)
    {
        //! ({code}) in the form of a code is to eliminate - Wstrict - aliasing build warnings
        quintptr fun1_offset = *(quintptr *)&fun1;
        quintptr fun2_offset = *(quintptr *)&fun2;

        if (fun1_offset < 0 || fun1_offset > UINT_LEAST16_MAX)
            return false;

        if (!objToOriginalVfptr.contains((quintptr**)t1) && !copyVtable((quintptr**)t1))
            return false;

        typedef QtPrivate::FunctionPointer<Fun1> FunInfo1;
        typedef QtPrivate::FunctionPointer<Fun2> FunInfo2;

        //compilation error if the arguments does not match.
        Q_STATIC_ASSERT_X((CheckCompatibleArguments<typename FunInfo1::Arguments, typename FunInfo2::Arguments>::value),
                          "Function1 and Function2 arguments are not compatible.");
        Q_STATIC_ASSERT_X((CheckCompatibleArguments<QtPrivate::List<typename FunInfo1::ReturnType>, QtPrivate::List<typename FunInfo2::ReturnType>>::value),
                          "Function1 and Function2 return type are not compatible..");

        quintptr *vfptr_t1 = *(quintptr**)t1;
        quintptr *vfptr_t2 = *(quintptr**)t2;

        if (fun2_offset > UINT_LEAST16_MAX)
            *(vfptr_t1 + fun1_offset / sizeof(quintptr)) = fun2_offset;
        else
            *(vfptr_t1 + fun1_offset / sizeof(quintptr)) = *(vfptr_t2 + fun2_offset / sizeof(quintptr));

        return true;
    }

    template<typename Fun1>
    static bool resetVfptrFun(const typename QtPrivate::FunctionPointer<Fun1>::Object *t1, Fun1 fun1)
    {
        quintptr *vfptr_t2 = objToOriginalVfptr.value((quintptr**)t1, 0);

        if (!vfptr_t2)
            return false;

        //! ({code}) in the form of a code is to eliminate - Wstrict - aliasing build warnings
        quintptr fun1_offset = *(quintptr *)&fun1;

        if (fun1_offset < 0 || fun1_offset > UINT_LEAST16_MAX)
            return false;

        quintptr *vfptr_t1 = *(quintptr**)t1;

        *(vfptr_t1 + fun1_offset / sizeof(quintptr)) = *(vfptr_t2 + fun1_offset / sizeof(quintptr));

        return true;
    }

private:
    static QHash<quintptr**, quintptr*> objToOriginalVfptr;
    static QHash<void*, quintptr*> objToGhostVfptr;
};

#endif // VTABLEHOOK_H
