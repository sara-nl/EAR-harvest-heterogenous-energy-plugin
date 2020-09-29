/*
*
* This program is part of the EAR software.
*
* EAR provides a dynamic, transparent and ligth-weigth solution for
* Energy management. It has been developed in the context of the
* Barcelona Supercomputing Center (BSC)&Lenovo Collaboration project.
*
* Copyright © 2017-present BSC-Lenovo
* BSC Contact   mailto:ear-support@bsc.es
* Lenovo contact  mailto:hpchelp@lenovo.com
*
* This file is licensed under both the BSD-3 license for individual/non-commercial
* use and EPL-1.0 license for commercial use. Full text of both licenses can be
* found in COPYING.BSD and COPYING.EPL files.
*/

//#define SHOW_DEBUGS 1

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <common/config.h>
#include <common/states.h>
#include <common/types/job.h>
#include <common/output/verbose.h>

#include <daemon/remote_api/eard_rapi.h>
#include <daemon/remote_api/eard_conf_rapi.h>
#include <daemon/remote_api/eard_server_api.h>

int eards_remote_connected=0;
int eards_sfd=-1;

#if DYNAMIC_COMMANDS
size_t get_command_size(request_t *command, char **data_to_send)
{
    size_t size = sizeof(internal_request_t);
    size_t offset = 0;
    size_t aux_size = 0;
    char *command_b;

    switch(command->req)
    {
        case EAR_RC_SET_POWERCAP_OPT:
            size += sizeof(powercap_opt_t);
            size += command->my_req.pc_opt.num_greedy * sizeof(int) * 2;
            offset = command->my_req.pc_opt.num_greedy * sizeof(int);
            break;
        
        //NOTE: individual values are not being passed to keep the code simple. Instead, we measure the actual data size of the
        //union and send that instead. Ideally we would only copy the value that we need, but that would require a big re-write.

        case EAR_RC_MAX_FREQ:
        case EAR_RC_SET_FREQ:
        case EAR_RC_DEF_FREQ:
        case EAR_RC_INC_TH:
        case EAR_RC_RED_PSTATE:
        case EAR_RC_NEW_TH:
            size += sizeof(new_conf_t); //all this values use the union as ear_conf (a new_conf_t)
            break;
        case EAR_RC_SET_POWER:
        case EAR_RC_INC_POWER:
        case EAR_RC_RED_POWER:
            size += sizeof(power_limit_t); //powercap struct
            break;
        case EAR_RC_SET_RISK:
            size += sizeof(risk_dec_t); //risk struct
            break;
        case EAR_RC_SET_POLICY:
            size += sizeof(new_policy_cont_t); //new policy_conf
            break;
        case EAR_RC_NEW_JOB:
            size += sizeof(new_job_req_t);
            break;
        case EAR_RC_END_JOB:
            size += sizeof(end_job_req_t);
            break;
    }
 
#if NODE_PROP
    if (command->num_nodes > 0)
    {
        size += command->num_nodes * sizeof(int);
        aux_size = command->num_nodes * sizeof(int);
    }
#endif

    debug("allocating command with size %u", size);
    //copy the original command
    command_b = calloc(1, size);
    debug("copying internal_request_t");
    memcpy(command_b, command, sizeof(internal_request_t)); //the first portion of request_t and internal_request_t align
#if NODE_PROP
    debug("copying nodes (%d num_nodes)", command->num_nodes);
    int i;
    for (i = 0; i < command->num_nodes; i++)
        debug("node%d %d", i, command->nodes[i]);
    debug("total_size: %u nodes size %u starting size(internal_req_t size) %u", size, aux_size, sizeof(internal_request_t));
    memcpy(&command_b[sizeof(internal_request_t)], command->nodes, aux_size); //copy the nodes to propagate to
    debug("copied nodes");
#endif

    aux_size += sizeof(internal_request_t); //aux_size holds the position we have to start writing from

    switch(command->req)
    {
        case EAR_RC_SET_POWERCAP_OPT:
            //copy the base powercap_opt_t since we don't automatically copy anything beyond internal_request_t
            memcpy(&command_b[aux_size], &command->my_req, sizeof(powercap_opt_t)); 
            aux_size += sizeof(powercap_opt_t); //add the copied size to the index

            //proceed as with fixed size method, copying both arrays
            memcpy(&command_b[aux_size], command->my_req.pc_opt.greedy_nodes, offset);
            memcpy(&command_b[aux_size + offset], command->my_req.pc_opt.extra_power, offset);
            break;
        default:
            debug("copying additional data, total_size: %u, already copied %u", size, aux_size);
            memcpy(&command_b[aux_size], &command->my_req, size-aux_size); 
            break;
    }

    *data_to_send = command_b; 
    return size;

}
#else
size_t get_command_size(request_t *command, char **data_to_send)
{
    size_t size = sizeof(request_t);
    size_t offset = 0;
    char *command_b;


    switch(command->req)
    {
        case EAR_RC_SET_POWERCAP_OPT:
            size += command->my_req.pc_opt.num_greedy * sizeof(int) * 2;
            offset = command->my_req.pc_opt.num_greedy * sizeof(int);
            break;
    }

    //copy the original command
    command_b = calloc(1, size);
    memcpy(command_b, command, sizeof(request_t));

    switch(command->req)
    {
        case EAR_RC_SET_POWERCAP_OPT:
            memcpy(&command_b[sizeof(request_t)], command->my_req.pc_opt.greedy_nodes, offset);
            memcpy(&command_b[sizeof(request_t) + offset], command->my_req.pc_opt.extra_power, offset);
            break;
    }

    *data_to_send = command_b; 
    return size;
}
#endif

