/**
 *   Copyright (C) XXX. All rights reserved.

 *   \file     mod_hashtab.h
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Apr 2012
 *   \version  1.0.0
 *   \brief    Interface of HASH Table
 */

#ifndef __MOD_HASHTAB_H__
#define __MOD_HASHTAB_H__

#include "app/qzap/common/mod_json/include/mod_base.h"
#include "app/qzap/common/mod_json/include/mod_list.h"

#if defined(__cplusplus)
extern "C" {
#endif

    /*! Compare interface, zero-result indicates equal.
     */
    typedef int (*mod_hashcomp_proc)(mod_any_t elem1, mod_any_t elem2);

    /*! Hash interface
     */
    typedef mod_size_t (*mod_hashcalc_proc)(mod_any_t elem);

    /*! Cleanup interface
     */
    typedef void (*mod_hashclean_proc)(mod_any_t elem, mod_any_t arg);

    typedef mod_hlist_node_t    mod_hashnode_t;
    typedef mod_hlist_head_t    mod_hashhead_t;
    typedef struct mod_hashtab  mod_hashtab_t;

    /*! Hash table
     */
    struct mod_hashtab
    {
        mod_hashhead_t     *bucket;
        mod_size_t          size;
        mod_size_t          offset;
        mod_hashcomp_proc   comp;
        mod_hashcalc_proc   hash;
        mod_hashclean_proc  clean;
    };

    /**
     *  \brief           Initialize an empty hash table 
     *  \param htab      The pointer of hash table object
     *  \param bucket    The pointer of bucket array
     *  \param size      The size of bucket array
     *  \param offset    The offset of link node in structure
     *  \return          0 indicates success, -1 indicates failure.
     */
    int mod_hashtab_init(
        mod_hashtab_t *htab, mod_hashhead_t *bucket, 
        mod_size_t size, mod_size_t offset
        );

    /**
     *  \brief           Clean up a hash table 
     *  \param htab      The pointer of hash table object
     */
    void mod_hashtab_cleanup(mod_hashtab_t *htab, ...);

    /**
     *  \brief           Insert an element into hash table 
     *  \param htab      The pointer of hash table object
     *  \param elem      The pointer of hash element
     *  \return          If the element exists in table, the operation 
                         shall fail. 
                         0 indicates success, -1 indicates failure.
     */
    int mod_hashtab_insert(mod_hashtab_t *htab, mod_any_t elem);

    /**
     *  \brief           Assign a new element in hash table 
     *  \param htab      The pointer of hash table object
     *  \param elem      The pointer of hash element
     *  \return          If the element exists in table, it shall erase the 
                         old one, then insert a new one. 
                         0 indicates success, -1 indicates failure.
     */
    int mod_hashtab_assign(mod_hashtab_t *htab, mod_any_t elem, ...);

    /**
     *  \brief           Erase an element from hash table
     *  \param htab      The pointer of hash table object
     *  \param elem      The pointer of hash element
     */
    void mod_hashtab_erase(mod_hashtab_t *htab, mod_any_t elem, ...);

    /**
     *  \brief           Find an element in hash table
     *  \param htab      The pointer of hash table object
     *  \param key       The key of element
     *  \return          The element, nil indicates no found.
     */
    mod_any_t mod_hashtab_find(mod_hashtab_t *htab, mod_any_t key);

    /**
     *  \brief           Rehash all elements into a new bucket list 
     *  \param htab      The pointer of hash table object
     *  \param bucket    The pointer of new bucket array
     *  \param size      The size of new bucket array
     *  \return          0 indicates success, -1 indicates failure.
                         If successful, it will update the pointer and 
                         the size of bucket in hash table.
     */
    int mod_hashtab_rehash(
        mod_hashtab_t *htab, mod_hashhead_t *bucket, mod_size_t size
        );

    /**
     *  \brief           Retrieve bucket pointer of a hash table
     *  \param htab      The pointer of hash table object
     *  \return          The pointer of bucket 
     */
    static inline
    mod_hashhead_t *mod_hashtab_bucket(const mod_hashtab_t *htab)
    {
        return (htab->bucket);
    }

    /**
     *  \brief           Retrieve bucket size of a hash table
     *  \param htab      The pointer of hash table object
     *  \return          The size of bucket 
     */
    static inline
    mod_size_t mod_hashtab_bucket_size(const mod_hashtab_t *htab)
    {
        return (htab->size);
    }

    /**
     *  \brief           Set a user comparer for hash table
     *  \param htab      The pointer of hash table object
     *  \param comp      The callback function of comparer
     */
    static inline
    void mod_hashtab_set_comparer(mod_hashtab_t *htab, mod_hashcomp_proc comp)
    {
        htab->comp = comp;
    }

    /**
     *  \brief           Set a user hasher for hash table
     *  \param htab      The pointer of hash table object
     *  \param hash      The callback function of hasher
     */
    static inline
    void mod_hashtab_set_hasher(mod_hashtab_t *htab, mod_hashcalc_proc hash)
    {
        htab->hash = hash;
    }

    /**
     *  \brief           Set a user cleaner for hash table
     *  \param htab      The pointer of hash table object
     *  \param clean     The callback function of cleaner (can be null)
     */
    static inline
    void mod_hashtab_set_cleaner(mod_hashtab_t *htab, mod_hashclean_proc clean)
    {
        htab->clean = clean;
    }

    /**
     *  \brief           Retrieve the struct for this entry
     *  \param ptr       The pointer of hash table node
     *  \param type      The type of struct
     *  \param mem       The member of struct
     */
#   define mod_hashtab_entry(ptr, type, mem)  \
        (type*)((char*)ptr - (long)&(((type*)0)->mem))

    /**
     *  \brief           Enumerate all nodes in a bucket list
     */
#   define mod_hashlist_for_each       mod_hlist_for_each

    /**
     *  \brief           Enumerate all nodes in a bucket list safely
     */
#   define mod_hashlist_for_each_safe  mod_hlist_for_each_safe

    /**
     *  \brief           Enumerate all buckets in a hash table
     */
#   define mod_hashhead_for_each(htab, pos)  \
        for (pos = (htab)->bucket; pos != (htab)->bucket + (htab)->size; ++pos)

    /**
     *  \brief           Enumerate all nodes in a hash table
     */
#   define mod_hashtab_for_each(htab, head, pos)  \
        mod_hashhead_for_each(htab, head)         \
        mod_hashlist_for_each(head, pos)

    /**
     *  \brief           Enumerate all nodes in a hash table safely
     */
#   define mod_hashtab_for_each_safe(htab, head, pos, tmp)  \
        mod_hashhead_for_each(htab, head)                   \
        mod_hashlist_for_each_safe(head, pos, tmp)


#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /*__MOD_HASHTAB_H__*/
