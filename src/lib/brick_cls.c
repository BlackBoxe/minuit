/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "op.h"

// DECLARATIONS

void cls_brick_init(t_brick *brick)
{
	t_context *C=ctx_get();
	brick_init(C->scene,brick);
}

void brick_make_trigger(t_brick *brick);
void brick_make_slider(t_brick *brick);
void brick_make_menu(t_brick *brick);
void brick_make_selector(t_brick *brick);
void brick_make_switch(t_brick *brick);

// TRIGGER

t_brick_class brick_trigger = {
	.cls_type="brick",
	.type=bt_trigger,
	.make=brick_make_trigger,
	.draw=brick_draw,
	.update=cls_brick_update,
	.trigger=cls_brick_trigger_generic,
	.init=cls_brick_init,
	.connect=cls_brick_connect,
	.disconnect=cls_brick_disconnect,
	};

// SLIDER

t_brick_class brick_slider = {
	.cls_type="brick",
	.type=bt_slider,
	.make=brick_make_slider,
	.draw=brick_draw,
	.update=cls_brick_update,
	.trigger=cls_brick_trigger_number,
	.init=cls_brick_init,
	.connect=cls_brick_connect,
	.disconnect=cls_brick_disconnect,
	};

// MENU

t_brick_class brick_menu = {
	.cls_type="brick",
	.type=bt_menu,
	.make=brick_make_menu,
	.draw=brick_draw,
	.update=cls_brick_update,
	.trigger=cls_brick_trigger_menu,
	.init=cls_brick_init,
	.connect=cls_brick_connect,
	.disconnect=cls_brick_disconnect,
	};

// SELECTOR

t_brick_class brick_selector = {
	.cls_type="brick",
	.type=bt_selector,
	.make=brick_make_selector,
	.draw=brick_draw,
	.update=cls_brick_update,
	.trigger=cls_brick_trigger_selector,
	.init=cls_brick_init,
	.connect=cls_brick_connect,
	.disconnect=cls_brick_disconnect,
	};

// SWITCH

t_brick_class brick_switch = {
	.cls_type="brick",
	.type=bt_switch,
	.make=brick_make_switch,
	.draw=brick_draw,
	.update=cls_brick_update,
	.trigger=cls_brick_trigger_switch,
	.init=cls_brick_init,
	.connect=cls_brick_connect,
	.disconnect=cls_brick_disconnect,
	};


// CLS

t_brick_class *bricks[] = {
	&brick_trigger,
	&brick_slider,
	&brick_menu,
	&brick_selector,
	&brick_switch,
	};

void brick_make_trigger(t_brick *brick)
{
	brick->cls=&brick_trigger;
//	brick->state.draw_value=0;
}

void brick_make_slider(t_brick *brick)
{
	brick->cls=&brick_slider;
	brick->state.draw_value=1;
}

void brick_make_menu(t_brick *brick)
{
	brick->cls=&brick_menu;
	brick->state.draw_value=0;
}

void brick_make_selector(t_brick *brick)
{
	brick->cls=&brick_selector;
	brick->state.draw_value=1;
}

void brick_make_switch(t_brick *brick)
{
	brick->cls=&brick_switch;
	brick->state.draw_value=0;
}

// INIT

void brick_cls_init(t_brick *brick)
{
	int i;
	int n = sizeof(bricks)/sizeof(bricks[0]);
	int found=0;
	for(i=0;i<n;i++)
	{
		if(brick->type==bricks[i]->type)
		{
			bricks[i]->make(brick);
			found=1;
			break;
		}
	}

	if(!found)printf("[ERROR:brick_cls_init] Unknown brick type %d\n",brick->type);
}
