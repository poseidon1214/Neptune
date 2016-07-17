/**
 *   Copyright (C) XXX. All rights reserved.

 *   \file     mod_hashtab.c
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Apr 2012
 *   \version  1.0.0
 *   \brief    Implementation of HASH Table
 */

#include "app/qzap/common/mod_json/include/mod_hashtab.h"
#include <string.h>
#include <stdarg.h>

/*! Convert element to node in table
 */
#define mod_hashtab_node(elem, off)  \
    ((mod_hashnode_t*)((long)(elem) + (off)))

/*! Convert node to element in table
 */
#define mod_hashtab_element(node, off)  \
    ((mod_any_t)((long)(node) - (off)))

/*! Insert a new node into a list
 */
#define mod_hashlist_insert  mod_hlist_insert

/*! Remove a node from a list
 */
#define mod_hashlist_erase   mod_hlist_erase

static
int mod_hashcomp_def(mod_any_t elem1, mod_any_t elem2)
{
    return (int)((long)elem1 - (long)elem2);
}

static
mod_size_t mod_hashcalc_def(mod_any_t elem)
{
    return (mod_size_t)(long)elem;
}

static inline
mod_any_t mod_hashlist_find(mod_hashhead_t *head, mod_any_t elem, 
                            mod_size_t offset, mod_hashcomp_proc comp)
{
    mod_hashnode_t *node;

    mod_hashlist_for_each(head, node)
    {
        mod_any_t cur = mod_hashtab_element(node, offset);

        if (comp(cur, elem) == 0)
        {
            return cur;
        }
    }
    return (mod_any_t)0;
}

static inline
void mod_hashlist_cleanup(mod_hashhead_t *head, mod_size_t offset, 
                          mod_hashclean_proc clean, mod_any_t arg)
{
    mod_hashnode_t *node, *temp;

    mod_hashlist_for_each_safe(head, node, temp)
    {
        clean(mod_hashtab_element(node, offset), arg);
    }
}

int mod_hashtab_init(mod_hashtab_t *htab, mod_hashhead_t *bucket, 
                     mod_size_t size, mod_size_t offset)
{
    if (!htab || !bucket || !size)
    {
        return -1;
    }

    (void)memset(bucket, 0, size * sizeof(mod_hashhead_t));
    htab->bucket = bucket;
    htab->size = size;
    htab->offset = offset;
    htab->comp = mod_hashcomp_def;
    htab->hash = mod_hashcalc_def;
    htab->clean = (mod_hashclean_proc)0;
    return 0;
}

void mod_hashtab_cleanup(mod_hashtab_t *htab, ...)
{
    if (htab && htab->bucket && htab->clean)
    {
        mod_hashhead_t *iter, *end;
        mod_any_t arg;
        va_list list;

        va_start(list, htab);
        iter = htab->bucket;
        end = htab->bucket + htab->size;
        arg = va_arg(list, mod_any_t);
        for ( ; iter != end; ++iter)
        {
            /* clean up */
            mod_hashlist_cleanup(iter, htab->offset, htab->clean, arg);
        }
        va_end(list);
    }
}

int mod_hashtab_insert(mod_hashtab_t *htab, mod_any_t elem)
{
    if (htab && htab->bucket && elem)
    {
        mod_hashhead_t *head;

        /* which bucket? */
        head = htab->bucket + htab->hash(elem) % htab->size;

        if (!mod_hashlist_find(head, elem, htab->offset, htab->comp))
        {
            mod_hashlist_insert(head, mod_hashtab_node(elem, htab->offset));
            return 0;
        }
    }
    return -1;
}

int mod_hashtab_assign(mod_hashtab_t *htab, mod_any_t elem, ...)
{
    mod_hashhead_t *head;
    mod_any_t temp;

    if (!htab || !htab->bucket || !elem)
    {
        return -1;
    }

    /* which bucket? */
    head = htab->bucket + htab->hash(elem) % htab->size;

    temp = mod_hashlist_find(head, elem, htab->offset, htab->comp);
    if (temp)
    {
        /* delete it first */
        mod_hashlist_erase(mod_hashtab_node(temp, htab->offset));

        if (htab->clean)
        {
            va_list list;

            /* clean up */
            va_start(list, elem);
            htab->clean(temp, va_arg(list, mod_any_t));
            va_end(list);
        }
    }

    /* insert into list */
    mod_hashlist_insert(head, mod_hashtab_node(elem, htab->offset));
    return 0;
}

void mod_hashtab_erase(mod_hashtab_t *htab, mod_any_t elem, ...)
{
    if (htab && elem)
    {
        mod_hashlist_erase(mod_hashtab_node(elem, htab->offset));

        if (htab->clean)
        {
            va_list list;

            /* clean up */
            va_start(list, elem);
            htab->clean(elem, va_arg(list, mod_any_t));
            va_end(list);
        }
    }
}

mod_any_t mod_hashtab_find(mod_hashtab_t *htab, mod_any_t key)
{
    if (!htab || !htab->bucket || !key)
    {
        return (mod_any_t)0;
    }

    return mod_hashlist_find(
        htab->bucket + (htab->hash(key) % htab->size), 
        key, htab->offset, htab->comp
        );
}

int mod_hashtab_rehash(mod_hashtab_t *htab, mod_hashhead_t *bucket, 
                       mod_size_t size)
{
    mod_hashhead_t *head;
    mod_hashnode_t *pos, *tmp;

    if (!htab || !bucket || !size)
    {
        return -1;
    }

    if (size == htab->size || bucket == htab->bucket)
    {
        return -1;
    }

    /* initialize as empty */
    (void)memset(bucket, 0, size * sizeof(mod_hashhead_t));

    mod_hashtab_for_each_safe(htab, head, pos, tmp)
    {
        mod_size_t hash;

        hash = htab->hash(mod_hashtab_element(pos, htab->offset));

        /* erase from old list */
        mod_hashlist_erase(pos);

        /* insert into new list */
        mod_hashlist_insert(bucket + hash % size, pos);
    }

    /* renew header */
    htab->bucket = bucket;
    htab->size = size;

    /* success */
    return 0;
}
