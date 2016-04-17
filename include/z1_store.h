//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_store.h                                                      |
// module: store                                                         |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#ifndef __Z1_STORE__
#define __Z1_STORE__

#include "z1_store_types.h"

#define Z1_ITEM_STORE_FN "ItemStore.z1"
#define Z1_STRING_STORE_FN "StringStore.z1"
#define Z1_PARAMETERS_FN "Parameters.z1"

#define ITEM_PAGE_COUNT 1024
#define STRING_PAGE_COUNT 1024
#define INDEX_PAGE_COUNT 128

extern void openStore();
extern void closeStore();

#endif //__Z1_STORE__