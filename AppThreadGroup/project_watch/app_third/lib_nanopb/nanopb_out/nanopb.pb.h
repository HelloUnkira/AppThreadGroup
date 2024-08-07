/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.7 */

#ifndef PB_NANOPB_PB_H_INCLUDED
#define PB_NANOPB_PB_H_INCLUDED
#include <pb.h>
#include "descriptor.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _FieldType {
    FieldType_FT_DEFAULT = 0, /* Automatically decide field type, generate static field if possible. */
    FieldType_FT_CALLBACK = 1, /* Always generate a callback field. */
    FieldType_FT_POINTER = 4, /* Always generate a dynamically allocated field. */
    FieldType_FT_STATIC = 2, /* Generate a static field or raise an exception if not possible. */
    FieldType_FT_IGNORE = 3, /* Ignore the field completely. */
    FieldType_FT_INLINE = 5 /* Legacy option, use the separate 'fixed_length' option instead */
} FieldType;

typedef enum _IntSize {
    IntSize_IS_DEFAULT = 0, /* Default, 32/64bit based on type in .proto */
    IntSize_IS_8 = 8,
    IntSize_IS_16 = 16,
    IntSize_IS_32 = 32,
    IntSize_IS_64 = 64
} IntSize;

typedef enum _TypenameMangling {
    TypenameMangling_M_NONE = 0, /* Default, no typename mangling */
    TypenameMangling_M_STRIP_PACKAGE = 1, /* Strip current package name */
    TypenameMangling_M_FLATTEN = 2, /* Only use last path component */
    TypenameMangling_M_PACKAGE_INITIALS = 3 /* Replace the package name by the initials */
} TypenameMangling;

typedef enum _DescriptorSize {
    DescriptorSize_DS_AUTO = 0, /* Select minimal size based on field type */
    DescriptorSize_DS_1 = 1, /* 1 word; up to 15 byte fields, no arrays */
    DescriptorSize_DS_2 = 2, /* 2 words; up to 4095 byte fields, 4095 entry arrays */
    DescriptorSize_DS_4 = 4, /* 4 words; up to 2^32-1 byte fields, 2^16-1 entry arrays */
    DescriptorSize_DS_8 = 8 /* 8 words; up to 2^32-1 entry arrays */
} DescriptorSize;

/* Struct definitions */
/* This is the inner options message, which basically defines options for
 a field. When it is used in message or file scope, it applies to all
 fields. */
typedef struct _NanoPBOptions {
    /* Allocated size for 'bytes' and 'string' fields.
 For string fields, this should include the space for null terminator. */
    bool has_max_size;
    int32_t max_size;
    /* Allocated number of entries in arrays ('repeated' fields) */
    bool has_max_count;
    int32_t max_count;
    /* Force type of field (callback or static allocation) */
    bool has_type;
    FieldType type;
    /* Use long names for enums, i.e. EnumName_EnumValue. */
    bool has_long_names;
    bool long_names;
    /* Add 'packed' attribute to generated structs.
 Note: this cannot be used on CPUs that break on unaligned
 accesses to variables. */
    bool has_packed_struct;
    bool packed_struct;
    /* Skip this message */
    bool has_skip_message;
    bool skip_message;
    /* Size of integer fields. Can save some memory if you don't need
 full 32 bits for the value. */
    bool has_int_size;
    IntSize int_size;
    /* Generate oneof fields as normal optional fields instead of union. */
    bool has_no_unions;
    bool no_unions;
    /* integer type tag for a message */
    bool has_msgid;
    uint32_t msgid;
    /* Add 'packed' attribute to generated enums. */
    bool has_packed_enum;
    bool packed_enum;
    /* decode oneof as anonymous union */
    bool has_anonymous_oneof;
    bool anonymous_oneof;
    /* Proto3 singular field does not generate a "has_" flag */
    bool has_proto3;
    bool proto3;
    /* Generate an enum->string mapping function (can take up lots of space). */
    bool has_enum_to_string;
    bool enum_to_string;
    /* Maximum length for 'string' fields. Setting this is equivalent
 to setting max_size to a value of length+1. */
    bool has_max_length;
    int32_t max_length;
    /* Generate bytes arrays with fixed length */
    bool has_fixed_length;
    bool fixed_length;
    /* Generate repeated field with fixed count */
    bool has_fixed_count;
    bool fixed_count;
    /* Shorten or remove package names from type names.
 This option applies only on the file level. */
    bool has_mangle_names;
    TypenameMangling mangle_names;
    /* Data type for storage associated with callback fields. */
    pb_callback_t callback_datatype;
    /* Callback function used for encoding and decoding.
 Prior to nanopb-0.4.0, the callback was specified in per-field pb_callback_t
 structure. This is still supported, but does not work inside e.g. oneof or pointer
 fields. Instead, a new method allows specifying a per-message callback that
 will be called for all callback fields in a message type. */
    pb_callback_t callback_function;
    /* Select the size of field descriptors. This option has to be defined
 for the whole message, not per-field. Usually automatic selection is
 ok, but if it results in compilation errors you can increase the field
 size here. */
    bool has_descriptorsize;
    DescriptorSize descriptorsize;
    /* Force proto3 messages to have no "has_" flag.
 This was default behavior until nanopb-0.4.0. */
    bool has_proto3_singular_msgs;
    bool proto3_singular_msgs;
    /* Generate message-level callback that is called before decoding submessages.
 This can be used to set callback fields for submsgs inside oneofs. */
    bool has_submsg_callback;
    bool submsg_callback;
    /* Set default value for has_ fields. */
    bool has_default_has;
    bool default_has;
    /* Extra files to include in generated `.pb.h` */
    pb_callback_t include;
    /* Package name that applies only for nanopb. */
    pb_callback_t package;
    /* Automatic includes to exclude from generated `.pb.h`
 Same as nanopb_generator.py command line flag -x. */
    pb_callback_t exclude;
    /* Override type of the field in generated C code. Only to be used with related field types */
    bool has_type_override;
    google_protobuf_FieldDescriptorProto_Type type_override;
    /* Due to historical reasons, nanopb orders fields in structs by their tag number
 instead of the order in .proto. Set this to false to keep the .proto order.
 The default value will probably change to false in nanopb-0.5.0. */
    bool has_sort_by_tag;
    bool sort_by_tag;
    /* Set the FT_DEFAULT field conversion strategy.
 A field that can become a static member of a c struct (e.g. int, bool, etc)
 will be a a static field.
 Fields with dynamic length are converted to either a pointer or a callback. */
    bool has_fallback_type;
    FieldType fallback_type;
} NanoPBOptions;


