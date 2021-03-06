/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "context.h"

t_event *event_new(void)
{
	t_event *event=(t_event*)malloc(sizeof(t_event));

	bzero(&event->ui,sizeof(t_state_event_ui));

	event->update_connections=0;

	event->state=event_idle;
	event->is_background_button_left_pressed=0;
	event->is_background_button_left_released=0;

	event->ui.is_menu_pressed=0;
	event->ui.is_menu_released=0;
	event->ui.is_menu_show=0;
	event->ui.is_menu_mouse_show=0;
	event->ui.pan = 0;

	event->is_selection=0;

	event->is_brick_transformed=0;
	event->is_brick_pressed=0;
	event->is_brick_moving=0;
	event->is_mouse_over_brick=0;
	event->is_typing=0;
	event->action=NULL;
	event->with_texture=1;
	event->with_polygon_offset=EVENT_WITH_POLYGON_OFFSET;
	event->with_clear=1;
	event->with_normal=1;
	event->with_light=1;
	event->with_depth=1;
	event->with_face=1;
	event->with_point=0;
	event->with_point_id=0;
	event->with_highlight=WITH_HIGHLIGHT;
	event->sketch_with_scale=SKETCH_WITH_SCALE;
	event->with_blend=1;

	event->is_drawing=0;

	event->color[0] = 0;
	event->color[1] = 0;
	event->color[2] = 0;

	event->rec=0;
	event->rec_save=0;
	event->recording=0;

	event->debug_select=CTX_DEBUG_SELECT;
	event->debug_mouse=CTX_DEBUG_MOUSE;
	event->debug_process=DEBUG_PROCESS;
	event->show_term=CTX_SHOW_TERM;
	event->free_scene=FREE_SCENE;

	vset(event->color_front,1,1,1);
	vset(event->color_back,0,0,0);
	event->color_buffer=0;

	event->last_frame=0;

	bzero(&event->buffer_char[0],20);
	event->buffer_char_counter=0;
	
	event->callback=NULL;
	event->standby_int=0;
	event->standby_char='\0';
	event->standby_string=NULL;
	event->video = 0;

	return event;
}