// Sends a command to eard
int send_command(request_t *command)
{
	ulong ack;
	int ret;
    size_t command_size;
    char *command_b;

	debug("Sending command %u",command->req);

    command_size = get_command_size(command, &command_b);
    debug("Command size: %u\t request_t size: %u", command_size, sizeof(request_t));

    
    if (send_non_block_data(eards_sfd, command_size, command_b, EAR_TYPE_COMMAND) != EAR_SUCCESS)
        error("Error sending command");

    free(command_b);

	ret=read(eards_sfd,&ack,sizeof(ulong));
	if (ret<0){
		printf("ERRO: %d", errno);
		error("Error receiving ack %s",strerror(errno));
	}
	else if (ret!=sizeof(ulong)){
		debug("Error receiving ack: expected %lu ret %d",sizeof(ulong),ret);
	}
	return (ret==sizeof(ulong)); // Should we return ack ?
}

/*
There is at least one byte available in the send buffer →send succeeds and returns the number of bytes accepted (possibly fewer than you asked for).
The send buffer is completely full at the time you call send.
→if the socket is blocking, send blocks
→if the socket is non-blocking, send fails with EWOULDBLOCK/EAGAIN
An error occurred (e.g. user pulled network cable, connection reset by peer) →send fails with another error
*/
int send_non_block_command(request_t *command)
{
    ulong ack; 
    int ret; 
	int tries=0;
	uint to_send,sended=0;
	uint to_recv,received=0;
	uint must_abort=0;
    request_header_t head;
    head.type = EAR_TYPE_COMMAND;
    head.size = sizeof(request_t);
    ret = write(eards_sfd, &head, sizeof(request_header_t));
    if (ret < sizeof(request_header_t))
    {
        warning("error sending request_header in non_block command");
        return 0;
    }
    debug("Sending command %u",command->req);
	to_send=sizeof(request_t);
	do
	{
		ret=send(eards_sfd, (char *)command+sended,to_send, MSG_DONTWAIT);
		if (ret>0){
			sended+=ret;
			to_send-=ret;
		}else if (ret<0){
			if ((errno==EWOULDBLOCK) || (errno==EAGAIN)) tries++;
			else{	
				must_abort=1;
      	error("Error sending command to eard %s,%d",strerror(errno),errno);
    	}
  	}else if (ret==0){
			warning("send returns 0 bytes");
			must_abort=1;
		}
	}while((tries<MAX_SOCKET_COMM_TRIES) && (to_send>0) && (must_abort==0));
	if (tries>=MAX_SOCKET_COMM_TRIES) debug("tries reached in recv %d",tries);
	/* If there are bytes left to send, we return a 0 */
	if (to_send){ 
		debug("return non blocking command with 0");
		return 0;
	}
	tries=0;
	to_recv=sizeof(ulong);
	do
	{
		ret=recv(eards_sfd, (char *)&ack+received,to_recv, MSG_DONTWAIT);
		if (ret>0){
			received+=ret;
			to_recv-=ret;
		}else if (ret<0){
			if ((errno==EWOULDBLOCK) || (errno==EAGAIN)) tries++;
			else{
				must_abort=1;
				error("Error receiving ack from eard %s,%d",strerror(errno),errno);
  		}
		}else if (ret==0){
			debug("recv returns 0 bytes");
			must_abort=1;
		}
	}while((tries<MAX_SOCKET_COMM_TRIES) && (to_recv>0) && (must_abort==0));

	if (tries>=MAX_SOCKET_COMM_TRIES){
		debug("Max tries reached in recv");
	}
	debug("send_non_block returns with %d",!to_recv);
    return (!to_recv); // Should we return ack ?
}

