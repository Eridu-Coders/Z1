//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_store_test.c                                                 |
// module: store                                                         |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#include "z1_store.h"
#include "z1_log.h"

// ------------ FUNCTIONS ---------------------------------------------------------------
void printSizeof(){
    printf("Size of Store modules types\n\n");
    
    printf("sizeof( %-20s) : %d\n", "Z1NarrowFlagField", (int)sizeof(Z1NarrowFlagField));
    printf("sizeof( %-20s) : %d\n", "Z1WideFlagField", (int)sizeof(Z1WideFlagField));
    printf("sizeof( %-20s) : %d\n", "Z1Counter", (int)sizeof(Z1Counter));
    printf("sizeof( %-20s) : %d\n", "Z1BigCounter", (int)sizeof(Z1BigCounter));
    printf("sizeof( %-20s) : %d\n", "Z1SpecialID", (int)sizeof(Z1SpecialID));
    printf("sizeof( %-20s) : %d\n", "Z1Status", (int)sizeof(Z1Status));
    printf("sizeof( %-20s) : %d\n\n", "Z1IndexLength", (int)sizeof(Z1IndexLength));

    printf("sizeof( %-20s) : %d\n\n", "size_t", (int)sizeof(size_t));

    printf("sizeof( %-20s) : %d\n", "Z1RecordID", (int)sizeof(Z1RecordID));
    printf("sizeof( %-20s) : %d\n", "Z1ScalarID", (int)sizeof(Z1ScalarID));
    printf("sizeof( %-20s) : %d\n", "Z1Date", (int)sizeof(Z1Date));
    printf("sizeof( %-20s) : %d\n", "Z1ObjectType", (int)sizeof(Z1ObjectType));
    printf("sizeof( %-20s) : %d\n\n", "Z1ItemData", (int)sizeof(Z1ItemData));
}

// ------------ MAIN --------------------------------------------------------------------
void main(int p_argc, char** p_argv){
    z1_initLog();
    printf("Z1 Store module test\n\n");
    
    printSizeof();
    openStore();
    closeStore();
}