/* Extensions */
extern const pb_extension_type_t nanopb_fileopt; /* field type: NanoPBOptions nanopb_fileopt; */
extern const pb_extension_type_t nanopb_msgopt; /* field type: NanoPBOptions nanopb_msgopt; */
extern const pb_extension_type_t nanopb_enumopt; /* field type: NanoPBOptions nanopb_enumopt; */
extern const pb_extension_type_t nanopb; /* field type: NanoPBOptions nanopb; */

#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _FieldType_MIN FieldType_FT_DEFAULT
#define _FieldType_MAX FieldType_FT_INLINE
#define _FieldType_ARRAYSIZE ((FieldType)(FieldType_FT_INLINE+1))

#define _IntSize_MIN IntSize_IS_DEFAULT
#define _IntSize_MAX IntSize_IS_64
#define _IntSize_ARRAYSIZE ((IntSize)(IntSize_IS_64+1))

#define _TypenameMangling_MIN TypenameMangling_M_NONE
#define _TypenameMangling_MAX TypenameMangling_M_PACKAGE_INITIALS
#define _TypenameMangling_ARRAYSIZE ((TypenameMangling)(TypenameMangling_M_PACKAGE_INITIALS+1))

#define _DescriptorSize_MIN DescriptorSize_DS_AUTO
#define _DescriptorSize_MAX DescriptorSize_DS_8
#define _DescriptorSize_ARRAYSIZE ((DescriptorSize)(DescriptorSize_DS_8+1))

#define NanoPBOptions_type_ENUMTYPE FieldType
#define NanoPBOptions_int_size_ENUMTYPE IntSize
#define NanoPBOptions_mangle_names_ENUMTYPE TypenameMangling
#define NanoPBOptions_descriptorsize_ENUMTYPE DescriptorSize
#define NanoPBOptions_type_override_ENUMTYPE google_protobuf_FieldDescriptorProto_Type
#define NanoPBOptions_fallback_type_ENUMTYPE FieldType


/* Initializer values for message structs */
#define NanoPBOptions_init_default               {false, 0, false, 0, false, FieldType_FT_DEFAULT, false, true, false, false, false, false, false, IntSize_IS_DEFAULT, false, false, false, 0, false, false, false, false, false, false, false, false, false, 0, false, false, false, false, false, TypenameMangling_M_NONE, {{NULL}, NULL}, {{NULL}, NULL}, false, DescriptorSize_DS_AUTO, false, false, false, false, false, false, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, false, _google_protobuf_FieldDescriptorProto_Type_MIN, false, true, false, FieldType_FT_CALLBACK}
#define NanoPBOptions_init_zero                  {false, 0, false, 0, false, _FieldType_MIN, false, 0, false, 0, false, 0, false, _IntSize_MIN, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, _TypenameMangling_MIN, {{NULL}, NULL}, {{NULL}, NULL}, false, _DescriptorSize_MIN, false, 0, false, 0, false, 0, {{NULL}, NULL}, {{NULL}, NULL}, {{NULL}, NULL}, false, _google_protobuf_FieldDescriptorProto_Type_MIN, false, 0, false, _FieldType_MIN}