int send_non_block_data(int fd, size_t size, char *data, int type)
{
    ulong ack; 
    int ret; 
	int tries=0;
	uint to_send,sent=0;
	uint to_recv,received=0;
	uint must_abort=0;

    /* Prepare header */
    request_header_t head;
    head.type = type;
    head.size = size;

    /* Send header, blocking */
    ret = write(eards_sfd, &head, sizeof(request_header_t));

    if (ret < sizeof(request_header_t))
    {
        warning("error sending request_header in non_block command");
        return 0;
    }

    debug("Sending data of size %u and type %d", head.size, head.type);

	to_send=size;
	do
	{
        ret=send(eards_sfd, (char *)data+sent, to_send, MSG_DONTWAIT);
        if (ret>0){
            sent+=ret;
            to_send-=ret;
        }
        else if (ret<0)
        {
            if ((errno==EWOULDBLOCK) || (errno==EAGAIN)) tries++;
            else{	
                must_abort=1;
                error("Error sending command to eard %s,%d",strerror(errno),errno);
            }
        }else if (ret==0){
            warning("send returns 0 bytes");
            must_abort=1;
        }
	} while ((tries<MAX_SOCKET_COMM_TRIES) && (to_send>0) && (must_abort==0));
	if (tries>=MAX_SOCKET_COMM_TRIES) debug("tries reached in recv %d",tries);

	/* If there are bytes left to send, we return a 0 */
	if (to_send){ 
		debug("return non blocking command with 0");
		return EAR_ERROR;
	}
    /* We do not receive an ack since that is exclusive to commands, not general data */

    return EAR_SUCCESS;
}

int send_data(int fd, size_t size, char *data, int type)
{
    int ret;
    request_header_t head;
    head.size = size;
    head.type = type;

    debug("sending data of size %lu and type %d", size, type);
    debug("data sizes: %lu and %lu", sizeof(head.size), sizeof(head.type));
    ret = write(fd, &head, sizeof(request_header_t));
    debug("sent head, %d bytes", ret);
    ret = write(fd, data, size);
    debug("sent data, %d bytes", ret);

    return EAR_SUCCESS; 

}

char is_valid_type(int type)
{
    if (type >= MIN_TYPE_VALUE && type <= MAX_TYPE_VALUE) return 1;
    return 0;
}

request_header_t receive_data(int fd, void **data)
{
    int ret, total, pending;
    request_header_t head;
    char *read_data;
    head.type = 0;
    head.size = 0;

    ret = read(fd, &head, sizeof(request_header_t));
    debug("values read: type %d size %u", head.type, head.size);
    if (ret < 0) {
        error("Error recieving response data header (%s) ", strerror(errno));
        head.type = EAR_ERROR;
        head.size = 0;
        return head;
    }

    if (head.size < 1 || !is_valid_type(head.type)) {
        if (!((head.size == 0) && (head.type == 0))) debug("Error recieving response data. Invalid data size (%d) or type (%d).", head.size, head.type);
        if (head.type != EAR_TYPE_APP_STATUS)
        {
            head.type = EAR_ERROR;
            head.size = 0;
            return head;
        }
    }
    //write ack should go here if we implement it
    read_data = calloc(head.size, sizeof(char));
    total = 0;
    pending = head.size;

    ret = read(fd, read_data+total, pending);
	if (ret<0){
		error("Error by receive data (%s)",strerror(errno));
        free(read_data);
        head.type = EAR_ERROR;
        head.size = 0;
		return head;
	}
	total+=ret;
	pending-=ret;
	while ((ret>0) && (pending>0)){
    	ret = read(fd, read_data+total, pending);
    	//ret = recv(eards_sfd, (char *)return_status+total, pending, MSG_DONTWAIT);
		if (ret<0){
		    error("Error by receive data (%s)",strerror(errno));
            free(read_data);
            head.type = EAR_ERROR;
            head.size = 0;
			return head;
		}
		total+=ret;
		pending-=ret;
	}
    *data = read_data;
    debug("returning from receive_data with type %d and size %u", head.type, head.size);
	return head;

}

int set_socket_block(int sfd, char blocking)
{
    if (sfd < 0) return EAR_ERROR;

    int flags = fcntl(sfd, F_GETFL, 0);
    //if flags is -1 fcntl returned an error
    if (flags == -1) return EAR_ERROR;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(sfd, F_SETFL, flags) != 0) return EAR_ERROR;
    return EAR_ERROR;
}

