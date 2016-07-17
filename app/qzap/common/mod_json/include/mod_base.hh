/**
 *   Copyright (C) XXX. All rights reserved.

 *   \file     mod_base.hh
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Jul 2013
 *   \version  1.0.0
 *   \brief    Interface of Module Base Definition (C++)
 */

#ifndef __MOD_BASE_HH__
#define __MOD_BASE_HH__

#ifndef MODFRAME_NS
#   define MODFRAME_NS        mod
#endif
#ifndef MODFRAME_NS_BEGIN
#   define MODFRAME_NS_BEGIN  namespace mod {
#endif
#ifndef MODFRAME_NS_END
#   define MODFRAME_NS_END    }
#endif
#ifndef MODFRAME_NS_USING
#   define MODFRAME_NS_USING  using namespace MODFRAME_NS;
#endif

MODFRAME_NS_BEGIN

typedef mod_s8_t      s8_type;
typedef mod_u8_t      u8_type;
typedef mod_s16_t     s16_type;
typedef mod_u16_t     u16_type;
typedef mod_s32_t     s32_type;
typedef mod_u32_t     u32_type;
typedef mod_s64_t     s64_type;
typedef mod_u64_t     u64_type;
typedef mod_sptr_t    sptr_type;
typedef mod_uptr_t    uptr_type;
typedef mod_byte_t    byte_type;
typedef mod_cbyte_t   cbyte_type;
typedef mod_char_t    char_type;
typedef mod_cchar_t   cchar_type;
typedef mod_short_t   short_type;
typedef mod_int_t     int_type;
typedef mod_long_t    long_type;
typedef mod_uchar_t   uchar_type;
typedef mod_ushort_t  ushort_type;
typedef mod_uint_t    uint_type;
typedef mod_ulong_t   ulong_type;
typedef mod_any_t     any_type;
typedef mod_bool_t    bool_type;
typedef mod_size_t    size_type;
typedef mod_ssize_t   ssize_type;
typedef mod_time_t    time_type;
typedef mod_name_t    name_type;
typedef mod_pid_t     pid_type;

MODFRAME_NS_END

#endif //__MOD_BASE_HH__
