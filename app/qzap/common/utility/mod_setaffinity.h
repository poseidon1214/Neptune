/**
 *   Copyright (C) RainVan(Yunfeng.Xiao). All rights reserved.

 *   \file     mod_setaffinity.h
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Jan 2013
 *   \version  1.0.0
 *   \brief    Interface of Setting CPU affinity of Process
 */

#ifndef __QZAP_MOD_SETAFFINITY_H__
#define __QZAP_MOD_SETAFFINITY_H__
#include <stdint.h>

/**
 *  \brief        Set CPU affinity of a process
 *  \param pid    The ID of process. Zero means the current process.
 *  \param mask   The mask of CPU affinity
 *  \return       0 indicates success, -1 indicates failure.
 */
int mod_setaffinity(int pid, uint64_t mask);

/**
 *  \brief        Bind a CPU with a process
 *  \param pid    The ID of process. Zero means the current process.
 *  \param id     The id of CPU (from zero)
 *  \return       0 indicates success, -1 indicates failure.
 */
int mod_bindaffinity(int pid, int id);


#endif /*__QZAP_MOD_SETAFFINITY_H__*/

