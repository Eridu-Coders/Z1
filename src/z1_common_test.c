//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_common_test.c                                                |
// module: common                                                        |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#include "z1_types.h"
#include "z1_log.h"

void main(int p_argc, char** p_argv){
    z1_initLog();
    printf("Z1 Common module test\n");
    
    printf("sizeof(%3s): %d\n", "U8", (int)sizeof(U8));
    printf("sizeof(%3s): %d\n", "U16", (int)sizeof(U16));
    printf("sizeof(%3s): %d\n", "U32", (int)sizeof(U32));
    printf("sizeof(%3s): %d\n\n", "U64", (int)sizeof(U64));

    printf("sizeof(%3s): %d\n", "S8", (int)sizeof(S8));
    printf("sizeof(%3s): %d\n", "S16", (int)sizeof(S16));
    printf("sizeof(%3s): %d\n", "S32", (int)sizeof(S32));
    printf("sizeof(%3s): %d\n\n", "S64", (int)sizeof(S64));
    
    printf("sizeof(%3s): %d\n\n", "size_t", (int)sizeof(size_t));
}