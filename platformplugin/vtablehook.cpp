/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
        quintptr **obj_ptr = (quintptr**)obj;
        objToOriginalVfptr.remove(obj_ptr);

        delete[] vtable;

        return true;
    }

    return false;
}

DPP_END_NAMESPACE
