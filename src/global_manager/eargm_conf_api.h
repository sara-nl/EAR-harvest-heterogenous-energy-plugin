/**
 * Copyright © 2017-present BSC-Lenovo
 *
 * This file is licensed under both the BSD-3 license for individual/non-commercial
 * use and EPL-1.0 license for commercial use. Full text of both licenses can be
 * found in COPYING.BSD and COPYING.EPL files.
 */

/**
*    \file remote_daemon_client.h
*    \brief This file defines the client side of the remote EAR API
*
* 	 Note:Specific functions could be substituted by a generic function passing a local_config_t
*/

#ifndef _EARGM_COMMON_API_H
#define _EARGM_COMMON_API_H

#define EARGM_NEW_JOB	100
#define EARGM_END_JOB	200
#define NO_COMMAND		100000

typedef struct eargm_request
{
    uint req;
    uint num_nodes;
} eargm_request_t;

#endif
