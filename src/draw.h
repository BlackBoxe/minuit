/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __DRAW_H
#define __DRAW_H

#include "ui.h"


#define PI 3.14159655f
#define SEL_LINE_WIDTH 200

#define DRAW_SELECTION_PASS 1
#define DRAW_MODE 1

#define DRAW_COLOR color_black
#define DRAW_COLOR_BACKGROUND_RED 255
#define DRAW_COLOR_BACKGROUND_GREEN 255
#define DRAW_COLOR_BACKGROUND_BLUE 255
#define DRAW_COLOR_BACKGROUND_ALPHA 0

#define DRAW_SHOW_AXIS_WORLD 0
#define DRAW_WITH_SCALE 0
#define DRAW_SCALE 1
#define DRAW_WITH_CLEAR 1
#define DRAW_WITH_SELECTION 1
#define DRAW_WITH_POINT 0
#define DRAW_WITH_EDGE 0
#define DRAW_WITH_POINT_ID 0;
#define DRAW_WITH_FACE 1;
#define DRAW_WITH_DEPTH 1;
#define DRAW_WITH_POLYGON_OFFSET 1;
#define DRAW_WITH_NORMAL 1
#define DRAW_WITH_TEXTURE 1
#define DRAW_WITH_HIGHLIGHT 1
#define DRAW_WITH_LIGHT 1
#define DRAW_WITH_ALPHA 1
#define DRAW_WITH_MATERIAL 1
#define DRAW_WITH_DRAW 1
#define DRAW_WITH_UI 1
#define DRAW_WITH_BLEND 1

typedef struct Draw t_draw;
typedef enum Draw_Mode t_draw_mode;
typedef enum Color_Mode t_color_mode;

enum Draw_Mode
{
	mode_selection,
	mode_draw
};

enum Color_Mode 
{
	color_white,
	color_black
};

struct Draw 
{
	int background_color[4];
	float front_color[4];

	t_draw_mode mode;
	t_color_mode color;

	int mode_direct;

	int with_selection_pass; //with_color
	int with_draw_pass;
	int with_ui;

	int with_clear;
	int with_normal;
	int with_texture;
	int with_material;
	int with_point;
	int with_point_id;
	int with_face;
	int with_blend;
	int with_restrict_matrix;

	int with_highlight;
	int with_light;
	int with_depth;
	int with_polygon_offset;
	int with_alpha;
	int with_grid;
	int img_with_alpha;
	int divx;
	int divy;

	int show_axis_world;
};

void draw_clear(float r,float g,float b,float a);
t_draw *draw_new(void);
void draw_scene(t_draw *draw, t_scene *scene);
void draw_switch_axis_world(t_draw *draw);
void draw_mesh_direct(t_draw *draw,t_scene *scene,t_mesh *mesh);
void draw_mesh(t_draw *draw, t_scene *scene, t_mesh *mesh);
void type_font_3d(char* string,float pos[3]);

#endif
