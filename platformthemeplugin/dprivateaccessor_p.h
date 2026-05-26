// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qcompilerdetection.h>

// Private member accessor using the explicit template instantiation technique.
//
// C++ Standard [temp.explicit]/12 states:
// "The usual access checking rules do not apply to names used to
// specify explicit instantiation definitions."
//
// This allows passing pointers to private/protected data members and
// member functions as template arguments in explicit instantiations,
// bypassing normal access control — without modifying the class definition
// and without the UB caused by "#define private public".
//
// NOTE: The friend declaration must live inside the Tag struct so the friend
// function is findable via ADL when calling get(TagName{}).

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wnon-template-friend")

template<typename Tag>
struct Qt5IntegrationPrivateAccessor
{
    using MemberPtr = typename Tag::MemberPtr;
    friend MemberPtr get(Tag) noexcept;
};

template<typename Tag, typename Tag::MemberPtr Ptr>
struct Qt5IntegrationPrivateAccessorImpl : Qt5IntegrationPrivateAccessor<Tag>
{
    friend typename Tag::MemberPtr get(Tag) noexcept { return Ptr; }
};

QT_WARNING_POP

// Non-static data member access
// Usage: D_PRIVATE_MEMBER(obj, TagName{})   → gives the member value (copy/ref)
// Usage: &D_PRIVATE_MEMBER(obj, TagName{})  → gives address of member
#define D_DECLARE_PRIVATE_MEMBER(TagName, ClassName, MemberName, MemberType) \
    struct TagName { \
        using MemberPtr = MemberType ClassName::*; \
        friend MemberPtr get(TagName) noexcept; \
    }; \
    template struct Qt5IntegrationPrivateAccessorImpl<TagName, &ClassName::MemberName>

// Trampoline: ensures get(tag) is called from a context with no class-scope
// get() member that might suppress ADL (C++ [basic.lookup.argdep] para 3).
namespace dtk_private_detail {
    template<typename Tag>
    inline typename Tag::MemberPtr access(Tag t) noexcept { return get(t); }
}

#define D_PRIVATE_MEMBER(obj, tag) ((obj).*dtk_private_detail::access(tag))

// Non-static member function call
// Usage: D_PRIVATE_CALL(obj, TagName{}, arg1, arg2)
#define D_DECLARE_PRIVATE_FUNCTION(TagName, ClassName, FuncName, RetType, ...) \
    struct TagName { \
        using MemberPtr = RetType (ClassName::*)(__VA_ARGS__); \
        friend MemberPtr get(TagName) noexcept; \
    }; \
    template struct Qt5IntegrationPrivateAccessorImpl<TagName, &ClassName::FuncName>

#define D_PRIVATE_CALL(obj, tag, ...) ((obj).*dtk_private_detail::access(tag))(__VA_ARGS__)

// Static data member access
// get(TagName{}) returns MemberType*, so D_PRIVATE_STATIC_MEMBER dereferences it
#define D_DECLARE_PRIVATE_STATIC_MEMBER(TagName, ClassName, MemberName, MemberType) \
    struct TagName { \
        using MemberPtr = MemberType*; \
        friend MemberPtr get(TagName) noexcept; \
    }; \
    template struct Qt5IntegrationPrivateAccessorImpl<TagName, &ClassName::MemberName>

#define D_PRIVATE_STATIC_MEMBER(tag) (*dtk_private_detail::access(tag))
