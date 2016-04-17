//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_types.h                                                      |
// module: common                                                        |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#ifndef __Z1_TYPES__
#define __Z1_TYPES__

#include <stdio.h>
#include <stdlib.h>

//! Unsigned 8 bit integer
typedef unsigned char U8;
//! Unsigned 16 bit integer
typedef unsigned short U16;
//! Unsigned 32 bit integer
typedef unsigned int U32;
//! Unsigned 64 bit integer
typedef unsigned long long U64;

//! Signed 8 bit integer
typedef char S8;
//! Signed 16 bit integer
typedef short S16;
//! Signed 32 bit integer
typedef int S32;
//! Signed 64 bit integer
typedef long long S64;

/**
    \ingroup store_module
    \brief List of edge/row flag values

    ZzRowFlagValues may be \e edge flags or \e row flags in the proper sense. In the former
    case, they are stored with the edge data. In the latter they are simply passed as information
    from the Object layer to the UI layer.	Some are used in both cases. In the list below, 
    values are marked with an \b R if used as row flags and with an \b E if	used as edge flags only.
*/
enum Z1FlagValues {
    Z1_IS_EDGE              = 0x00000001,   //!< 1 --> Edge / 0 --> Vertex
    Z1_HAS_SPECIAL_TYPES    = 0x00000002,   //!< the type field of this vertex/edge refers to a quad of special types not a stored vertex 
    Z1_HAS_FIXED_TEXT       = 0x00000004,   //!< The structure tail is a fixed m_text and not a complex

    Z1_OPENED               = 0x00000008,   //!< Opened edge : [-] in list display - \b RE
    Z1_CLOSED               = 0x00000010,   //!< Closed edge : [+] in list display - \b RE
    Z1_SEPABOVE             = 0x00000020,   //!< has a separator above - \b RE
    Z1_SEPBELOW             = 0x00000040,   //!< has a separator below - \b RE
    Z1_PROTECTED            = 0x00000080,   //!< cannot be modified unles protection overridden - \b E
    Z1_HIDDEN               = 0x00000100,   //!< not displayed, unless specified - \b E
    Z1_DELETED              = 0x00000200,   //!< deleted edge - \b E
    Z1_ABSOLUTE             = 0x00000400,   //!< structural edges which cannot be deleted under any circumstances - \b E

    Z1_TYPE_MASK            = 0x00003800,   //!< Three bits able to store 7 types (0 = no flag types)
    Z1_ET_ISA               = 0x00000800,   //!< ISA Edge (Is A - Opo: ITO)
    Z1_ET_ITO               = 0x00001000,   //!< ITO Edge (Is Type Of - Opo: ISA)
    Z1_ET_OWN               = 0x00001800,   //!< OWN Edge (Owns - Opo: BLG)
    Z1_ET_BLG               = 0x00002000,   //!< BLG Edge (Belongs to - Opo: OWN)
    Z1_ET_AUTO              = 0x00002800,   //!< Auto Edge (link to self - No Opo)
    Z1_ET_FIELD             = 0x00003000,   //!< Edge leading to a field
    Z1_ET_RESERVED_TYPE1    = 0x00003800,   //!< Reserved edge type

    Z1_VT_SPECIAL           = 0x00000800,   //!< Root of all special vertices
    Z1_VT_TYPE              = 0x00001000,   //!< Root of all types - subtype of Z1_VT_SPECIAL
    Z1_VT_VERTEX_TYPE       = 0x00001800,   //!< Vertex Type - subtype of Z1_VT_TYPE
    Z1_VT_EDGE_TYPE         = 0x00002000,   //!< Edge Type - subtype of Z1_VT_TYPE
    Z1_VT_VISIBLE_ET        = 0x00002800,   //!< Visible Edge Type - subtype of Z1_VT_EDGE_TYPE
    Z1_VT_ETF               = 0x00003000,   //!< Exclusive type family - subtype of Z1_VT_SPECIAL
    Z1_VT_FIELD             = 0x00003800,   //!< Field vertex

    Z1_RESERVED17           = 0x00004000,   //!< reserved
    Z1_RESERVED16           = 0x00008000,   //!< reserved
    Z1_RESERVED15           = 0x00010000,   //!< reserved
    Z1_RESERVED14           = 0x00020000,   //!< reserved
    Z1_RESERVED13           = 0x00040000,   //!< reserved
    Z1_RESERVED12           = 0x00080000,   //!< reserved
    Z1_RESERVED11           = 0x00100000,   //!< reserved
    Z1_RESERVED10           = 0x00200000,   //!< reserved
    Z1_RESERVED09           = 0x00400000,   //!< reserved
    Z1_RESERVED08           = 0x00800000,   //!< reserved

    Z1_RESERVED07           = 0x01000000,   //!< reserved
    Z1_RESERVED06           = 0x02000000,   //!< reserved
    Z1_RESERVED05           = 0x04000000,   //!< reserved
    Z1_RESERVED04           = 0x08000000,   //!< reserved
    Z1_RESERVED03           = 0x10000000,   //!< reserved
    Z1_RESERVED02           = 0x20000000,   //!< reserved
    Z1_RESERVED01           = 0x40000000,   //!< reserved
    Z1_RESERVED00           = 0x80000000    //!< reserved
    
//    Z1_IS_SPECIAL =             0x0002,     //!< this vertex is a special vertex
//    Z1_IS_TYPE =                0x0004,     //!< this vertex is a type (particular case of special vertex)
//    Z1_IS_EDGE_TYPE =           0x0008,     //!< this vertex is an edge type (particular case of type vertex)
//    Z1_ET_IS_VISIBLE =          0x00000040,     //!< For edge types - indicate visibility to the end user in normal circumstances
//    Z1_EXCL_TYPE_FAMILY =       0x00000080,     //!< For special vertices who are the roots of ETFs
//    Z1_VT_MERGE_FIELDS =        0x00000100,     //!< Upon merge, fields of same type are merged
//    Z1_VT_MERGE_FORBIDDEN =     0x00000200,     //!< Vertices of this type cannot be merged
//    Z1_VT_MATERIALIZE_ITO =     0x00000400,     //!< When this type is given to a vertex, must an ITO edge be created from the type vertex

//    RWF_MAIN            = 0x00400000, //!< is the main row, grey background - \b R
//    RWF_SELECTED        = 0x00800000, //!< selected edge - \b R
//    RWF_EDITABLE        = 0x01000000, //!< edge can be edited if in edit mode - \b R
//    RWF_SELECTABLE      = 0x02000000, //!< selectable : a selector will appear in row selector column - \b R
//    RWF_NOETMODIF       = 0x04000000, //!< edge type cannot be changed - \b R
//    RWF_NOVTYPE         = 0x08000000, //!< no vertex type (thus, no icon drawn) - \b R
//    RWF_ISDEFAULT       = 0x10000000, //!< for email & postal addresses, indicates default value - \b R
//    RWF_ETMESSAGE       = 0x20000000, //!< edge type selection limited to message relevant values - \b R
//    RWF_NEVEREDIT       = 0x40000000, //!< edge cannot be edited whatever the circumstances - \b R
//    RWF_STRIKE          = 0x80000000  //!< striked out row text - \b R
};
#endif //__Z1_TYPES__