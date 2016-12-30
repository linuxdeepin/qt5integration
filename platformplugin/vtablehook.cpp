#include "vtablehook.h"

DPP_BEGIN_NAMESPACE

QHash<quintptr**, quintptr*> VtableHook::objToOriginalVfptr;
QHash<void*, quintptr*> VtableHook::objToGhostVfptr;

bool VtableHook::copyVtable(quintptr **obj)
{
    quintptr *vtable = *obj;

    while (*vtable > 0) {
        ++vtable;
    }

    int vtable_size = vtable - *obj;

    if (vtable_size == 0)
        return false;

    quintptr *new_vtable = new quintptr[++vtable_size];

    memcpy(new_vtable, *obj, (vtable_size) * sizeof(quintptr));

    //! save original vfptr
    objToOriginalVfptr[obj] = *obj;
    *obj = new_vtable;
    //! save ghost vfptr
    objToGhostVfptr[obj] = new_vtable;

    return true;
}

bool VtableHook::clearGhostVtable(void *obj)
{
    quintptr *vtable = objToGhostVfptr.take(obj);

    if (vtable) {
        objToOriginalVfptr.remove((quintptr**)obj);

        delete[] vtable;

        return true;
    }

    return false;
}

DPP_END_NAMESPACE
