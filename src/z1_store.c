//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_store.c                                                      |
// module: store                                                         |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#include "z1_store.h"
#include "z1_log.h"

#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void* g_mapRegionItemStore = NULL;
void* g_mapRegionStringStore = NULL;
void* g_mapRegionParameters = NULL;

int g_fdItemStore = -1;
int g_fdStringStore = -1;
int g_fdParameters = -1;

size_t g_itemTableSize = 0;
size_t g_stringTableSize = 0;
size_t g_parametersSize = 0;

// These are pointers because they reference the values in the Parameters table
Z1ScalarID* g_firstFreeItem;        // ID of the first free Item record in the free chain
Z1BigCounter* g_freeItemCount;      // Number of free item records left    
Z1IndexLength* g_stringFreeBase;    // Begining of the string heap

// Record ID <---> Scalar (64b.) conversion
U64 z1_id2scalar(Z1RecordID p_id){ return (Z1ScalarID)p_id.m_lower + (U64)p_id.m_high * 0x100000000; }
Z1RecordID z1_scalar2id(Z1ScalarID p){ 
    Z1RecordID l_result;
    l_result.m_high = (p / 0x100000000) % 0x10000; 
    l_result.m_lower = p % 0x100000000;
    return l_result;
}

// Create a new table
// Do not open it
// Abort if already exists
void createZ1Table(char* p_path, size_t p_size){
    int l_sysCallResult;

    // Abort if file exists
    struct stat l_fileStat;
    if(stat(p_path, &l_fileStat) >= 0){
        Z1_LOG_CRITICAL("Z1 table file [%s] already exists", p_path)
        abort();
    }
    
    // Open item store file descriptor for creation writing
    int l_tableFd = open(p_path, O_CREAT | O_RDWR, (mode_t)0660);
    if(l_tableFd == -1){
        Z1_LOG_CRITICAL("Z1 table file [%s] cannot be opened for writing: %d-%s", 
            p_path, errno, strerror(errno) )
        abort();
    }
    // seek to end of file to make it the right size
    l_sysCallResult = lseek(l_tableFd, p_size-1, SEEK_SET);
    if(l_sysCallResult == -1){
        Z1_LOG_CRITICAL("Error trying to lseek within [%s] : %d-%s", 
            p_path, errno, strerror(errno) )
        abort();
    }

    // write an 'X' at end of file (otherwise, the seek has no effect)
    l_sysCallResult = write(l_tableFd, "X", 1); 
    if(l_sysCallResult == -1){
        Z1_LOG_CRITICAL("Error trying to write at end of [%s] : %d-%s", 
            p_path, errno, strerror(errno) )
        abort();
    }

    close(l_tableFd);

    Z1_LOG_INFO("Z1 table file [%s] successfully created and resized", p_path) 
}

// Map a table to memory
// Aborts if not exist
void mapTable(char* p_path, int* p_fd, void** p_region, size_t p_size){
    int l_sysCallResult;

    // Abort if file does not exists
    struct stat l_fileStat;
    if(stat(p_path, &l_fileStat) < 0){
        Z1_LOG_CRITICAL("Z1 table file [%s] does not exists", p_path)
        abort();
    }
    
    // Open item store file descriptor for creation writing
    int l_tableFd = open(p_path, O_RDWR);
    if(l_tableFd == -1){
        Z1_LOG_CRITICAL("Z1 table file [%s] cannot be opened for R/W: %d-%s", 
            p_path, errno, strerror(errno) )
        abort();
    }
    // creating mapped region
    void *l_mapRegion = mmap(
        NULL,                       // No a-priori given start address --> the kernel will choose it
        p_size,                     // size of the mapped region (= file size)
        PROT_WRITE | PROT_READ,     // flags to authorize both read and write (but not code execution)
        MAP_SHARED,                 // Shared with other processes (normally not needed but bugs otherwise)
        l_tableFd,                  // file descriptor of the mapped file
        0                           // no offset --> mapped region starts at first byte of file
    ); 
    if(l_mapRegion == MAP_FAILED){
        Z1_LOG_CRITICAL("Error trying to map [%s] : %d-%s", 
            p_path, errno, strerror(errno) )
        close(l_tableFd);
        abort();
    }   
    Z1_LOG_INFO("Z1 table file [%s] successfully mapped to memory starting at [%s]", 
        p_path, z1_displayPointer((U64)l_mapRegion))  

    *p_region = l_mapRegion;
    *p_fd = l_tableFd;
}