// based on getaddrinfo  man page
int eards_remote_connect(char *nodename,uint port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char port_number[50]; 	// that size needs to be validated
    int sfd, s;
    fd_set set;

		if (eards_remote_connected){ 
			debug("Connection already done!");
			return eards_sfd;
		}
   	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

		sprintf(port_number,"%d",port);
   	s = getaddrinfo(nodename, port_number, &hints, &result);
    if (s != 0) {
			debug("getaddrinfo fail for %s and %s",nodename,port_number);
			return EAR_ERROR;
    }

    struct timeval timeout;
    memset(&timeout, 0, sizeof(struct timeval));
    timeout.tv_sec = 0;
    timeout.tv_usec = 5000;
    socklen_t  optlen;
		int valopt, sysret;

   	for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1) // if we can not create the socket we continue
            continue;

        set_socket_block(sfd, 0);
        
        int res = connect(sfd, rp->ai_addr, rp->ai_addrlen);
        if (res < 0 && errno != EINPROGRESS)
        {
            close(sfd);
            continue;
        }
        if (res == 0) //sucessful connection
        {
            set_socket_block(sfd, 1);
            break;
        }
        else
        {
            FD_ZERO(&set);
            FD_SET(sfd, &set);
            if (select(sfd+1, &set, &set, NULL, &timeout) > 0) 
            {
                optlen = sizeof(int);
                sysret = getsockopt(sfd, SOL_SOCKET, SO_ERROR, (void *)(&valopt), &optlen);
                if (sysret)
                {
                    debug("Error geting sockopt");
                    close(sfd);
                    continue;
                }
                else if (optlen != sizeof(int))
                {
                    debug("Error with getsockopt");
                    close(sfd);
                    continue;
                }
                else if (valopt)
                {
                    debug("Error opening connection %s",nodename);
                    close(sfd);
                    continue;
                }
                else debug("Connected");
            }
            else
            {
                debug("Timeout connecting to %s node", nodename);
                close(sfd);
                continue;
            }
            set_socket_block(sfd, 1);
            break;
        }
    }

   	if (rp == NULL) {               /* No address succeeded */
		debug("Failing in connecting to remote eards");
		return EAR_ERROR;
    }

    char conection_ok = 0;

    timeout.tv_sec = 1;

    setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (void *)(&timeout), sizeof(timeout));
    
    if (read(sfd, &conection_ok, sizeof(char)) > 0)
    {
        debug("Handshake with server completed.");
    }
    else
    {
        debug("Couldn't complete handshake with server, closing conection.");
        close(sfd);
        return EAR_ERROR;
    }

    memset(&timeout, 0, sizeof(struct timeval));
    setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (void *)(&timeout), sizeof(timeout));

   	freeaddrinfo(result);           /* No longer needed */
	eards_remote_connected=1;
	eards_sfd=sfd;
	return sfd;

}

int eards_remote_disconnect()
{
	eards_remote_connected=0;
	close(eards_sfd);
	return EAR_SUCCESS;
}

request_header_t correct_data_prop(int target_idx, int total_ips, int *ips, request_t *command, uint port, void **data)
{
    char *temp_data, *final_data = NULL;
    int rc, i, current_dist, off_ip, current_idx, final_size = 0, default_type = EAR_ERROR;
    struct sockaddr_in temp;
    request_header_t head;
    char next_ip[64];

    current_dist = target_idx - target_idx%NUM_PROPS;
    off_ip = target_idx%NUM_PROPS;

    for ( i = 1; i <= NUM_PROPS; i++)
    {
        current_idx = current_dist*NUM_PROPS + i*NUM_PROPS + off_ip;
        //check that the next ip exists within the range
        if (current_idx >= total_ips) break;

        //prepare next node data
        temp.sin_addr.s_addr = ips[current_idx];
        strcpy(next_ip, inet_ntoa(temp.sin_addr));
        //prepare next node distance
        command->node_dist = current_idx - off_ip;

        //connect and send data
        rc = eards_remote_connect(next_ip, port);
        if (rc < 0)
        {
            debug("propagate_req:Error connecting to node: %s", next_ip);
            head = correct_data_prop(current_idx, total_ips, ips, command, port, (void **)&temp_data);
        }
        else
        {
            send_command(command);
            head = receive_data(rc, (void **)&temp_data);
            if ((head.size) < 1 || head.type == EAR_ERROR)
            {
                debug("propagate_req: Error propagating command to node %s", next_ip);
                eards_remote_disconnect();
                head = correct_data_prop(current_idx, total_ips, ips, command, port, (void **)&temp_data);
            }
            else eards_remote_disconnect();
        }

        //TODO: data processing, this is a workaround for status_t
        if (head.size > 0 && head.type != EAR_ERROR)
        {
            default_type = head.type;
            final_data = realloc(final_data, final_size + head.size);
            memcpy(&final_data[final_size], temp_data, head.size);
            final_size += head.size;
            free(temp_data);
        }
    }

    head.size = final_size;
    head.type = default_type;
    *data = final_data;
    
    if (default_type == EAR_ERROR && final_size > 0)
    {
        free(final_data);
        head.size = 0;
    }
    else if (final_size == 0 && default_type != EAR_ERROR) head.type = EAR_ERROR;


    return head;

}

