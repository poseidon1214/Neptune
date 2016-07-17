/**
 *   Copyright (C) XXX. All rights reserved.

 *   \file     mod_base.h
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Apr 2011
 *   \version  1.0.0
 *   \brief    Interface of Module Base Definition
 */

#ifndef __MOD_BASE_H__
#define __MOD_BASE_H__

#include <stdint.h>

#if !defined(__cplusplus) && defined(_MSC_VER)
#define inline  __inline
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define MOD_TRUE   ((mod_bool_t)1)
#define MOD_FALSE  ((mod_bool_t)0)

    typedef int8_t          mod_s8_t;
    typedef uint8_t         mod_u8_t;
    typedef int16_t         mod_s16_t;
    typedef uint16_t        mod_u16_t;
    typedef int32_t         mod_s32_t;
    typedef uint32_t        mod_u32_t;
    typedef int64_t         mod_s64_t;
    typedef uint64_t        mod_u64_t;
    typedef intptr_t        mod_sptr_t;
    typedef uintptr_t       mod_uptr_t;
    typedef uint8_t         mod_byte_t;
    typedef const uint8_t   mod_cbyte_t;
    typedef char            mod_char_t;
    typedef const char      mod_cchar_t;
    typedef short           mod_short_t;
    typedef int             mod_int_t;
    typedef long            mod_long_t;
    typedef unsigned char   mod_uchar_t;
    typedef unsigned short  mod_ushort_t;
    typedef unsigned int    mod_uint_t;
    typedef unsigned long   mod_ulong_t;
    typedef void *          mod_any_t;
    typedef char            mod_bool_t;
    typedef unsigned int    mod_size_t;
    typedef int             mod_ssize_t;
    typedef long            mod_time_t;
    typedef const char *    mod_name_t;
    typedef int             mod_pid_t;

#if defined(__cplusplus)
} /* extern "C" */
#endif

#if defined(__cplusplus)
#include "mod_base.hh"  /* includes C++ interface */
#endif

#endif /*__MOD_BASE_H__*/
