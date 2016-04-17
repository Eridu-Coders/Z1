//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_store_types.h                                                |
// module: store                                                         |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#ifndef __Z1_STORE_TYPES__
#define __Z1_STORE_TYPES__

#include "z1_types.h"

typedef struct Z1RecordID{              //!< Common type for Record IDs
    U32 m_lower;
    U16 m_high;
} __attribute__((__packed__)) Z1RecordID;

typedef U64 Z1ScalarID;                 //!< ID representation used for in-memory manipulation

typedef struct Z1Date{
    U32 m_lower;
    U8 m_high;
} __attribute__((__packed__)) Z1Date;   //!< Common type for date data

typedef U16 Z1NarrowFlagField;          //!< 16 Bit wide field for flag storage
typedef U32 Z1WideFlagField;            //!< 32 Bit wide field for flag storage
typedef U32 Z1Counter;                  //!< Common type for most types of counters in the storage module
typedef U64 Z1BigCounter;               //!< Bigger counter for where this is absolutely necessary
typedef U16 Z1SpecialID;                //!< ID for special vertices and SV types (0 = none)
typedef U16 Z1Status;                   //!< Type for status information
typedef U64 Z1IndexLength;              //!< String length or position in the string file

typedef union Z1ObjectType {
    Z1RecordID m_object;                //!< A single 48 bit Z1RecordID
    struct { 
        Z1SpecialID m_specialA;
        Z1SpecialID m_specialB;
        Z1SpecialID m_specialC;
    } quad;                             //!< 3 16-bit Special Vertex IDs
} __attribute__((__packed__)) Z1ObjectType;

# define LOCAL_TEXT_LEN_V 40
# define LOCAL_TEXT_LEN_E 30

typedef struct Z1ItemData{
    Z1RecordID m_ID;                                //!< Item ID
    Z1ObjectType m_type;                            //!< Type member. see Z1ObjectType for details
    Z1WideFlagField m_flags;                        //!< flags
    union {                                         //!< Tail - see general overview of Z1ItemData
        struct {                                    //!< Edge-specific part of the tail
            Z1RecordID m_ePrevious;                 //!< ID of the previous edge in the edge list of the origin vertex of the edge
            Z1RecordID m_eNext;                     //!< ID of the next edge in the edge list of the origin vertex of the edge
            Z1RecordID m_vOrigin;                   //!< ID of the origin vertex of the edge
            union { // simple or complex
                U8 m_text[LOCAL_TEXT_LEN_E];        //!< Fixed length text part of the tail of the edge tail - see general overview of Z1ItemData
                struct {                            //!< Complex part of the tail of the edge tail - see general overview of Z1ItemData
                    Z1RecordID m_vTarget;           //!< ID of the target vertex of the edge
                    Z1RecordID m_eReciprocal;       //!< ID of reciprocal edge of the edge, if there is one.

                    Z1Date m_creationDate;          //!< Creation date of this edge
                    Z1Date m_lastModDate;           //!< Last modification date
                } __attribute__((__packed__)) complex;
            } __attribute__((__packed__)) tail;     
        } __attribute__((__packed__)) edge;         
        struct {                                    //!< Complex part of the tail of the vertex tail - see general overview of Z1ItemData
            Z1BigCounter m_incomingEdges;           //!< Number of edges pointing towards this vertex (if 0, it can be garbage collected)
            union {                                 //!< Tail of the vertex tail - see general overview of Z1ItemData
                U8 m_text[LOCAL_TEXT_LEN_V];        //!< Fixed length text part of the tail of the vertex tail - see general overview of Z1ItemData
                struct { // complex case
                    Z1RecordID m_firstEdge;         //!< ID of the first edge of this vertex
                    Z1RecordID m_firstEdgeSpecial;  //!< ID of the first edge of this vertex
                    Z1IndexLength m_stringID;       //!< Displacement of the label string stored in the "strings" db
                    Z1IndexLength m_searchStringID; //!< Displacement of the application determined search string stored in the "strings" db
                    Z1Date m_creationDate;          //!< Creation date of this vertex
                    Z1Date m_lastModDate;           //!< Last Modification date
                } __attribute__((__packed__)) complex;
            } __attribute__((__packed__)) tail;
        } __attribute__((__packed__)) vertex;
    } __attribute__((__packed__)) tail;
} __attribute__((__packed__)) Z1ItemData;

#define PREVIOUS_EDGE tail.edge.m_ePrevious
#define NEXT_EDGE tail.edge.m_eNext
#define EDGE_ORIGIN tail.edge.m_vOrigin

#define EDGE_TEXT tail.edge.tail.m_text

#define EDGE_TARGET tail.edge.tail.complex.m_vTarget
#define EDGE_RECIPROCAL tail.edge.tail.complex.m_eReciprocal
#define EDGE_CREATION tail.edge.tail.complex.m_creationDate
#define EDGE_LMD tail.edge.tail.complex.m_lastModDate

#define INCOMING_EDGES tail.vertex.m_incomingEdges

#define VERTEX_TEXT tail.vertex.tail.m_text

#define FIRST_EGE tail.vertex.tail.complex.m_firstEdge
#define FIRST_EDGE_SPECIAL tail.vertex.tail.complex.m_firstEdgeSpecial
#define STRING_ID tail.vertex.tail.complex.m_stringID
#define SEARCH_STRING tail.vertex.tail.complex.m_searchStringID
#define VISIBLE_EGES tail.vertex.tail.complex.m_visibleEdges
#define VERTEX_CREATION tail.vertex.tail.complex.m_creationDate
#define VERTEX_LMD tail.vertex.tail.complex.m_lastModDate
#define EXTERNAL_REF tail.vertex.tail.complex.m_externalRef

#endif // __Z1_STORE_TYPES__