void correct_error(int target_idx, int total_ips, int *ips, request_t *command, uint port)
{
    if (command->node_dist > total_ips) return;
    struct sockaddr_in temp;

    int i, rc, current_dist, off_ip, current_idx;
    char next_ip[50]; 

    current_dist = target_idx - target_idx%NUM_PROPS;
    off_ip = target_idx%NUM_PROPS;

    for (i = 1; i <= NUM_PROPS; i++)
    {
        current_idx = current_dist*NUM_PROPS + i*NUM_PROPS + off_ip;
        //check that the next ip exists within the range
        if (current_idx >= total_ips) break;

        //prepare next node data
        temp.sin_addr.s_addr = ips[current_idx];
        strcpy(next_ip, inet_ntoa(temp.sin_addr));

        //prepare next node distance
        command->node_dist = current_idx - off_ip;

        //connect and send data
        rc = eards_remote_connect(next_ip, port);
        if (rc < 0)
        {
            debug("propagate_req:Error connecting to node: %s", next_ip);
            correct_error(current_idx, total_ips, ips, command, port);
        }
        else
        {
            if (!send_command(command)) 
            {
                debug("propagate_req: Error propagating command to node %s", next_ip);
                eards_remote_disconnect();
                correct_error(current_idx, total_ips, ips, command, port);
            }
            else eards_remote_disconnect();
        }
    }
}

#if NODE_PROP
void correct_error_nodes(request_t *command, int self_ip, uint port)
{
     
    request_t tmp_command;
    request_header_t head;
    struct sockaddr_in temp;
    int i, rc;
    char next_ip[50]; 

    if (command->num_nodes < 1) return;
    else
    {   //if there is only one node and it is the current one nothing needs to be done
        if (command->num_nodes == 1 && command->nodes[0] == self_ip) return;
        //if not, we remove the current node from the list
        for (i = 0; i < command->num_nodes; i++)
        {
            if (command->nodes[i] == self_ip)
            {
                command->num_nodes --;
                command->nodes[i] = command->nodes[command->num_nodes];
                command->nodes = realloc(command->nodes, sizeof(int) * command->num_nodes);
            }
        }
    }

    memcpy(&tmp_command, command, sizeof(request_t));
    int base_distance = command->num_nodes/NUM_PROPS;
    if (base_distance < 1) base_distance = 1; //if there is only 1 or 2 nodes

    internal_send_command_nodes(command, port, base_distance, NUM_PROPS);

    // we set num_nodes to 0 to prevent unknown errors, but memory is freed in request_propagation
    command->num_nodes = 0;
    
}

request_header_t correct_data_prop_nodes(request_t *command, int self_ip, uint port, void **data)
{
    request_t tmp_command;
    request_header_t head;
    char *temp_data, *final_data = NULL;
    int rc, i, final_size = 0, default_type = EAR_ERROR;
    struct sockaddr_in temp;
    char next_ip[64];

    head.size = 0;
    head.type = 0;

    if (command->num_nodes < 1) return head;
    else
    {   //if there is only one node and it is the current one nothing needs to be done
        if (command->num_nodes == 1 && command->nodes[0] == self_ip) return head;
        //if not, we remove the current node from the list
        for (i = 0; i < command->num_nodes; i++)
        {
            if (command->nodes[i] == self_ip)
            {
                command->num_nodes --;
                command->nodes[i] = command->nodes[command->num_nodes];
                command->nodes = realloc(command->nodes, sizeof(int) * command->num_nodes);
            }
        }
    }

    memcpy(&tmp_command, command, sizeof(request_t));
    int base_distance = command->num_nodes/NUM_PROPS;
    if (base_distance < 1) base_distance = 1; //if there is only 1 or 2 nodes
    for (i = 0; i < NUM_PROPS; i++)
    {
        if (base_distance * i >= command->num_nodes) break;
        //prepare next node data
        temp.sin_addr.s_addr = command->nodes[base_distance * i];
        strcpy(next_ip, inet_ntoa(temp.sin_addr));

        // node distance is not needed
        // we calculate how many ips will be sent to this node by getting the max possible ip index and subtracting the starting one
        int max_ips = (base_distance * (1 + i) < command->num_nodes) ? base_distance * (1 + i) : command->num_nodes;
        max_ips -= base_distance * i;

        tmp_command.nodes = calloc(max_ips, sizeof(int));
        tmp_command.num_nodes = max_ips;

        memcpy(tmp_command.nodes, &command->nodes[base_distance*i], max_ips*sizeof(int));

        //connect and send data
        rc = eards_remote_connect(next_ip, port);
        if (rc < 0)
        {
            error("propagate_req:Error connecting to node: %s\n", next_ip);
            head = correct_data_prop_nodes(&tmp_command, self_ip, port, (void **)&temp_data);
        }
        else
        {
            send_command(&tmp_command);
            head = receive_data(rc, (void **)&temp_data);
            if ((head.size) < 1 || head.type == EAR_ERROR)
            {
                error("propagate_req: Error propagating command to node %s\n", next_ip);
                eards_remote_disconnect();
                head = correct_data_prop_nodes(&tmp_command, self_ip, port, (void **)&temp_data);
            }
            else eards_remote_disconnect();
        }

        if (head.size > 0 && head.type != EAR_ERROR)
        {
            default_type = head.type;
            final_data = realloc(final_data, final_size + head.size);
            memcpy(&final_data[final_size], temp_data, head.size);
            final_size += head.size;
            free(temp_data);
        }

        free(tmp_command.nodes);
    }

    head.size = final_size;
    head.type = default_type;
    *data = final_data;
    
    if (default_type == EAR_ERROR && final_size > 0)
    {
        free(final_data);
        head.size = 0;
    }
    else if (final_size == 0 && default_type != EAR_ERROR) head.type = EAR_ERROR;

    // we set num_nodes to 0 to prevent unknown errors, but memory is freed in request_propagation
    command->num_nodes = 0;
    return head;

}

