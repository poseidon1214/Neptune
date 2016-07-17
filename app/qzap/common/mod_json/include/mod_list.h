/**
 *   Copyright (C) XXX. All rights reserved.

 *   \file     mod_list.h
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Apr 2012
 *   \version  1.0.0
 *   \brief    Interface/Implementation of List Class
 */

#ifndef __MOD_LIST_H__
#define __MOD_LIST_H__

#include "app/qzap/common/mod_json/include/mod_base.h"

#if defined(__cplusplus)
extern "C" {
#endif

    /*! LIST HEAD
     */
    typedef struct mod_list_head mod_list_head_t;

    /*! Initialize LIST HEAD
     */
#   define MOD_LIST_HEAD_INIT(name) { &(name), &(name) }

    /*! Initialize LIST NODE
     */
#   define MOD_LIST_NODE_INIT() { 0, 0 }

    /*! LIST HEAD
     */
    struct mod_list_head 
    {
        mod_list_head_t  *next;
        mod_list_head_t  *prev;
    };

    /**
     *  \brief           Initialize an empty circular list
     *  \param head      The head of list
     */
    static inline
    void mod_list_head_init(mod_list_head_t *head)
    {
        head->next = head;
        head->prev = head;
    }

    /**
     *  \brief           Retrieve non-zero if a list is empty
     *  \param head      The head of list
     *  \return          1 indicates TRUE, 0 indicates FALSE
     */
    static inline
    mod_bool_t mod_list_empty(const mod_list_head_t *head)
    {
        return (head->next == head);
    }

    /**
     *  \brief           Retrieve next one of a list node
     *  \param node      The pointer of node
     *  \return          The pointer of next node
     */
    static inline
    mod_list_head_t *mod_list_next(mod_list_head_t *node)
    {
        return (node->next);
    }

    /**
     *  \brief           Retrieve previous one of a list node
     *  \param node      The pointer of node
     *  \return          The pointer of previous node
     */
    static inline
    mod_list_head_t *mod_list_prev(mod_list_head_t *node)
    {
        return (node->prev);
    }

    /**
     *  \brief           Retrieve the front of a list
     *  \param head      The head of list
     *  \return          The pointer of front node
     */
    static inline
    mod_list_head_t *mod_list_front(mod_list_head_t *head)
    {
        return (head != head->next ? head->next : 0);
    }

    /**
     *  \brief           Retrieve the back of a list
     *  \param head      The head of list
     *  \return          The pointer of back node
     */
    static inline
    mod_list_head_t *mod_list_back(mod_list_head_t *head)
    {
        return (head != head->prev ? head->prev : 0);
    }

    /**
     *  \brief           Add a new node at the end of the list
     *  \param head      The head of list
     *  \param node      The pointer of new node
     */
    static inline
    void mod_list_push_back(mod_list_head_t *head, mod_list_head_t *node)
    {
        mod_list_head_t *prev = head->prev;
        head->prev = node;
        node->next = head;
        node->prev = prev;
        prev->next = node;
    }

    /**
     *  \brief           Insert a new node at the beginning of the list
     *  \param head      The head of list
     *  \param node      The pointer of new node
     */
    static inline
    void mod_list_push_front(mod_list_head_t *head, mod_list_head_t *node)
    {
        mod_list_head_t *next = head->next;
        next->prev = node;
        node->next = next;
        node->prev = head;
        head->next = node;
    }

    /**
     *  \brief           Remove a node from the list
     *  \param node      The pointer of node
     */
    static inline
    void mod_list_erase(mod_list_head_t *node)
    {
        mod_list_head_t *next = node->next;
        mod_list_head_t *prev = node->prev;
        next->prev = prev;
        prev->next = next;
        node->next = (mod_list_head_t*)0;
        node->prev = (mod_list_head_t*)0;
    }

    /**
     *  \brief           Remove the last node from the list
     *  \param head      The head of list
     */
    static inline
    void mod_list_pop_back(mod_list_head_t *head)
    {
        mod_list_erase(head->prev);
    }

    /**
     *  \brief           Remove the first node from the list
     *  \param head      The head of list
     */
    static inline
    void mod_list_pop_front(mod_list_head_t *head)
    {
        mod_list_erase(head->next);
    }

    /**
     *  \brief           Delete from one list and add as another's tail
     *  \param head      The head of destination list
     *  \param node      The pointer of node
     */
    static inline
    void mod_list_move_back(mod_list_head_t *head, mod_list_head_t *node)
    {
        mod_list_erase(node);
        mod_list_push_back(head, node);
    }

    /**
     *  \brief           Delete from one list and add as another's front
     *  \param head      The head of destination list
     *  \param node      The pointer of node
     */
    static inline
    void mod_list_move_front(mod_list_head_t *head, mod_list_head_t *node)
    {
        mod_list_erase(node);
        mod_list_push_front(head, node);
    }

    /**
     *  \brief           Join two lists, not excluding list itself.
     *  \param head      The new list to merge
     *  \param list      The place to add it in the first list
     */
    static inline
    void mod_list_splice(mod_list_head_t *head, mod_list_head_t *list)
    {
        if (!mod_list_empty(list))
        {
            mod_list_head_t *first = list->next;
            mod_list_head_t *last = list->prev;
            mod_list_head_t *at = head->prev;

            last->next = head;
            head->prev = last;

            first->prev = at;
            at->next = first;

            list->next = list;
            list->prev = list;
        }
    }

    /**
     *  \brief           Retrieve the struct for this entry
     *  \param ptr       The pointer of list node
     *  \param type      The type of struct
     *  \param mem       The member of struct
     */
#   define mod_list_entry(ptr, type, mem)  \
        (type*)((char*)ptr - (long)&(((type*)0)->mem))

    /**
     *  \brief           Enumerate all nodes in a list
     */
#   define mod_list_for_each(head, pos)  \
        for (pos = (head)->next; pos != (head); pos = pos->next)

    /**
     *  \brief           Enumerate all nodes in a list safely
     */
#   define mod_list_for_each_safe(head, pos, tmp)  \
        for (pos = (head)->next, tmp = pos->next;  \
             pos != (head);  \
             pos = tmp, tmp = pos->next)

    /**
     *  \brief           Enumerate all nodes in a list
     */
#   define mod_list_for_each_prev(head, pos)  \
        for (pos = (head)->prev; pos != (head); pos = pos->prev)

    /**
     *  \brief           Enumerate all nodes in a list safely
     */
#   define mod_list_for_each_prev_safe(head, pos, tmp)  \
        for (pos = (head)->prev, tmp = pos->prev; \
             pos != (head);  \
             pos = tmp, tmp = pos->prev)

    /*! HLIST NODE
     */
    typedef struct mod_hlist_node mod_hlist_node_t;

    /*! HLIST HEAD
     */
    typedef struct mod_hlist_head mod_hlist_head_t;

    /*! Initialize LIST HEAD
     */
#   define MOD_HLIST_HEAD_INIT() { 0 }

    /*! Initialize LIST NODE
     */
#   define MOD_HLIST_NODE_INIT() { 0, 0 }

    /*! HLIST NODE
     */
    struct mod_hlist_node 
    {
        mod_hlist_node_t  *next;
        mod_hlist_node_t  **pprev;
    };

    /*! HLIST HEAD
     */
    struct mod_hlist_head 
    {
        mod_hlist_node_t  *first;
    };

    /**
     *  \brief           Initialize an empty singly list
     *  \param head      The head of list
     */
    static inline
    void mod_hlist_head_init(mod_hlist_head_t *head)
    {
        head->first = (mod_hlist_node_t*)0;
    }

    /**
     *  \brief           Initialize a singly list node
     *  \param node      The pointer of node
     */
    static inline
    void mod_hlist_node_init(mod_hlist_node_t *node)
    {
        node->next = (mod_hlist_node_t*)0;
        node->pprev = (mod_hlist_node_t**)0;
    }

    /**
     *  \brief           Retrieve non-zero if a list is empty
     *  \param head      The head of list
     *  \return          1 indicates TRUE, 0 indicates FALSE
     */
    static inline
    mod_bool_t mod_hlist_empty(const mod_hlist_head_t *head)
    {
        return (!head->first);
    }

    /**
     *  \brief           Retrieve first node of a list
     *  \param head      The head of list
     *  \return          The pointer of first node
     */
    static inline
    mod_hlist_node_t *mod_hlist_first(mod_hlist_head_t *head)
    {
        return (head->first);
    }

    /**
     *  \brief           Retrieve next one of a list node
     *  \param node      The pointer of node
     *  \return          The pointer of next node
     */
    static inline
    mod_hlist_node_t *mod_hlist_next(mod_hlist_node_t *node)
    {
        return (node->next);
    }

    /**
     *  \brief           Retrieve previous one of a list node
     *  \param node      The pointer of node
     *  \return          The pointer of previous node
     */
    static inline
    mod_hlist_node_t *mod_hlist_prev(mod_hlist_node_t *node)
    {
        return (mod_hlist_node_t*)node->pprev;
    }

    /**
     *  \brief           Insert a new node at the beginning of the list
     *  \param head      The head of list
     *  \param node      The pointer of new node
     */
    static inline
    void mod_hlist_insert(mod_hlist_head_t *head, mod_hlist_node_t *node)
    {
        mod_hlist_node_t *first = head->first;
        node->next = first;
        if (first)
        {
            first->pprev = &node->next;
        }
        head->first = node;
        node->pprev = &head->first;
    }

    /**
     *  \brief           Insert a new node in front of current node
     *  \param cur       The pointer of current node
     *  \param node      The pointer of new node
     */
    static inline
    void mod_hlist_add_before(mod_hlist_node_t *cur, mod_hlist_node_t *node)
    {
        node->pprev = cur->pprev;
        node->next = cur;
        cur->pprev = &node->next;
        *(cur->pprev) = node;
    }

    /**
     *  \brief           Insert a new node behind current node
     *  \param cur       The pointer of current node
     *  \param node      The pointer of new node
     */
    static inline
    void mod_hlist_add_after(mod_hlist_node_t *cur, mod_hlist_node_t *node)
    {
        node->next = cur->next;
        if (cur->next)
        {
            cur->next->pprev = &node->next;
        }
        cur->next = node;
        node->pprev = &cur->next;
    }

    /**
     *  \brief           Remove a node from the list
     *  \param node      The pointer of node
     */
    static inline
    void mod_hlist_erase(mod_hlist_node_t *node)
    {
        mod_hlist_node_t *next = node->next;
        mod_hlist_node_t **pprev = node->pprev;
        *pprev = next;
        if (next)
        {
            next->pprev = pprev;
        }
        node->next = (mod_hlist_node_t*)0;
        node->pprev = (mod_hlist_node_t**)0;
    }

    /**
     *  \brief           Delete from one list and add as another's front
     *  \param head      The head of destination list
     *  \param node      The pointer of node
     */
    static inline
    void mod_hlist_move(mod_hlist_head_t *head, mod_hlist_node_t *node)
    {
        mod_hlist_erase(node);
        mod_hlist_insert(head, node);
    }

    /**
     *  \brief           Retrieve the struct for this entry
     *  \param ptr       The pointer of list node
     *  \param type      The type of struct
     *  \param mem       The member of struct
     */
#   define mod_hlist_entry(ptr, type, mem)  \
        (type*)((char*)ptr - (long)&(((type*)0)->mem))

    /**
     *  \brief           Enumerate all nodes in a list
     */
#   define mod_hlist_for_each(head, pos)  \
        for (pos = (head)->first; pos; pos = pos->next)

    /**
     *  \brief           Enumerate all nodes in a list safely
     */
#   define mod_hlist_for_each_safe(head, pos, tmp)  \
        for (pos = (head)->first; pos && (tmp = pos->next, 1); pos = tmp)


#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /*__MOD_LIST_H__*/
