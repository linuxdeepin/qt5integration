/*
 * SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <themed_icon_lookup.h>

using namespace themed_icon_lookup;

int main()
{
    auto *r = find_icon("TestIcon", 32, 1);
    free_cstring(const_cast<char *>(r));

    return 0;
}