// Unmap a previously opened table
// No control: will produce unpredictable results if called on unopened table
void unmapTable(char* p_path, int p_fd, void *p_region, size_t p_size){
    // closing map region
    int l_sysCallResult = munmap(p_region, p_size); 
    if(l_sysCallResult == -1){
        Z1_LOG_CRITICAL("Failed to unmap l_mapRegion [%s] : %d-%s", 
            p_path, errno, strerror(errno) );
        abort();
    }
    Z1_LOG_INFO("Item Store file [%s] unmapped successfully", p_path)

    // closing file
    close(p_fd);
    Z1_LOG_INFO("Item Store file [%s] file descriptor closed successfully", p_path)
}

void parameterHookUp(){
    // Check if store actually opened
    if( g_fdParameters < 0 || g_parametersSize == 0 || g_mapRegionParameters == NULL){

        Z1_LOG_CRITICAL("Attempting to hook-up an unmapped parameter table")
        abort();
    }

    // ID of the first free Item record in the free chain
    g_firstFreeItem = (Z1ScalarID*)(g_mapRegionParameters + 1);
    
    // Number of free item records left    
    g_freeItemCount = (Z1BigCounter*)(g_firstFreeItem + 1);
    
    // Begining of the string heap
    g_stringFreeBase = (Z1IndexLength*)(g_freeItemCount + 1);
    
    Z1_LOG_DEBUG("g_firstFreeItem:   [%s]", z1_displayPointer((U64)g_firstFreeItem))
    Z1_LOG_DEBUG("g_freeItemCount:   [%s]", z1_displayPointer((U64)g_freeItemCount))
    Z1_LOG_DEBUG("g_stringFreeBase:  [%s]", z1_displayPointer((U64)g_stringFreeBase))
}

// Create both tables
// Abort if sizes not previously calculated
void createStore(){
    // Check if sizes calculated
    if( g_itemTableSize == 0 || g_stringTableSize == 0 || g_parametersSize == 0){
        Z1_LOG_CRITICAL("Attempting to create table with zero sizes")
        abort();
    }
    Z1_LOG_INFO("Creating tables")
    
    // create tables
    createZ1Table(Z1_ITEM_STORE_FN, g_itemTableSize);
    createZ1Table(Z1_STRING_STORE_FN, g_stringTableSize);
    createZ1Table(Z1_PARAMETERS_FN, g_parametersSize);

    Z1_LOG_INFO("Table low-level init")

    // Map tables requiring an initialization
    mapTable(Z1_ITEM_STORE_FN, &g_fdItemStore, &g_mapRegionItemStore, g_itemTableSize);
    mapTable(Z1_PARAMETERS_FN, &g_fdParameters, &g_mapRegionParameters, g_parametersSize);
    
    // Hookup parameter pointer to the Parameters mapped area
    parameterHookUp();

    Z1Counter l_itemCapacity = (Z1Counter)(g_itemTableSize/sizeof(Z1ItemData));
    // Set parameter default values
    *g_firstFreeItem = (Z1ScalarID)0;
    *g_freeItemCount = l_itemCapacity;
    *g_stringFreeBase = (Z1IndexLength)0;
    
    Z1_LOG_DEBUG("*g_freeItemCount : [%8llX] = %lld", *g_freeItemCount, *g_freeItemCount)
    
    // initialize linked list of free items
    for( Z1BigCounter i=0; i<l_itemCapacity; i++){
        Z1ItemData* l_item = (Z1ItemData*)g_mapRegionItemStore + i;
        l_item->m_ID = z1_scalar2id(i);
        l_item->m_flags = Z1_IS_EDGE | Z1_DELETED;
        l_item->EDGE_ORIGIN = z1_scalar2id(i);
        l_item->NEXT_EDGE = z1_scalar2id(i+1);
        sprintf(l_item->EDGE_TEXT, "Free item #%lld", i);
    }
    
    // Unmap tables
    unmapTable(Z1_ITEM_STORE_FN, g_fdItemStore, g_mapRegionItemStore, g_itemTableSize);
    unmapTable(Z1_PARAMETERS_FN, g_fdParameters, g_mapRegionParameters, g_parametersSize);

    Z1_LOG_INFO("Table creation and low-level init complete")
}

