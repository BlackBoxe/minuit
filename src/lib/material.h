/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "minuit.h"

#ifndef __MATERIAL_H
#define __MATERIAL_H

typedef struct Material t_material;

struct Material
{
	int id;
	int id_chunk;
	short users;
	char name[_NAME_];

	int idcol[3];
	int id_node;

	float color[4];
	float specular[4];
	float shininess[4];
	float s_color[4];
};

t_material*	material_new(const char *name);
t_material*	material_rebind(t_scene *sc,void *ptr);

#endif
