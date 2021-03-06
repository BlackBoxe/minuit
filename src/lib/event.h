/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __EVENT_H
#define __EVENT_H


typedef struct Event t_event;
typedef enum EventState t_event_state;

enum EventState
{
	event_idle,
	event_linking,
	event_unlinking,
};

typedef struct State_Event_UI t_state_event_ui;
struct State_Event_UI
{
	// mouse
	int is_mouse_left_pressed:1;
	int is_mouse_left_released:1;
	int is_mouse_over_brick:1;
	int start_x:1;
	int start_y:1;
	int end_x:1;
	int end_y:1;

	int pan:1;

	// background

	int is_background_button_left_pressed:1;
	int is_background_button_left_released:1;
	
	int is_menu_show:1;
	int is_menu_mouse_show:1;
	int is_menu_pressed:1;
	int is_menu_released:1;
	
	int is_selection:1;

	// brick
	int is_brick_transformed:1;
	int is_brick_pressed:1;
	int is_brick_moving:1;
	int is_drawing:1;
	int is_linking:1;
	int is_linking_released:1;
	int is_typing:1;
};

struct Event
{
	t_event_state state;

	t_state_event_ui ui;

	int update_connections;

	// color
	int color[4];
	float color_front[3];
	float color_back[3];
	int color_buffer;

	// mouse
	int is_mouse_left_pressed;
	int is_mouse_left_released;
	int is_mouse_over_brick;
	int start_x;
	int start_y;
	int end_x;
	int end_y;

	// background

	int is_background_button_left_pressed;
	int is_background_button_left_released;

	int is_selection;

	// brick
	int is_brick_transformed;
	int is_brick_pressed;
	int is_brick_moving;
	int is_drawing;
	int is_linking;
	int is_linking_released;
	int is_typing;

	char *action;

	// draw 
	int with_texture;
	int with_polygon_offset;
	int with_normal;
	int with_light;
	int with_clear;
	int with_depth;
	int with_point;
	int with_point_id;
	int with_face;
	int with_highlight;
	int with_blend;

	// sketch
	int sketch_with_scale;

	int rec;
	int recording;
	int rec_save;
	int frame_rec;
	int debug_select;
	int debug_mouse;
	int debug_process;
	int show_term;
	int free_scene;
	int last_frame; //last recorded camera

	char buffer_char[20];
	int buffer_char_counter;

	void (* callback)(void);
	int standby_int;
	char standby_char;
	char *standby_string;
	int video;
};

t_event *event_new(void);

#endif