#endif

#if NODE_PROP

#define MAX_PROP_DEPTH 3
/* Calculates the "ideal" amount of nodes a node will comunicate to. See send_command_nodes for
 * how the comunication works. We calculate it taking into account the total number of nodes 
 * the message will be send to, the number of propagations a node will do and the maximum propagation
 * depth we want to achieve (how many propagations can it take to get to leaf nodes). */
int get_max_prop_group(int num_props, int max_depth, int num_nodes)
{

    int max_nodes, final_nodes;
    /* If we the standard number of propagations is higher than the number of nodes, we
     * set the distance to 1 so every node is contacted via the initial communication */
    if (num_nodes < num_props) return 1;
    max_nodes = powl(num_props, max_depth);

    /* Once we have the max number of nodes one initial communication can propagate to, the idea is
     * to optimise the number of initial communications. For now we optimise the lower bound, to guarantee
     * that the initial communication will, at the very least communicate with num_props nodes. */
    while (max_nodes > num_nodes / num_props)
    {
        max_nodes /= num_props;
    }

    /* The following is an idea to guarantee that each initial communication propagates to at least 
     * a certain % of nodes. The example is for 10%. Not implemented yet, certain cases would have to
     * be explored: the % makes a max_nodes > num_nodes / num_props; the % makes max_nodes = num_props. */
    /*while (max_nodes < num_nodes * 0.10)
    {
        max_nodes *= num_props;
    }*/

    if (max_nodes > num_props) return max_nodes;

    return num_props;

}

void internal_send_command_nodes(request_t *command, int port, int base_distance, int num_sends)
{
    int i, rc;
    struct sockaddr_in temp;
    char next_ip[256];
    request_t tmp_command;

    memcpy(&tmp_command, command, sizeof(request_t));
    for (i = 0; i < num_sends; i++)
    {

        // if no more nodes to propagate to
        debug("base distance %d i %d num_nodes %d", base_distance, i, command->num_nodes);
        if (base_distance * i > command->num_nodes) break;

        temp.sin_addr.s_addr = command->nodes[i*base_distance];
        strcpy(next_ip, inet_ntoa(temp.sin_addr));

        // node distance is not needed
        int max_ips = (base_distance * (1 + i) < command->num_nodes) ? base_distance * (1 + i) : command->num_nodes;
        max_ips -= base_distance * i;

        tmp_command.nodes = calloc(max_ips, sizeof(int));
        tmp_command.num_nodes = max_ips;

        memcpy(tmp_command.nodes, &command->nodes[base_distance*i], max_ips*sizeof(int));

        rc=eards_remote_connect(next_ip, port);
        if (rc<0){
            debug("Error connecting with node %s, trying to correct it", next_ip);
            correct_error_nodes(&tmp_command, command->nodes[base_distance*i], port);
        }
        else{
            debug("Node %s with %d num_nodes contacted!", next_ip, tmp_command.num_nodes);
            if (!send_command(&tmp_command)) {
                debug("Error sending command to node %s, trying to correct it", next_ip);
                eards_remote_disconnect();
                correct_error_nodes(&tmp_command, command->nodes[i*base_distance], port);
            }
            else eards_remote_disconnect();
        }
        free(tmp_command.nodes);
        tmp_command.num_nodes = 0;
    }
}

void send_command_nodes(request_t command, cluster_conf_t *my_cluster_conf)
{
    int i, j, rc, base_distance, num_sends;
    struct sockaddr_in temp;
    char next_ip[256];

    time_t ctime = time(NULL);
    command.time_code = ctime;

    /* We get which nodes this function will comunicate with by sending a message
     * every _base_distance_ nodes, and sending to that node the list of nodes between 
     * themselves and the next node. IE: if we have nodes A B C D E F G and we get a 
     * base_distance of 3, we will send the message to A, D and G. To A we will send A B C,
     * D will receive D E F, and G will only receive themselves. */
    base_distance = get_max_prop_group(NUM_PROPS, MAX_PROP_DEPTH, command.num_nodes);
    num_sends = command.num_nodes / base_distance; //number of nodes this function will comunicate with
    debug("sending command with %d nodes, %d base_distance and %d num_sends", command.num_nodes, base_distance, num_sends);

    /* Inner functionality for initial communication has been moved to its own function to avoid code replication */
    internal_send_command_nodes(&command, my_cluster_conf->eard.port, base_distance, num_sends);
}
#endif