/* Field tags (for use in manual encoding/decoding) */
#define NanoPBOptions_max_size_tag               1
#define NanoPBOptions_max_count_tag              2
#define NanoPBOptions_type_tag                   3
#define NanoPBOptions_long_names_tag             4
#define NanoPBOptions_packed_struct_tag          5
#define NanoPBOptions_skip_message_tag           6
#define NanoPBOptions_int_size_tag               7
#define NanoPBOptions_no_unions_tag              8
#define NanoPBOptions_msgid_tag                  9
#define NanoPBOptions_packed_enum_tag            10
#define NanoPBOptions_anonymous_oneof_tag        11
#define NanoPBOptions_proto3_tag                 12
#define NanoPBOptions_enum_to_string_tag         13
#define NanoPBOptions_max_length_tag             14
#define NanoPBOptions_fixed_length_tag           15
#define NanoPBOptions_fixed_count_tag            16
#define NanoPBOptions_mangle_names_tag           17
#define NanoPBOptions_callback_datatype_tag      18
#define NanoPBOptions_callback_function_tag      19
#define NanoPBOptions_descriptorsize_tag         20
#define NanoPBOptions_proto3_singular_msgs_tag   21
#define NanoPBOptions_submsg_callback_tag        22
#define NanoPBOptions_default_has_tag            23
#define NanoPBOptions_include_tag                24
#define NanoPBOptions_package_tag                25
#define NanoPBOptions_exclude_tag                26
#define NanoPBOptions_type_override_tag          27
#define NanoPBOptions_sort_by_tag_tag            28
#define NanoPBOptions_fallback_type_tag          29
#define nanopb_fileopt_tag                       1010
#define nanopb_msgopt_tag                        1010
#define nanopb_enumopt_tag                       1010
#define nanopb_tag                               1010

/* Struct field encoding specification for nanopb */
#define NanoPBOptions_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, INT32,    max_size,          1) \
X(a, STATIC,   OPTIONAL, INT32,    max_count,         2) \
X(a, STATIC,   OPTIONAL, UENUM,    type,              3) \
X(a, STATIC,   OPTIONAL, BOOL,     long_names,        4) \
X(a, STATIC,   OPTIONAL, BOOL,     packed_struct,     5) \
X(a, STATIC,   OPTIONAL, BOOL,     skip_message,      6) \
X(a, STATIC,   OPTIONAL, UENUM,    int_size,          7) \
X(a, STATIC,   OPTIONAL, BOOL,     no_unions,         8) \
X(a, STATIC,   OPTIONAL, UINT32,   msgid,             9) \
X(a, STATIC,   OPTIONAL, BOOL,     packed_enum,      10) \
X(a, STATIC,   OPTIONAL, BOOL,     anonymous_oneof,  11) \
X(a, STATIC,   OPTIONAL, BOOL,     proto3,           12) \
X(a, STATIC,   OPTIONAL, BOOL,     enum_to_string,   13) \
X(a, STATIC,   OPTIONAL, INT32,    max_length,       14) \
X(a, STATIC,   OPTIONAL, BOOL,     fixed_length,     15) \
X(a, STATIC,   OPTIONAL, BOOL,     fixed_count,      16) \
X(a, STATIC,   OPTIONAL, UENUM,    mangle_names,     17) \
X(a, CALLBACK, OPTIONAL, STRING,   callback_datatype,  18) \
X(a, CALLBACK, OPTIONAL, STRING,   callback_function,  19) \
X(a, STATIC,   OPTIONAL, UENUM,    descriptorsize,   20) \
X(a, STATIC,   OPTIONAL, BOOL,     proto3_singular_msgs,  21) \
X(a, STATIC,   OPTIONAL, BOOL,     submsg_callback,  22) \
X(a, STATIC,   OPTIONAL, BOOL,     default_has,      23) \
X(a, CALLBACK, REPEATED, STRING,   include,          24) \
X(a, CALLBACK, OPTIONAL, STRING,   package,          25) \
X(a, CALLBACK, REPEATED, STRING,   exclude,          26) \
X(a, STATIC,   OPTIONAL, UENUM,    type_override,    27) \
X(a, STATIC,   OPTIONAL, BOOL,     sort_by_tag,      28) \
X(a, STATIC,   OPTIONAL, UENUM,    fallback_type,    29)
#define NanoPBOptions_CALLBACK pb_default_field_callback
#define NanoPBOptions_DEFAULT (const pb_byte_t*)"\x20\x01\x28\x00\x30\x00\x40\x00\x50\x00\x58\x00\x60\x00\x68\x00\x78\x00\x80\x01\x00\xa8\x01\x00\xb0\x01\x00\xb8\x01\x00\xd8\x01\x01\xe0\x01\x01\xe8\x01\x01\x00"

extern const pb_msgdesc_t NanoPBOptions_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define NanoPBOptions_fields &NanoPBOptions_msg

/* Maximum encoded size of messages (where known) */
/* NanoPBOptions_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
