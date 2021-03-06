/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "op.h"

typedef enum Type_Parent
{
	t_parent_child,
	t_child_parent
}t_parent;

typedef enum Type_Operation 
{
	t_op_add,
	t_op_mult
}t_operation;


void brick_set_updated(t_brick *brick)
{
	brick->plug_in.is_updated = 1;
	brick->plug_intern.is_updated = 1;
	brick->plug_out.is_updated = 1;
}

// BRICK ADD

void *op_brick_add(t_brick *brick)
{
	t_context *C = ctx_get();

	// store
	C->scene->store=1;

	char *name=brick->name;

	     if(is(name,"frame")) 		add_slider_int(C,"frame",&C->app->frame); 
	else if(is(name,"timer"))  		add_slider_int(C,"timer",&C->app->timer); 
	else if(is(name,"=")) 			add_operator_double(C,"=");
	else if(is(name,">"))  			add_operator_double(C,">"); 
	else if(is(name,"<"))  			add_operator_double(C,"<"); 
	else if(is(name,"mod"))  		add_operator_double(C,"mod"); 
	else if(is(name,"x")) 			add_maths(C,"x");
	else if(is(name,"+"))  			add_maths(C,"+"); 
	else if(is(name,"++")) 
	{
		t_node *_node_block = add_slider_int_special(C,"++",op_plusplus); 
		t_block *_block = _node_block->data;
		t_brick *_brick = block_brick_get(_block,"++");
		_brick->plug_out.open_out = 0;
		_brick->plug_out.open_in = 0;

	}
	else if(is(name,"switch"))  		add_switch(C,"switch",NULL); 
	else if(is(name,"clone"))  		add_clone(C); 
	else if(is(name,"pipe")) 		add_pipe(C); 
	else if(is(name,"sec")) 		add_slider_int(C,"sec",&C->app->timer);
	else if(is(name,"int")) 		add_slider_int(C,"int",NULL);
	else if(is(name,"float")) 		add_slider_float(C,"float",NULL);
	else if(is(name,"10")) 			add_multiplier(C,"10");
	else if(is(name,"100")) 		add_multiplier(C,"100");
	else if(is(name,".1")) 			add_multiplier(C,".1");
	else if(is(name,".01")) 		add_multiplier(C,".01");
	else if(is(name,".001")) 		add_multiplier(C,".001");
	else if(is(name,"msec")) 		add_slider_int(C,"msec",&C->app->clock->msec);
	else if(is(name,"cos")) 		add_operator_single(C,"cos",op_cos);
	else if(is(name,"sin")) 		add_operator_single(C,"sin",op_sin);
	else if(is(name,"cam_pos_x")) 		add_slider_float(C,"cam pos x",&C->camera->pos[0]);
	else if(is(name,"cam_pos_y")) 		add_slider_float(C,"cam pos y",&C->camera->pos[1]);
	else if(is(name,"cam_pos_z")) 		add_slider_float(C,"cam pos z",&C->camera->pos[2]);
	else if(is(name,"cam_rot_xy")) 		add_slider_float_custom(C,"cam rot xy",op_camera_rotate_xy); 
	else if(is(name,"cam_rot_z")) 		add_slider_float_custom(C,"cam rot z",op_camera_rotate_z); 
	else if(is(name,"pos x")) 		add_slider_object(C,"pos x");
	else if(is(name,"pos y")) 		add_slider_object(C,"pos y");
	else if(is(name,"pos z")) 		add_slider_object(C,"pos z");
	else if(is(name,"rot x")) 		add_slider_object(C,"rot x");
	else if(is(name,"rot y")) 		add_slider_object(C,"rot y");
	else if(is(name,"rot z")) 		add_slider_object(C,"rot z");
	else if(is(name,"scl x")) 		add_slider_object(C,"scl x");
	else if(is(name,"scl y")) 		add_slider_object(C,"scl y");
	else if(is(name,"scl z")) 		add_slider_object(C,"scl z");
	else if(is(name,"red")) 		add_slider_object(C,"red");
	else if(is(name,"green")) 		add_slider_object(C,"green");
	else if(is(name,"blue")) 		add_slider_object(C,"blue");
	else if(is(name,"alpha")) 		add_slider_object(C,"alpha");
	else if(is(name,"label")) 		add_label(C,"label");
	else if(is(name,"get")) 		add_get(C);
	else if(is(name,"rewind")) 		add_trigger_always(C,"rewind",op_rewind); 
	else if(is(name,"not")) 		add_slider_int_custom(C,"not",NULL,op_not); 
	else if(is(name,"mouse_x")) 		add_slider_int(C,"mouse x",&C->app->mouse->x); 
	else if(is(name,"mouse_y")) 		add_slider_int(C,"mouse y",&C->app->mouse->y); 
	else if(is(name,"keyboard")) 		add_slider_char(C,"keyboard",&C->app->keyboard->key_pressed); 
	else if(is(name,"rnd")) 		add_slider_int_custom(C,"rnd",NULL,op_rnd);
	else if(is(name,"neg")) 		add_slider_int_custom(C,"neg",NULL,op_neg);
	else if(is(name,"last?")) 		add_switch_custom(C,"last?",NULL,op_is_last);
	else if(is(name,"for")) 		add_for(C);
	else if(is(name,"vector")) 		add_vector(C);
	else if(is(name,"bang")) 		add_slider_int_custom(C,"bang",NULL,op_bang); 
	else if(is(name,"quit")) 		add_trigger_always(C,"quit",op_do_quit); 
	else if(is(name,"const")) 		add_const(C); 
	else if(is(name,"and")) 		add_maths(C,"and"); 

	// store
	C->scene->store=0;

	brick_set_updated(brick);

	return NULL;
}