void send_command_all(request_t command, cluster_conf_t *my_cluster_conf)
{
    int i, j,  rc, total_ranges;
    int **ips, *ip_counts;
    struct sockaddr_in temp;
    char next_ip[256];
    time_t ctime = time(NULL);
    command.time_code = ctime;
    total_ranges = get_ip_ranges(my_cluster_conf, &ip_counts, &ips);
    for (i = 0; i < total_ranges; i++)
    {
        for (j = 0; j < ip_counts[i] && j < NUM_PROPS; j++)
        {
            command.node_dist = 0;
            temp.sin_addr.s_addr = ips[i][j];
            strcpy(next_ip, inet_ntoa(temp.sin_addr));
            
            rc=eards_remote_connect(next_ip, my_cluster_conf->eard.port);
            if (rc<0){
                debug("Error connecting with node %s, trying to correct it", next_ip);
                correct_error(j, ip_counts[i], ips[i], &command, my_cluster_conf->eard.port);
            }
            else{
                debug("Node %s with distance %d contacted!", next_ip, command.node_dist);
                if (!send_command(&command)) {
                    debug("Error sending command to node %s, trying to correct it", next_ip);
                    eards_remote_disconnect();
                    correct_error(j, ip_counts[i], ips[i], &command, my_cluster_conf->eard.port);
                }
                else eards_remote_disconnect();
            }
            
        }
    }
    for (i = 0; i < total_ranges; i++)
        free(ips[i]);
    free(ips);
    free(ip_counts);

}

request_header_t data_all_nodes(request_t *command, cluster_conf_t *my_cluster_conf, void **data)
{
    int i, j, rc, total_ranges, final_size = 0, default_type = EAR_ERROR;
    int **ips, *ip_counts;
    struct sockaddr_in temp;
    request_header_t head;
    char *temp_data, *all_data = NULL;
    char next_ip[64];
    
    total_ranges = get_ip_ranges(my_cluster_conf, &ip_counts, &ips);
    for (i = 0; i < total_ranges; i++)
    {
        for (j = 0; j < ip_counts[i] && j < NUM_PROPS; j++)
        {
            temp.sin_addr.s_addr = ips[i][j];
            strcpy(next_ip, inet_ntoa(temp.sin_addr));
            
            rc=eards_remote_connect(next_ip, my_cluster_conf->eard.port);
            if (rc<0){
                debug("Error connecting with node %s, trying to correct it", next_ip);
                head = correct_data_prop(j, ip_counts[i], ips[i], command, my_cluster_conf->eard.port, (void **)&temp_data);
            }
            else{
                debug("Node %s with distance %d contacted!", next_ip, command->node_dist);
                send_command(command);
                head = receive_data(rc, (void **)&temp_data);
                if (head.size < 1 || head.type == EAR_ERROR) {
                    debug("Error sending command to node %s, trying to correct it", next_ip);
                    eards_remote_disconnect();
                    head = correct_data_prop(j, ip_counts[i], ips[i], command, my_cluster_conf->eard.port, (void **)&temp_data);
                }
                else eards_remote_disconnect();
            }
        
            if (head.size > 0 && head.type != EAR_ERROR)
            {
                head = process_data(head, (char **)&temp_data, (char **)&all_data, final_size);
                free(temp_data);
                final_size = head.size;
                default_type = head.type;
            }
            
        }
    }
    head.size = final_size;
    head.type = default_type;
    *data = all_data;

    if (default_type == EAR_ERROR && final_size > 0)
    {
        free(all_data);
        head.size = 0;
    }
    else if (final_size < 1 && default_type != EAR_ERROR) head.type = EAR_ERROR;

    // Freeing allocated memory
    if (total_ranges > 0) {
        for (i = 0; i < total_ranges; i++)
            free(ips[i]);
        free(ip_counts);
        free(ips);
    }

    return head;
}

powercap_status_t *mem_alloc_powercap_status(char *final_data)
{
    int final_size;
    powercap_status_t *status = calloc(1, sizeof(powercap_status_t));

    final_size = sizeof(powercap_status_t);
    memcpy(status, final_data, final_size);

    status->greedy_nodes = calloc(status->num_greedy, sizeof(int));
    status->greedy_req = calloc(status->num_greedy, sizeof(uint));
    status->extra_power = calloc(status->num_greedy, sizeof(uint));

    memcpy(status->greedy_nodes, &final_data[final_size], sizeof(int)*status->num_greedy);
    final_size += status->num_greedy*sizeof(int);

    memcpy(status->greedy_req, &final_data[final_size], sizeof(uint)*status->num_greedy);
    final_size += status->num_greedy*sizeof(uint);

    memcpy(status->extra_power, &final_data[final_size], sizeof(uint)*status->num_greedy);
    final_size += status->num_greedy*sizeof(uint);

    return status;
}

