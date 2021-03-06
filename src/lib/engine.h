/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */


#ifndef __ENGINE_H
#define __ENGINE_H

#include "util.h"
#include "process.h"

#define ENGINE_WITH_GLOBAL_LIMIT 1
#define ENGINE_GLOBAL_FREQ 1.0f
#define ENGINE_GLOBAL_LIMIT 0.01f

typedef struct Engine t_engine;
typedef struct Engine_Class t_engine_cls;

// ENGINE

struct Engine_Class
{
	char cls_type[_NAME_];
};

struct Engine
{
	int id;
	int id_chunk;
	short users;
	char name[_NAME_];
	t_engine_cls *cls;

	int tot_processes;
	
	int with_global_limit;
	float global_limit;
	float global_freq;

	t_lst *processes;
};

// ENGINE.C

t_engine *engine_new(const char *name);
void engine_quit(t_engine *engine);
void engine_init(void);
void engine_start(void);
void engine_stop(void);
void engine_start_job(t_engine *engine,int id);
void engine_stop_job(t_engine *engine,int id);

t_process *engine_process_get(t_engine *engine,char *name);

#endif