// SLIDER

void *op_slider(t_brick *brick)
{
	t_context *C = ctx_get();

	float dx = C->app->mouse->dx;
	int delta_x = C->app->mouse->delta_x;

	t_plug *plug_intern= &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	// trigger
	//if(!C->event->is_brick_transformed && brick->mode==bm_triggering) // (see:plug_update)
	if(brick->mode==bm_triggering) // (see:plug_update)
	{
		// dragging
		if(brick->state.is_draging)
		{
			// release
			if(C->app->mouse->button_left==button_released)
			{
				brick->state.is_draging=0;
				brick_release(brick);
			}
			else
			{
				if(brick->plug_intern.data_type==dt_int)
				{
					int *data=brick->plug_intern.data;
					if(delta_x>0) *data 	+=  dx;
					else *data 		-=  dx;
				}
				else if(brick->plug_intern.data_type==dt_float)
				{
					float inc=.1;
					float *data=brick->plug_intern.data;
					if(delta_x>0) *data 	+= inc * dx;
					else *data 		-= inc * dx;
				}
			}
		}
		// simple clic
		else
		{
			// set dragging
			if(C->app->mouse->button_left == button_pressed && C->app->mouse->is_moving)
			{
				brick->state.is_draging=1;
			}
			else
			{
				// exec
				if(!brick->state.is_done)
				{
					brick->state.is_done=1;

					if(brick->plug_intern.data_type==dt_int)
					{

						int *data=brick->plug_intern.data;
						if(brick->state.is_left_pressed) 	*data -= 1; 
						else if(brick->state.is_right_pressed) 	*data += 1; 
					}
					else if(brick->plug_intern.data_type==dt_float)
					{
						float inc=.1;
						float *data=brick->plug_intern.data;
						if(brick->state.is_left_pressed)  	*data -= inc; 
						else if(brick->state.is_right_pressed) 	*data += inc; 
					}
				}
				// release
				else
				{
					if(C->app->mouse->button_left==button_released)
					{
						brick->state.is_done=0;
						brick_release(brick); 
					}
				}
			}
		}
	}

	// update
	brick_set_updated(brick);

	return NULL;
}

// SLIDER POSITIVE

void *op_slider_positive(t_brick *brick)
{
	// slide
	op_slider(brick);

	// set positive
	int *val = brick->plug_intern.data;
	if(*val < 0) *val=0; 

	// update
	brick_set_updated(brick);

	return NULL;
}

void *op_slider_positive_non_zero(t_brick *brick)
{
	// slide
	op_slider(brick);

	// set positive
	int *val = brick->plug_intern.data;
	if(*val < 1) *val=1; 

	// update
	brick_set_updated(brick);

	return NULL;
}

// SELECTOR

void *op_selector(t_brick *brick)
{
	t_context *C = ctx_get();
	int *target = brick->plug_intern.data;

	if(brick->state.is_mouse_over)
	{
		if(C->app->mouse->button_left==button_pressed)
		{
			// switch state when released
			if(brick->state.is_released)
			{
				brick->state.is_released=0;
				// switch value

				if(brick->var.selector<brick->var.selector_length)
				{
					brick->var.selector++;
					(*target)++;
				}
				else
				{
					brick->var.selector=0;
					*target=0;
				}
			}
		}
	}

	// release
	if(!brick->state.is_released && C->app->mouse->button_left==button_released)
	{
		brick->state.is_released=1;
		brick_release(brick);
	}

	brick_set_updated(brick);

	return NULL;
}

// SWITCH

void *op_switch(t_brick *brick)
{
	t_context *C = ctx_get();

	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	if(brick->mode==bm_triggering) 
	{
		int *target = brick->plug_intern.data;

		if(brick->state.is_left_pressed || brick->state.is_right_pressed)
		{
			if(C->app->mouse->button_left==button_pressed)
			{
				// switch state when released
				if(brick->state.is_released)
				{
					brick->state.is_released=0;
					// switch value
					if(*target == 0)  *target=1; 
					else  *target=0; 
				}
			}
		}
	}

	// release
	if(!brick->state.is_released && C->app->mouse->button_left==button_released)
	{
		brick->state.is_released=1;
		brick_release(brick);
	}

	brick_set_updated(brick);

	return NULL;
}

// >