// Entry point to open the store
void openStore(){
    Z1_LOG_INFO("Initializing low-level store functions")

    // calculate table sizes
    size_t l_pageSize = sysconf(_SC_PAGESIZE);
    g_itemTableSize = l_pageSize * ITEM_PAGE_COUNT;
    g_stringTableSize = l_pageSize * STRING_PAGE_COUNT;
    g_parametersSize = l_pageSize;
    
    Z1_LOG_DEBUG("l_pageSize         : %d", (int)l_pageSize)
    Z1_LOG_DEBUG("g_itemTableSize    : %d", (int)g_itemTableSize)
    Z1_LOG_DEBUG("g_stringTableSize  : %d", (int)g_stringTableSize)
    Z1_LOG_DEBUG("g_parametersSize   : %d", (int)g_parametersSize)
    Z1_LOG_DEBUG("Item capacity      : %d", (int)(g_itemTableSize/sizeof(Z1ItemData)) )

    // Check if files already exist
    struct stat l_fileStat;
    int l_itemExists = (stat(Z1_ITEM_STORE_FN, &l_fileStat) >= 0);
    int l_stringExists = (stat(Z1_STRING_STORE_FN, &l_fileStat) >= 0);
    int l_paramExists = (stat(Z1_PARAMETERS_FN, &l_fileStat) >= 0);
    
    // if both do not exist ---> Create them
    if(!l_itemExists && !l_stringExists && !l_paramExists) createStore();
    else
        // If one exists but not the other --> Error
        if( l_itemExists + l_stringExists + l_paramExists < 4){
            Z1_LOG_CRITICAL("Some of [%s], [%s] and [%s] already exist but not all", 
                Z1_ITEM_STORE_FN, Z1_STRING_STORE_FN, Z1_PARAMETERS_FN)
            abort();
        }   
        
    // Both tables now exist --> map them
    mapTable(Z1_ITEM_STORE_FN, &g_fdItemStore, &g_mapRegionItemStore, g_itemTableSize);
    mapTable(Z1_STRING_STORE_FN, &g_fdStringStore, &g_mapRegionStringStore, g_stringTableSize);
    mapTable(Z1_PARAMETERS_FN, &g_fdParameters, &g_mapRegionParameters, g_parametersSize);
    
    // Hookup parameter pointer to the Parameters mapped area
    parameterHookUp();

    Z1_LOG_INFO("Z1 store Open")
}

// Entry point to close the store
void closeStore(){
    // Check if store actually opened
    if( g_fdItemStore < 0 || g_itemTableSize == 0 || g_mapRegionItemStore == NULL ||
        g_fdStringStore < 0 || g_stringTableSize == 0 || g_mapRegionStringStore == NULL ||
        g_fdParameters < 0 || g_parametersSize == 0 || g_mapRegionParameters == NULL){

        Z1_LOG_CRITICAL("Attempting to close an uninitialized store")
        abort();
    }
    
    // unmap tables and close file descriptors
    unmapTable(Z1_ITEM_STORE_FN, g_fdItemStore, g_mapRegionItemStore, g_itemTableSize);
    unmapTable(Z1_STRING_STORE_FN, g_fdStringStore, g_mapRegionStringStore, g_stringTableSize);
    unmapTable(Z1_PARAMETERS_FN, g_fdParameters, g_mapRegionParameters, g_parametersSize);

    Z1_LOG_INFO("Z1 store Closed")
}
