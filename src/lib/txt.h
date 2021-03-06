/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "util.h"

#ifndef __TXT_H
#define __TXT_H

typedef struct Txt t_txt;

struct Txt
{
	int id;
	int id_chunk;
	short users;
	char name[_NAME_LONG_];

	int icol[3];
	int id_node;

	float width;

	float grid_step;
	float grid_size_x;
	float grid_size_y;
	float grid_spacing;
	float grid_width;
	float grid_height;

	float letter_scale_x; //scale x
	float letter_scale_y; //scale y

	float letter_width;
	float letter_height;

	void (* draw)(t_txt *txt);
	void (* data_change)(t_txt *txt,const char *data);
	float (* get_width)(t_txt *txt);

	int use_bitmap_font;
};

// TXT.C

void txt_alphabet_make(void);
void txt_layout_init(void);

float txt_get_length(t_txt *txt);

void txt_draw_direct_2d(char *string,float pos[3],float *color,float scale);
void txt_draw_3d(char *string,float pos[3]);
void txt_free(t_txt *txt);

void txt_init(t_txt *txt,const char *name);
t_txt *txt_new(const char *data);

#endif