void *op_superior(t_brick *brick)
{

	t_block *block=brick->block;

	t_brick *brick_val_1 = block_brick_get(block,"a");
	t_brick *brick_val_2 = block_brick_get(block,"b");
	t_brick *brick_result = brick;

	t_plug *plug_val_1 = &brick_val_1->plug_intern;
	t_plug *plug_val_2 = &brick_val_2->plug_intern;
	t_plug *plug_result = &brick_result->plug_intern;

	float *f1;
	float *f2;

	int *i1;
	int *i2;

	int *result = plug_result->data;

	t_data_type type_1 = plug_val_1->data_type;
	t_data_type type_2 = plug_val_2->data_type;

	t_data_type type_common  = dt_null; 

	if(type_1 == type_2) type_common = type_1;

	if(type_common != dt_null)
	{
		if(type_common == dt_float)
		{
			f1 = plug_val_1->data;
			f2 = plug_val_2->data;

			if(*f1 > *f2) *result = 1;
			else *result = 0;
		}
		else
		{
			i1 = plug_val_1->data;
			i2 = plug_val_2->data;

			if(*i1 > *i2) *result = 1;
			else *result = 0;
		}
	}
	else
	{
		*result = 0;
	}

	brick_set_updated(brick);

	return NULL;
}

// <

void *op_inferior(t_brick *brick)
{

	t_block *block=brick->block;

	t_brick *brick_val_1 = block_brick_get(block,"a");
	t_brick *brick_val_2 = block_brick_get(block,"b");
	t_brick *brick_result = brick;

	t_plug *plug_val_1 = &brick_val_1->plug_intern;
	t_plug *plug_val_2 = &brick_val_2->plug_intern;
	t_plug *plug_result = &brick_result->plug_intern;

	float *f1;
	float *f2;

	int *i1;
	int *i2;

	int *result = plug_result->data;

	t_data_type type_1 = plug_val_1->data_type;
	t_data_type type_2 = plug_val_2->data_type;

	t_data_type type_common  = dt_null; 

	if(type_1 == type_2) type_common = type_1;

	if(type_common != dt_null)
	{
		if(type_common == dt_float)
		{
			f1 = plug_val_1->data;
			f2 = plug_val_2->data;

			if(*f1 < *f2) *result = 1;
			else *result = 0;
		}
		else
		{
			i1 = plug_val_1->data;
			i2 = plug_val_2->data;

			if(*i1 < *i2) *result = 1;
			else *result = 0;
		}
	}
	else
	{
		*result = 0;
	}

	brick_set_updated(brick);

	return NULL;
}

// EQUAL

void *op_equal(t_brick *brick)
{

	t_block *block=brick->block;

	t_brick *brick_val_1 = block_brick_get(block,"a");
	t_brick *brick_val_2 = block_brick_get(block,"b");
	t_brick *brick_result = brick;

	t_plug *plug_val_1 = &brick_val_1->plug_intern;
	t_plug *plug_val_2 = &brick_val_2->plug_intern;
	t_plug *plug_result = &brick_result->plug_intern;

	float *f1;
	float *f2;

	int *i1;
	int *i2;

	int *result = plug_result->data;

	t_data_type type_1 = plug_val_1->data_type;
	t_data_type type_2 = plug_val_2->data_type;

	t_data_type type_common  = dt_null; 

	if(type_1 == type_2) type_common = type_1;

	if(type_common != dt_null)
	{
		if(type_common == dt_float)
		{
			f1 = plug_val_1->data;
			f2 = plug_val_2->data;

			if(*f1 == *f2) *result = 1;
			else *result = 0;
		}
		else
		{
			i1 = plug_val_1->data;
			i2 = plug_val_2->data;

			if(*i1 == *i2) *result = 1;
			else *result = 0;
		}
	}
	else
	{
		*result = 0;
	}

	brick_set_updated(brick);

	return NULL;
}

// EQUAL

void *op_mod(t_brick *brick)
{

	t_block *block=brick->block;

	t_brick *brick_val_1 = block_brick_get(block,"a");
	t_brick *brick_val_2 = block_brick_get(block,"b");
	t_brick *brick_result = brick;

	t_plug *plug_val_1 = &brick_val_1->plug_intern;
	t_plug *plug_val_2 = &brick_val_2->plug_intern;
	t_plug *plug_result = &brick_result->plug_intern;

	float *f1;
	float *f2;

	int *i1;
	int *i2;

	int *result = plug_result->data;

	t_data_type type_1 = plug_val_1->data_type;
	t_data_type type_2 = plug_val_2->data_type;

	t_data_type type_common  = dt_null; 

	if(type_1 == type_2) type_common = type_1;

	if(type_common != dt_null)
	{
		if(type_common == dt_float)
		{
			f1 = plug_val_1->data;
			f2 = plug_val_2->data;

			if(*f1 == *f2) *result = 1;
			else *result = 0;
		}
		else
		{
			i1 = plug_val_1->data;
			i2 = plug_val_2->data;

			if(*i2) *result = *i1 % *i2;
		}
	}
	else
	{
		*result = 0;
	}

	brick_set_updated(brick);

	return NULL;
}

// ++

