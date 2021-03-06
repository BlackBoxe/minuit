/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __SERVER_H_
#define __SERVER_H_

#include "util.h"

typedef struct MNserver t_server;

struct MNserver
{
	t_socket *socket;
};

void server_connect(t_server *server,int port);
t_server *server_new(void);

//void slave(void);

#endif