char *mem_alloc_char_powercap_status(powercap_status_t *status)
{
    int size = sizeof(powercap_status_t) + ((status->num_greedy) * (sizeof(int) + sizeof(uint)*2));
    char *data = calloc(size, sizeof(char));

    memcpy(data, status, sizeof(powercap_status_t));
    size = sizeof(powercap_status_t);

    memcpy(&data[size], status->greedy_nodes, sizeof(int) * status->num_greedy);
    size += sizeof(int) * status->num_greedy;
    
    memcpy(&data[size], status->greedy_req, sizeof(uint) * status->num_greedy);
    size += sizeof(uint) * status->num_greedy;

    memcpy(&data[size], status->extra_power, sizeof(uint) * status->num_greedy);
    size += sizeof(uint) * status->num_greedy;


    return data;
}

powercap_status_t *memmap_powercap_status(char *final_data, int *size)
{
    int final_size;
    powercap_status_t *status = (powercap_status_t *) final_data;

    final_size = sizeof(powercap_status_t);
    status->greedy_nodes = (int *)&final_data[final_size];
    final_size += status->num_greedy*sizeof(int);
               
    status->greedy_req = (uint *)&final_data[final_size];
    final_size += status->num_greedy*sizeof(uint);

    status->extra_power = (uint *)&final_data[final_size];
    final_size += status->num_greedy*sizeof(uint);

    *size = final_size;
    return status;

}

request_header_t process_data(request_header_t data_head, char **temp_data_ptr, char **final_data_ptr, int final_size)
{
    char *temp_data = *temp_data_ptr;
    char *final_data = *final_data_ptr;
    request_header_t head;
    head.type = data_head.type;
    switch(data_head.type)
    {
        case EAR_TYPE_RELEASED:
						head.size = data_head.size;
            if (final_data != NULL)
            {
               pc_release_data_t *released = (pc_release_data_t *)final_data; 
               pc_release_data_t *new_released = (pc_release_data_t *)temp_data; 
               released->released += new_released->released;
            }
            else
            {
                final_data = realloc(final_data, final_size + data_head.size);
                memcpy(&final_data[final_size], temp_data, data_head.size);
                //cannot directly assign final_data = temp_data because the caller function (data_all_nodes) frees temp_data after passing through here
            }
            break;
        case EAR_TYPE_POWER_STATUS:
            if (final_data != NULL)
            {
                int total_size = 0;
                powercap_status_t *original_status = memmap_powercap_status(final_data, &final_size);
                total_size += final_size - sizeof(powercap_status_t);
                powercap_status_t *new_status = memmap_powercap_status(temp_data, &final_size);
                total_size += final_size;

                char *final_status = calloc(total_size, sizeof(char));
                powercap_status_t *status = (powercap_status_t *)final_status;

								status->total_nodes = original_status->total_nodes + new_status->total_nodes;
                status->idle_nodes = original_status->idle_nodes + new_status->idle_nodes;
                status->released= original_status->released + new_status->released;
                status->num_greedy = original_status->num_greedy + new_status->num_greedy;
                status->requested = original_status->requested + new_status->requested;
                status->current_power = original_status->current_power + new_status->current_power;
                status->total_powercap = original_status->total_powercap + new_status->total_powercap;

                final_size = sizeof(powercap_status_t);
                memcpy(&final_status[final_size], original_status->greedy_nodes, sizeof(int)*original_status->num_greedy);
                final_size += sizeof(int)*original_status->num_greedy;
                memcpy(&final_status[final_size], new_status->greedy_nodes, sizeof(int)*new_status->num_greedy);
                final_size += sizeof(int)*new_status->num_greedy;

                memcpy(&final_status[final_size], original_status->greedy_req, sizeof(uint)*original_status->num_greedy);
                final_size += sizeof(uint)*original_status->num_greedy;
                memcpy(&final_status[final_size], new_status->greedy_req, sizeof(uint)*new_status->num_greedy);
                final_size += sizeof(uint)*new_status->num_greedy;

                memcpy(&final_status[final_size], original_status->extra_power, sizeof(uint)*original_status->num_greedy);
                final_size += sizeof(uint)*original_status->num_greedy;
                memcpy(&final_status[final_size], new_status->extra_power, sizeof(uint)*new_status->num_greedy);
                final_size += sizeof(uint)*new_status->num_greedy;

                final_data = realloc(final_data, final_size);
                memcpy(final_data, final_status, final_size);
                
                status = memmap_powercap_status(final_data, &final_size);
                head.size = final_size;
                free(final_status);

            }
            else
            {
                final_data = realloc(final_data, final_size + data_head.size);
                memcpy(&final_data[final_size], temp_data, data_head.size);
                head.size = data_head.size;
                final_data = (char *)memmap_powercap_status(final_data, &final_size);
                //setting powercap_status pointers to its right value
                //check if final_size == head.size??

            }
        break;
        default:
            final_data = realloc(final_data, final_size + data_head.size);
            memcpy(&final_data[final_size], temp_data, data_head.size);
            head.size = final_size + data_head.size;
        break;
    }

    *final_data_ptr = final_data;

    return head;
}