void *op_plusplus(t_brick *brick)
{
	op_slider(brick);

	t_plug *plug_intern = &brick->plug_intern;
	t_plug *plug_out=&brick->plug_out;

	int *intern = plug_intern->data;
	int *data;

	if(plug_out->is_connected && (*intern == 1))
	{
		t_plug *plug_dst = plug_get_dst(plug_intern);

		switch(plug_dst->data_type)
		{
			case(dt_int):
				data = plug_dst->data;
				(*data)++;

				break;
			default:
				break;
		}
	}

	brick_set_updated(brick);

	return NULL;
}

void exe_remove_brick(t_dict *args)
{
	t_context *C = ctx_get();
	// remove brick

	t_brick *_brick = dict_pop_data(args,"brick");
	t_block *block = _brick->block;

	t_plug *plug_intern = &_brick->plug_intern;
	int *slider = plug_intern->data;

	t_lst *bricks=block->bricks;
	t_link *last=bricks->last;
	t_brick *brick=NULL;
	if(last) brick=last->data;

	if(brick)
	{
		if(brick_delete(brick))
		{
			lst_link_remove(bricks,last);
			scene_struct_delete(C->scene,last);
			block->tot_bricks--;
		}
		else
		{
			*slider=block->tot_bricks;
		}
	}
}

void exe_add_brick_parent_child(t_dict *args)
{
	t_context *C = ctx_get();

	t_brick *brick = dict_pop_data(args,"brick");

	t_block *block=brick->block;
	t_plug *plug_intern=&brick->plug_intern;

	C->scene->store=1;

	//add brick

	t_node *n=add_part_slider_float(C,block,".",NULL);
	t_brick *b=n->data;
	b->state.is_versatil=1;

	t_plug *p_intern = &b->plug_intern;
	t_plug *p_in = &b->plug_in;
	
	plug_add_parent(p_intern,plug_intern);
	p_in->follow_in=0;

	C->scene->store=0;

	block->state.set_global_width=1;
}

void exe_add_brick_child_parent(t_dict *args)
{
	t_context *C = ctx_get();

	t_brick *brick = dict_pop_data(args,"brick");
	t_brick *brick_target = dict_pop_data(args,"target");

	t_block *block=brick->block;

	C->scene->store=1;

	t_node *n=add_part_slider_float(C,block,".",NULL);
	t_brick *b=n->data;
	b->state.is_versatil=1;

	t_plug *p_intern = &b->plug_intern;
	t_plug *p_in = &b->plug_in;
	
	t_plug *plug_target = &brick_target->plug_intern;
	plug_add_parent(plug_target,p_intern);
	p_in->follow_in=1;

	C->scene->store=0;

	block->state.set_global_width=1;
}

void action_free(t_action *action)
{
	t_context *C = ctx_get();
	t_scene *sc = C->scene;
	if(action->args) scene_struct_delete(sc,action->args);
	free(action);
}

t_action *action_new(const char *name)
{
	t_action *action = (t_action *)malloc(sizeof(t_action));
	
	action->act = NULL;
	action->args = NULL;

	return action;
}

void add_exe_add_brick(t_brick *brick,t_brick *brick_target,void (* f)(t_dict *))
{
	t_action *action = action_new("action");

	action->act = f;

	t_node *node_dict = dict_add("args");
	t_dict *dict = node_dict->data;
	action->args = dict;

	dict_symbol_add(action->args,"brick",dt_null,brick);
	dict_symbol_add(action->args,"target",dt_null,brick_target);

	exe_add_action(action);
}

void add_exe_remove_brick(t_brick *brick)
{
	t_action *action = action_new("action");

	action->act = exe_remove_brick;

	t_node *node_dict = dict_add("args");
	t_dict *dict = node_dict->data;
	action->args = dict;

	dict_symbol_add(action->args,"brick",dt_null,brick);

	exe_add_action(action);
}

// ADD BRICKS

void op_add_bricks(t_brick *brick,t_brick *brick_target,int n,t_parent parent)
{
	// slide
	op_slider_positive(brick);

	t_block *block=brick->block;
	t_plug *plug_intern=&brick->plug_intern;
	int tot_bricks=block->tot_bricks;
	int *slider = plug_intern->data;

	if(slider==0)
	{
	}
	// add brick
	else if( *slider > tot_bricks-n) 
	{
		if(parent == t_parent_child)
			add_exe_add_brick(brick,brick_target,exe_add_brick_parent_child);
		else
			add_exe_add_brick(brick,brick_target,exe_add_brick_child_parent);

	}
	// remove brick
	else if(*slider < tot_bricks -n)
	{
		add_exe_remove_brick(brick);
	}

}


