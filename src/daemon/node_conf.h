/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/



#ifndef _NODE_CONF_H_
#define _NODE_CONF_H_

#define TREE_WITH	5
#define MAX_NODENAME_PATH 128
typedef struct node_conf{
	char *nodename;
}node_conf_t;

/** This function reads the list of nodes from the file and creates a node list, memory is allocated and must be released
 * with node_list_dispose function
*/
node_conf_t *read_node_list(char *node_conf_file);
/** releases the memory previously allocated with read_node_list and/or create_forward_list function
 */
void node_list_dispose(node_conf_t * node_list);

/** Based on TREE_WITH, creates a list of nodes where messages must be forwarded (null if this node is a leaf). It allocates memory
 */
node_conf_t *create_forward_list(node_conf_t * system_nodelist);


#else
#endif
