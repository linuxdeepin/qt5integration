/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