void *op_pipe(t_brick *brick)
{
	// add bricks
	op_add_bricks(brick,NULL,2,t_parent_child); //XXX 2

	t_block *block = brick->block;
	t_plug *plug_intern = &brick->plug_intern;
	int tot_bricks = block->tot_bricks;

	// flow
	plug_intern->cls->flow(plug_intern);

	t_brick *brick_state = block_brick_get(block,"state");
	t_plug *plug_state = &brick_state->plug_intern;
	int *state = plug_state->data;

	// update bricks
	if(tot_bricks>0)
	{
		t_link *l;
		t_brick *b;

		for(l = block->bricks->first;l;l = l->next)
		{
			b = l->data;
			if(!is(b->name,"clone"))
			{
				t_plug *plug_clone = &b->plug_intern;
				t_plug *plug_in_clone = &b->plug_in;
				t_plug *plug_out_clone = &b->plug_out;

				if(plug_in_clone->is_connected)
				{
					t_plug *plug_src = plug_get_src(plug_clone);

					// match current target type
					if(plug_clone->data_type != plug_src->data_type)
					{
						t_brick *brick_clone = plug_clone->brick;
						brick_type_change(brick_clone,plug_src);
					}
				}

				plug_out_clone->open_out = *state;

			}
		}
	}

	//XXX!!! set updated missing ??
	return NULL;
}


// CLONE

void *op_clone(t_brick *brick)
{
	// add bricks
	op_add_bricks(brick,NULL,1,t_parent_child);

	t_block *block = brick->block;
	t_plug *plug_in = &brick->plug_in;
	t_plug *plug_intern = &brick->plug_intern;
	t_plug *plug_out = &brick->plug_out;
	int tot_bricks = block->tot_bricks;

	// flow
	plug_intern->cls->flow(plug_intern);

	// update bricks
	if(tot_bricks>0)
	{
		if(plug_in->is_connected)
		{
			t_link *l;
			t_brick *b;

			t_plug *plug_src = plug_get_src(plug_intern);

			for(l = block->bricks->first;l;l = l->next)
			{
				b = l->data;
				if(!is(b->name,"clone"))
				{
					t_plug *plug_clone = &b->plug_intern;
					t_plug *plug_in_clone = &b->plug_in;

					// match current target type
					if(plug_clone->data_type != plug_src->data_type)
					{
						t_brick *brick_clone = plug_clone->brick;
						brick_type_change(brick_clone,plug_src);
					}

					// connect 
					t_plug *_plug_out = plug_in->src;

					plug_in_clone->src=_plug_out;
					plug_in_clone->is_connected=1;
				}
			}
		}

		if(plug_out->is_connected)
		{
			t_link *l;
			t_brick *b;

			t_plug *plug_src = plug_get_dst(plug_intern);

			for(l = block->bricks->first;l;l = l->next)
			{
				b = l->data;
				if(!is(b->name,"clone"))
				{
					t_plug *plug_clone = &b->plug_intern;
					t_plug *plug_in_clone = &b->plug_in;

					if(plug_clone->data_type != plug_src->data_type)
					{
						t_brick *brick_clone = plug_clone->brick;
						brick_type_change(brick_clone,plug_src);
					}

					t_plug *_p_out = plug_out->dst;
					t_plug *_p_out_i = _p_out->dst;
					t_plug *_plug_out = _p_out_i->dst;

					plug_in_clone->src=_plug_out;
					plug_in_clone->is_connected=1;

					b->cls->trigger(b);

				}
			}
		}
	}

	brick_set_updated(brick);

	return NULL;
}


// SIN

void *op_sin(t_brick *brick)
{
	t_plug *plug_intern= &brick->plug_intern;
	float *data = brick->plug_intern.data;

	// flow
	plug_intern->cls->flow(plug_intern);

	// compute
	t_plug *plug_in=&brick->plug_in;
	if(plug_in->is_connected) *data=sin(*data);

	// release
	if(brick->mode==bm_triggering) brick_release(brick);

	brick_set_updated(brick);

	return NULL;
}

// COS

void *op_cos(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;
	float *data = brick->plug_intern.data;

	// flow
	plug_intern->cls->flow(plug_intern);

	// compute
	t_plug *plug_in=&brick->plug_in;
	if(plug_in->is_connected) *data=cos(*data);

	// release
	if(brick->mode==bm_triggering) brick_release(brick);

	brick_set_updated(brick);

	return NULL;
}

// WINDOW

void *op_window(t_brick *brick)
{
	op_slider(brick);

	t_context *C=ctx_get();
	t_app *app = app_get();
	app->window->viewport_width=app->window->width;
	app->window->viewport_height=app->window->height;
	glutReshapeWindow(app->window->width,app->window->height);
	op_camera_frustum_init(C->camera);

	brick_set_updated(brick);

	return NULL;
}

// CAMERA

void *op_camera_rotate_xy(t_brick *brick)
{
	t_context *C=ctx_get();

	op_slider(brick);

	float *data=brick->plug_intern.data;

	op_camera_rotate(C,*data,0);	

	brick_set_updated(brick);

	return NULL;
}

// CAMERA

void *op_camera_rotate_z(t_brick *brick)
{
	t_context *C=ctx_get();

	op_slider(brick);

	float *data=brick->plug_intern.data;

	op_camera_rotate(C,0,*data);	

	brick_set_updated(brick);

	return NULL;
}

// DELETE

void *op_delete(t_brick *brick)
{
	t_context *C = ctx_get();
	if(C->scene->selected)
	{
		t_node *node=C->scene->selected;

		node->cls->del(node);

		ctx_ui_menu_hide(C);
		brick_release(brick);
		C->scene->selected=NULL;
	}

	brick_set_updated(brick);

	return NULL;
}

// MENU

void *op_menu(t_brick *brick)
{
	brick_release(brick);

	brick_set_updated(brick);

	return NULL;
}

// VOID

void *op_void(t_brick *brick)
{
	brick_release(brick);

	brick_set_updated(brick);

	return NULL;
}

// POINTER

void *op_pointer(t_brick *brick)
{
	t_plug *plug_intern=&brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	brick_set_updated(brick);

	return NULL;
}

// ADD

void op_maths_plug(t_operation operation,t_plug *dst,t_plug *src)
{

	t_context *C=ctx_get();

	t_data_type type_dst=dst->data_type;
	t_data_type type_src=src->data_type;

	int *dst_int;
	float *dst_float;
	t_vlst *dst_vlst;

	void *data_src=src->data;

	int count;
	int length; 
	int i,j;

	float *data_dst;
	float *data;
	t_data_type type=dt_null;

	t_plug *plug_src;

	switch(type_dst)
	{
		case(dt_int):

			dst_int=dst->data;

			switch(type_src)
			{
				case(dt_int):

					switch(operation)
					{
						case (t_op_add): 	*dst_int += drf_int(data_src); break;
						case (t_op_mult): 	*dst_int *= drf_int(data_src); break;
					}

					break;

				case(dt_float):
					
					switch(operation)
					{
						case(t_op_add): 	*dst_int += (int)drf_float(data_src); break;
						case(t_op_mult): 	*dst_int *= (int)drf_float(data_src); break;
					}

					break;

				default:
					break;
			}

			break;

		case(dt_float):

			dst_float=dst->data;

			switch(type_src)
			{

				case(dt_int):

					switch(operation)
					{
						case(t_op_add):		*dst_float += (float)drf_int(data_src); break;
						case(t_op_mult):	*dst_float *= (float)drf_int(data_src); break;
					}

					break;

				case(dt_float):
				
					switch(operation)
					{
						case(t_op_add):		*dst_float += drf_float(data_src); break;
						case(t_op_mult):	*dst_float *= drf_float(data_src); break;
					}
					
					break;

				default:
					break;
			}

			break;

		case(dt_vlst):

			dst_vlst=dst->data;

			count=dst_vlst->count;
			length=dst_vlst->length; 

			data_dst=dst_vlst->data;

			plug_src=src->src;

			for(i=0;i<count;i++)
			{
				for(j=0;j<length;j++)
				{
					if(plug_src->is_connected)
					{

						C->event->update_connections=1;

						t_plug *plug_source=plug_get_src(src);
						t_brick *brick_source=plug_source->brick;

						brick_source->cls->trigger(brick_source);

						data=plug_source->data;
						type=plug_source->data_type;

						C->event->update_connections=0;
					}

					switch(type)
					{
						case(dt_int):

							switch(operation)
							{
								case(t_op_add):	 	*data_dst += (float)drf_int(data); break;
								case(t_op_mult):	*data_dst *= (float)drf_int(data); break;
							}

							break;

						case(dt_float):

							switch(operation)
							{
								case(t_op_add):	 	*data_dst += drf_float(data); break;
								case(t_op_mult):	*data_dst *= drf_float(data); break;
							}

							break;
							
						default:
							break;
					}

					data_dst++;
				}
			}


			break;

		

		default:printf("[?]\n");break;

	}
}

t_vlst *get_first_vlst(t_block *block)
{
	t_link *l;
	t_brick *b;
	t_plug *p;

	// offset first two bricks

	l=block->bricks->first;
	l=l->next;
	l=l->next;

	for(;l;l=l->next)
	{
		b=l->data;
		p=&b->plug_intern;
		switch(p->data_type)
		{
			case(dt_vlst):return p->data ;break;
			default:break;
		}
	}

	return NULL;

}

int get_first_vlst_pos(t_block *block)
{
	t_link *l;
	t_brick *b;
	t_plug *p;

	// offset first two bricks

	l=block->bricks->first;
	l=l->next;
	l=l->next;

	int pos=2;

	for(;l;l=l->next)
	{
		b=l->data;
		p=&b->plug_intern;
		switch(p->data_type)
		{
			case(dt_vlst):return pos;break;
			default:break;
		}
		pos++;
	}

	return pos;
}

void op_maths_exe(t_operation operation,t_brick *brick)
{
	t_link *l;
	t_brick *b;
	t_plug *p;
	t_block *block = brick->block;

	t_brick *brick_result=block_brick_get(block,"result");
	t_plug *plug_result=&brick_result->plug_intern;

	int pos=0;

	pos=get_first_vlst_pos(block);

	// offset first two bricks operator, result
	l=block->bricks->first;
	l=l->next;
	l=l->next;

	// get type
	int this_pos=2;
	for(;l;l=l->next)
	{
		b=l->data;
		p=&b->plug_intern;
		// don't evaluate result plug
		if(this_pos != pos) op_maths_plug(operation,plug_result,p);
		this_pos++;
	}
}

void *op_maths(t_operation operation,t_brick *brick)
{

	// get result type

	int is_int=0;
	int is_float=0;
	int is_vlst=0;

	//int pos=0;

	t_link *l;
	t_brick *b;
	t_plug *p;

	t_block *block=brick->block;
	t_brick *brick_result=block_brick_get(block,"result");
	t_plug *plug_result=&brick_result->plug_intern;

	// ADD BRICKS
	op_add_bricks(brick,brick_result,2,t_child_parent);

	// offset first two bricks
	l=block->bricks->first;
	l=l->next;
	l=l->next;

	// get type
	for(;l;l=l->next)
	{
		b=l->data;
		p=&b->plug_intern;
		switch(p->data_type)
		{
			case(dt_int):is_int=1;break;
			case(dt_float):is_float=1;break;
			case(dt_vlst):is_vlst=1;break;
			default:break;
		}
	}

	// change type
	// init result

	if(is_vlst)
	{
		brick_change_type_by_name(brick_result,dt_vlst);
		t_vlst *v=get_first_vlst(block);
		//pos=get_first_vlst_pos(block);
		if(v) brick_result->plug_intern.data=v;
		else printf("[ERROR] Can't get vlst\n");
	}
	else if(is_float)
	{
		brick_change_type_by_name(brick_result,dt_float);
		float *r=plug_result->data;
		switch(operation)
		{
			case(t_op_add): *r=0; break;
			case(t_op_mult): *r=1; break;
		}
	}
	else if(is_int)
	{
		brick_change_type_by_name(brick_result,dt_int);
		int *r=plug_result->data;

		switch(operation)
		{
			case(t_op_add): *r=0; break;
			case(t_op_mult): *r=1; break;
		}
	}

	return NULL;
}

void *op_add(t_brick *brick)
{
	op_maths(t_op_add,brick);
	op_maths_exe(t_op_add,brick);
	brick_set_updated(brick);
	return NULL;
}

void *op_mult(t_brick *brick)
{
	op_maths(t_op_mult,brick);
	op_maths_exe(t_op_mult,brick);
	brick_set_updated(brick);
	return NULL;
}

void *op_and(t_brick *brick)
{
	t_link *l;
	t_brick *b;
	t_plug *p;

	t_block *block=brick->block;
	t_brick *brick_result=block_brick_get(block,"result");
	t_plug *plug_result=&brick_result->plug_intern;
	int *result = plug_result->data;

	// ADD BRICKS
	op_add_bricks(brick,brick_result,2,t_child_parent);

	int pos=0;

	pos=get_first_vlst_pos(block);

	// offset first two bricks operator, result
	l=block->bricks->first;
	l=l->next;
	l=l->next;

	int r=1;

	// get type
	int this_pos=2;
	for(;l;l=l->next)
	{
		b=l->data;
		p=&b->plug_intern;
		// don't evaluate result plug
		if(this_pos != pos)
		{
			int *_r = p->data;
			if(*_r == 0) r=0;

		}
		this_pos++;
	}

	*result = r;

	return NULL;

}

// SELECT

void *op_set_selected(t_brick *brick)
{
	t_context *C=ctx_get();
	t_plug *plug=&brick->plug_intern;

	// flow
	plug->cls->flow(plug);

	if(brick->mode==bm_triggering) 
	{
		if(plug->data)
		{
			t_node *node=plug->data;
			ctx_scene_set_selected(C,node);
		}
	}

	brick_set_updated(brick);

	return NULL;
}

// GET

void *__op_get(t_brick *brick)
{
	t_plug *plug=&brick->plug_intern;
	t_plug *plug_in=&brick->plug_in;
	t_plug *plug_out=&brick->plug_out;
	t_plug *plug_src=NULL;

	t_plug *plug_src_in;

	if(plug_in->is_connected)
	{
		t_plug *plug_source=plug_in->src;
		t_brick *brick_in=plug_source->brick;
		plug_src=&brick_in->plug_intern;
		plug_src_in = &brick_in->plug_in;
	}

	t_lst *lst;
	t_link *link;

	if(plug_src)
	{
		// if src is type lst
		//if(plug_src->data_type==dt_lst && plug_src->is_ready)
		if(plug_src->data_type==dt_lst && plug_src_in->open_out)
		{
			// set ready
			//plug->is_ready=1;
			plug_out->open_out=1;
			lst=plug_src->data;

			// if lst
			if(lst)
			{
				if(!lst->current)
				{
					if(lst->first)
					{
						printf("op_get set first\n");
						lst->current=lst->first;
					}
				}
				else
				{
					link=lst->current;
					if(link->next)
					{
						printf("op_get set next\n");
						lst->current=link->next;
					}

					//XXX change type
					plug->data_type=dt_camera;
					t_camera *camera=link->data;
					plug->data=camera;
				}
			}
		}
		else
		{
			//plug->is_ready=0;
			plug_out->open_out=0;
		}
	}

	brick_set_updated(brick);

	return NULL;
}

// REWIND

void *op_rewind(t_brick *brick)
{
	t_plug *plug_intern=&brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	// update
	brick_set_updated(brick);

	return NULL;
}

// NOT

void *op_not(t_brick *brick)
{
	t_plug *plug_intern=&brick->plug_intern;
	t_plug *plug_in=&brick->plug_in;

	// flow
	plug_intern->cls->flow(plug_intern);

	int *data=plug_intern->data;

	switch(plug_intern->data_type)
	{
		case(dt_int):

			if(plug_in->is_connected)
			{
				if(*data == 0) *data = 1;
				else *data =0;
			}

			break;
		default:printf("op_not need an int\n");break;
	}

	brick_set_updated(brick);

	return NULL;
}

// SET VLST

int _pressed=0;
int _released=0;

void *op_set_vlst(t_brick *brick)
{
	t_context *C=ctx_get();

	op_slider_positive_non_zero(brick);

	t_block *block=brick->block;

	//XXX not color ...
	t_brick *brick_vlst=block_brick_get(block,"vertex");

	t_vlst *vlst=brick_vlst->plug_intern.data;

	if(C->app->mouse->button_left == button_pressed)
	{
		_pressed=1;
	}
	
	if(_pressed== 1 && C->app->mouse->button_left == button_released)
	{
		_pressed=0;
		_released=1;
	}

	if(C->app->mouse->button_left==button_released)
	{
		if(vlst->count != vlst->count_new) 
		{
			if(_released)
			{
				_released=0;
				vlst_update_data(vlst,NULL);
			}
		}
	}

	brick_set_updated(brick);

	return NULL;
}

// SET COLORS

void *op_set_colors(t_brick *brick)
{
	t_context *C=ctx_get();

	op_slider_positive(brick);

	t_block *block=brick->block;

	t_brick *brick_vlst=block_brick_get(block,"colors");

	t_vlst *vlst=brick_vlst->plug_intern.data;

	if(C->app->mouse->button_left==button_released)
	{
		if(vlst->count != vlst->count_new) vlst_update_data(vlst,NULL);
	}

	brick_set_updated(brick);

	return NULL;
}

// RND

void *op_rnd(t_brick *brick)
{
	t_plug *plug_intern=&brick->plug_intern;
	int *data=plug_intern->data;
	*data=u_randrange(0,100);

	brick_set_updated(brick);

	return NULL;
}

// NEG

void *op_neg(t_brick *brick)
{
	t_plug *plug_intern=&brick->plug_intern;
	t_plug *plug_in=&brick->plug_in;

	// flow
	plug_intern->cls->flow(plug_intern);

	// negate
	if(plug_in->is_connected)
		plug_data_negate(plug_intern);

	// update
	brick_set_updated(brick);

	return NULL;
}

// IS_LAST

void *op_is_last(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;
	t_plug *plug_src=plug_get_src(plug_intern);

	int *data = plug_intern->data;

	if(plug_src)
	{
		if(plug_src->data_type==dt_lst)
		{
			t_lst *lst = plug_src->data;

			if(lst->current && lst->last)
			{
				if(lst->current == lst->last)
				{
					*data = 1;
				}
				else
				{
					*data = 0;
				}
			}
			else
			{
				*data = 0;
			}
		}
	}
	else
	{
		*data = 0;
	}

	brick_set_updated(brick);

	return NULL;
}

// FOR

void *op_for(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	brick_set_updated(brick);

	return NULL;
}

// OPERATOR

void *op_operator(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	brick_set_updated(brick);

	return NULL;
}


// VECTOR

void *op_vector(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	brick_set_updated(brick);

	return NULL;
}

// BANG

void *op_bang(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	int *state = plug_intern->data;
	int *bang = &plug_intern->bang;

	// [TRUE] : IN (true) + BANG (false) = BANG (true)
	// let true pass throw
	if(*state == 1 && !(*bang))
	{
		*bang = 1;
	}
	// [FALSE] IN (false) && BANG (true) = BANG (flase)
	else if(*state == 0 && *bang)
	{
		*bang=0;
	}
	// ELSE [FALSE]
	else
	{
		*state=0;
	}

	brick_set_updated(brick);

	return NULL;
}

void *op_float(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	if(brick->mode == bm_triggering)	brick_release(brick);
	brick_set_updated(brick);

	return NULL;
}


void *op_int(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	if(brick->mode == bm_triggering)	brick_release(brick);
	brick_set_updated(brick);

	return NULL;
}

void *op_do_quit(t_brick *brick)
{
	t_plug *plug_intern = &brick->plug_intern;

	// flow
	plug_intern->cls->flow(plug_intern);

	int i = drf_int(plug_intern->data);

	if(i) op_quit(NULL);

	brick_set_updated(brick);

	return NULL;
}

void *op_const(t_brick *brick)
{
	t_block *block = brick->block;

	t_brick *brick_i = block_brick_get(block,"i");

	t_plug *plug_const = &brick->plug_intern;
	t_plug *plug_i = &brick_i->plug_intern;

	int *_const = plug_const->data;
	int *_i = plug_i->data;

	// flow
	plug_const->cls->flow(plug_const);

	if(!plug_const->is_eval)
	{
		plug_const->is_eval = 1;

		*_const = *_i;
	}

	brick_set_updated(brick);

	if(brick->mode == bm_triggering)
		brick_release(brick);

	return NULL;
}


