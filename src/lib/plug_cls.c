/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "op.h"

typedef enum Plug_Mode
{
	mode_in,
	mode_out
}t_plug_mode;


void cls_plug_make_float(t_plug *plug);
void cls_plug_make_int(t_plug *plug);
void cls_plug_make_string(t_plug *plug);
void cls_plug_make_pointer(t_plug *plug);

void cls_plug_make_mesh(t_plug *plug);
void cls_plug_make_vertex(t_plug *plug);
void cls_plug_make_face(t_plug *plug);
void cls_plug_make_vlst(t_plug *plug);
void cls_plug_make_lst(t_plug *plug);
void cls_plug_make_camera(t_plug *plug);
void cls_plug_make_char(t_plug *plug);
void cls_plug_make_object(t_plug *plug);
void cls_plug_make_selector(t_plug *plug);
void cls_plug_make_trigger(t_plug *plug);
void cls_plug_make_operator(t_plug *plug);
void cls_plug_make_vector(t_plug *plug);


void plug_data_reset(t_plug *plug)
{
	int *zero;
	zero=0;

	switch (plug->data_type)
	{
		case (dt_int): flow_int_int(plug->data,zero); break;
		case (dt_float): flow_float_int(plug->data,zero); break;
		default: break;
	}
}

void plug_data_set(t_plug *plug,t_data_type type,void *data)
{
	switch (plug->data_type)
	{
		case (dt_int):

			switch(type)
			{
				case(dt_int): flow_int_int(plug->data,data); break;
				case(dt_float): flow_int_float(plug->data,data); break;
				default: break;
			}

			break;

		case (dt_float):

			switch(type)
			{
				case(dt_int): flow_float_int(plug->data,data); break;
				case(dt_float): flow_float_float(plug->data,data); break;
				default: break;
			}

			break;

		default:
			break;
	}
}


void plug_data_negate(t_plug *plug)
{
	switch (plug->data_type)
	{
		case (dt_int): negate_int(plug->data); break;
		case (dt_float): negate_float(plug->data); break;
		default: break;
	}
}

			

// input: 	plug_intern 
// return: 	plug_intern
t_plug *plug_get_src(t_plug *plug)
{
	// get plug_in
	t_plug *plug_src=plug->src;
	// get plug_out
	t_plug *plug_src_out=plug_src->src;

	if(plug_src_out)
	{
		//get plug_intern of out
		t_plug *plug_source=plug_src_out->src;

		// return target's plug intern
		return plug_source;
	}
	else
	{
		return NULL;
	}
}

t_plug *plug_get_dst(t_plug *plug)
{
	// get plug_out
	t_plug *plug_dst = plug->dst;
	// get dst
	t_plug *plug_dst_out = plug_dst->dst;

	if(plug_dst_out)
	{
		// get plug_intern
		t_plug *plug_distant = plug_dst_out->dst;

		return plug_distant;
	}
	else
	{
		return NULL;
	}
}

void plug_warning(t_plug *dst_plug,t_plug *src_plug)
{

	t_context *C=ctx_get();
	char msg[40];
	char *src_plug_type=data_name_get(src_plug->data_type);
	char *dst_plug_type=data_name_get(dst_plug->data_type);

	sprintf(msg,"%d(%s)(%s)-(%s)(%s)",C->app->frame,src_plug->name,src_plug_type,dst_plug->name,dst_plug_type);

	term_print(C->term,msg);
}

// FLOW

void _cls_flow_(t_plug *plug,void (* f)(t_plug_mode mode,t_plug *p1,t_plug *p2))
{
	t_brick *brick=plug->brick;

	t_plug *plug_in=&brick->plug_in;
	t_plug *plug_out=&brick->plug_out;

	// IN

	if(plug_in->is_connected)
	{
		t_plug *plug_origin = plug_in->src;

		if(plug_origin->open_out)
		{
			t_plug *src_plug=plug_get_src(plug);

			if(plug_in->flow_in)
			{
				f(mode_in,plug,src_plug);
			}
		}
	}
	else
	{
		f(mode_in,plug,NULL);
	}

	// OUT

	if(plug_out->is_connected)
	{
		t_plug *plug_origin = plug_out->dst;

		if(plug_origin->open_in)
		{
			t_plug *src_plug=plug_get_dst(plug);

			if(plug_out->flow_out)
			{
				f(mode_out,plug,src_plug);
			}
		}
	}
	else
	{
		f(mode_out,plug,NULL);
	}
}

// FLOAT

void __cls_plug_flow_float(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		float *data=plug->data;

		float increment=plug->brick->var.increment;

		switch(src_type)
		{
			case dt_int:

				*data=(float)drf_int(src_plug->data);
				break;

			case dt_float:

				*data=drf_float(src_plug->data);
				break;
			default:
				plug_warning(plug,src_plug);
				break;
		}

		if(increment) *data=*data * increment;  
	}

}

void cls_plug_flow_float(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_float);
}

// INT

void __cls_plug_flow_int(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		int *data=plug->data;

		float increment=plug->brick->var.increment;

		switch(src_type)
		{
			case dt_int:

				*data=drf_int(src_plug->data);
				break;

			case dt_float:

				*data=(int)drf_float(src_plug->data);
				break;
			case dt_lst:
				break;

			default:
				plug_warning(plug,src_plug);
				break;
		}

		if(increment) *data=*data * increment;  
	}
}


void cls_plug_flow_int(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_int);
}

// LST

void __cls_plug_flow_lst(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		t_brick *src_brick=src_plug->brick;

		switch(src_type)
		{
			// + LST
			case dt_lst:
				//copy pointer
				plug->data=src_plug->data;
				break;

			// +INT (REWIND)
			case dt_int:

				if(is(src_brick->name,"rewind"))
				{
					int *state = src_plug->data;

					if(*state == 1)
					{
						t_lst *lst=(t_lst *)plug->data;
						lst->current=lst->first;
					}
				}

				break;
				
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_lst(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_lst);
}

// VLST

void __cls_plug_flow_vlst(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		switch(src_type)
		{
			// + LST
			case dt_vlst:
				//copy pointer
				plug->data=src_plug->data;
				break;
				
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_vlst(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_vlst);
}

// VERTEX

void __cls_plug_flow_vertex(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		t_mesh *mesh;

		switch(src_type)
		{
			// + VLST
			case dt_vlst:

				mesh=plug->data;

				mesh->vertex=src_plug->data;
				mesh->state.need_update=1;

				break;
				
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_vertex(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_vertex);
}

// MESH

void __cls_plug_flow_mesh(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		switch(src_type)
		{
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_mesh(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_mesh);
}

// OBJECT

void __cls_plug_flow_object(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		// node (pointer)
		t_node *node=plug->data;
		// object
		t_object *object=node->data;

		t_mesh *mesh;

		t_camera *camera_src;
		t_camera *camera_dst;

		switch(src_type)
		{
			// + MESH
			case dt_mesh:

				if(is(object->type,"mesh"))
				{
					mesh=src_plug->data;
					object->mesh=mesh;
				}

				break;

			// + CAMERA
			case dt_camera:

				if(is(object->type,"camera"))
				{
					if(src_plug->open_out)
					{
						camera_src=src_plug->data;
						camera_dst=object->data;
						camera_copy(camera_dst,camera_src);
					}
				}

				break;

			case dt_lst:
				break;
				
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_object(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_object);
}

// POINTER

void __cls_plug_flow_pointer(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		t_object *object;

		switch(src_type)
		{
			// + MESH
			case dt_mesh:

				object=plug->data;
				object->mesh=src_plug->data;

				break;
				
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_pointer(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_pointer);
}

// FOR 

int for_init=0;

void __cls_plug_flow_operator_for(t_plug_mode mode,t_plug *plug,t_plug *plug_src)
{
	t_context *C =ctx_get();

	t_plug *plug_in = plug->src;
	t_brick *brick = plug->brick;

	// get bricks
	t_block *block = brick->block;
	t_brick *brick_indice = block_brick_get(block,"indice");
	t_brick *brick_vector = block_brick_get(block,"vector");

	t_plug *plug_indice = &brick_indice->plug_intern;
	t_plug *plug_vector = &brick_vector->plug_intern;
	t_plug *plug_vector_in = &brick_vector->plug_in;

	int *data_indice = plug_indice->data;
	t_vector *vector = plug_vector->data;

	// IN
	if(mode == mode_in)
	{
		// reset vector
		vector->data = NULL;

		// if for connected
		if(plug_in->is_connected )
		{
			// get src
			t_plug *src_plug = plug_get_src(plug);
			t_data_type src_type = src_plug->data_type;

			t_vlst *vlst=NULL;

			switch(src_type)
			{
				// + VLST
				case dt_vlst:

					// SET VLST
					vlst = src_plug->data;

					// SET vector
					if(vlst)
					{
						vector->data = vlst->data;
						vector->type = vlst->data_type;
						vector->length = vlst->length;
					}

					break;
					
				default:
					plug_warning(plug,src_plug);
					break;
			}

			// SET VECTOR
			if(plug_vector_in->is_connected)
			{
				if(vlst)
				{
					// set vector, open for fisrt loop
					//if(!plug->is_eval)
					if(!for_init)
					//if(1==2)
					{

						if(C->ui->show_step) term_log(":FOR loop (INIT) %d",brick->counter);
						plug->is_eval = 1;
						for_init = 1;

						// get pointer
						float *ptr = vlst->data;

						// set pointer
						vector->data = ptr;

						// set indice
						*data_indice=0;

						t_plug *plug_vector_src = plug_vector_in->src;
						t_brick *brick_vector_src = plug_vector_src->brick;

						brick_vector_src->cls->trigger(brick_vector_src);

						


							t_lst *BRICKS = ctx_links_lst_get();
							lst_add(BRICKS,brick,"for");
					}
					else
					{
						if(brick->counter < vlst->count)
						{
							if(C->ui->show_step) term_log(":FOR loop %d",brick->counter);
							// get pointer
							float *ptr = vlst->data;

							// set pointer
							vector->data = ptr + (vlst->length * brick->counter);

							// set indice
							*data_indice=brick->counter;

							// get branch (all bricks)
							t_lst *lst=lst_new("lst");
							block_branch_get(0,lst,block);

							// reset states
							ctx_links_reset(C,lst);

							// add to loop
							t_lst *BRICKS = ctx_links_lst_get();

							t_link *l;
							t_brick *b;
							for(l=lst->first;l;l=l->next)
							{

								b = l->data;
								lst_add(BRICKS,l->data,b->name);
							}

							// free
							lst_free(lst);


							// counter ++
							brick->counter++;

						}
						else
						{
							//XXX
							vector->data = NULL;
							// reset counter
							brick->counter = 0;

							for_init=0;
						}
					}
				}
			}
			else
			{
				*data_indice = 0;

			}
		}
	}
}

// GET

void __cls_plug_flow_operator_get(t_plug_mode mode,t_plug *plug,t_plug *plug_src)
{
	t_brick *brick = plug->brick;

	t_block *block = brick->block;

	t_brick *brick_indice = block_brick_get(block,"indice");
	t_brick *brick_result = block_brick_get(block,"result");

	t_plug *plug_result = &brick_result->plug_intern;
	t_plug *plug_indice = &brick_indice->plug_intern;

	t_lst *lst;
	t_vlst *vlst;

	// IN
	if(plug_src && mode == mode_in)
	{
		switch(plug_src->data_type)
		{
			// + LST
			case (dt_lst):
				
				// get lst
				lst = plug_src->data;

				// if lst
				if(lst)
				{
					if(!lst->current)
					{
						// set first
						if(lst->first)
						{
							lst->current = lst->first;
						}
					}
					else
					{
						t_link *link = lst->current;

						if(link->next)
						{
							lst->current = link->next;
						}
						else
						{
							//
						}

						//XXX change type

						plug_result->data_type=dt_camera;
						t_camera *camera=link->data;
						plug_result->data=camera;

					}
				}

				break;

			// +VLST

			case(dt_vlst):
				
				// get vlst
				vlst = plug_src->data;

				if(vlst)
				{
					t_plug _plug;
					_plug.data_type = dt_vector;

					// change type to vector
					if(brick_result->plug_intern.data_type != dt_vector)
						brick_type_change(brick_result,&_plug);

					// get indice
					int i = drf_int(plug_indice->data);

					// if < indice
					if(i < vlst->count)
					{
						t_vector *vector = plug_result->data;

						// get pointer
						float *ptr = vlst->data;
						// do pointer arithmetic
						vector->data = ptr + (vlst->length * i);

					}
					else
					{
						t_vector *vector = plug_result->data;
						set_int(plug_indice->data,vlst->count-1);
						float *ptr = vlst->data;
						vector->data = ptr + (vlst->length * (vlst->count-1));
					}

					t_plug *plug_result_out = &brick_result->plug_out;

					//XXX
					// open vector
					if(plug_result_out->is_connected)
					{
						t_plug *plug_vector = plug_get_dst(plug_result);

						if(plug_vector->data_type == dt_vector)
						{
							t_brick *brick_vector = plug_vector->brick;
							t_plug *plug_vector_in = &brick_vector->plug_in;

							// FLOW IN
							plug_vector_in->flow_in=1;

							t_block *block_vector = brick_vector->block;

							t_brick *brick_x = block_brick_get(block_vector,"x");
							t_brick *brick_y = block_brick_get(block_vector,"y");
							t_brick *brick_z = block_brick_get(block_vector,"z");

							brick_x->state.draw_value=1;
							brick_y->state.draw_value=1;
							brick_z->state.draw_value=1;

							brick_x->action = op_slider;
							brick_y->action = op_slider;
							brick_z->action = op_slider;
						}
					}
				}

				break;
				

			default:
				printf(">> %s\n",data_name_get(plug_src->data_type));
				break;
		}
	}
}

// OPERATOR


void cls_plug_flow_operator(t_plug *plug)
{
	switch(plug->operator_type)
	{
		case(ot_null):
			break;
		case(ot_for):
			_cls_flow_(plug,__cls_plug_flow_operator_for);
			break;
		case(ot_get):

			_cls_flow_(plug,__cls_plug_flow_operator_get);
			break;
		default:
			break;
	}
}

// VECTOR

void __cls_plug_flow_vector(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	t_brick *brick = plug->brick;
	t_block *block = brick->block;

	// get X Y Z bricks

	t_brick *brick_x = block_brick_get(block,"x");
	t_brick *brick_y;
	t_brick *brick_z;


	t_plug *plug_x;
	t_plug *plug_y;
	t_plug *plug_z;

	t_plug *plug_x_in;
	t_plug *plug_y_in;
	t_plug *plug_z_in;

	// Not All Vectors have x y z in block
	// Reset once
	if(brick_x && mode == mode_in)
	{
		brick_y = block_brick_get(block,"y");
		brick_z = block_brick_get(block,"z");


		plug_x = &brick_x->plug_intern;
		plug_y = &brick_y->plug_intern;
		plug_z = &brick_z->plug_intern;

		plug_x_in = &brick_x->plug_in;
		plug_y_in = &brick_y->plug_in;
		plug_z_in = &brick_z->plug_in;

		// CLOSE X Y Z
		plug_x_in->flow_in = 0;
		plug_y_in->flow_in = 0;
		plug_z_in->flow_in = 0;

		// RESET X Y Z
		plug_x->data = NULL;
		plug_y->data = NULL;
		plug_z->data = NULL;

		brick_x->state.draw_value = 0;
		brick_y->state.draw_value = 0;
		brick_z->state.draw_value = 0;
	}

	t_vector *vector;

	float *vector_data;

	if(src_plug)
	{
		t_data_type type = src_plug->data_type;

		switch(type)
		{
			// + VECTOR
			case dt_vector:

				vector = src_plug->data;
				vector_data = vector->data;


				//-- wait for initialisation from for loop
				if(vector_data)
				{
					t_context *C = ctx_get();
					if(C->ui->show_step) printf(":VECTOR is connected\n");
					plug_x->data = vector_data;
					plug_y->data = vector_data+1;
					plug_z->data = vector_data+2;

					// open x y z
					plug_x_in->flow_in = 1;
					plug_y_in->flow_in = 1;
					plug_z_in->flow_in = 1;

					brick_x->state.draw_value = 1;
					brick_y->state.draw_value = 1;
					brick_z->state.draw_value = 1;

					/*
					brick_x->cls->trigger(brick_x);
					brick_y->cls->trigger(brick_y);
					brick_z->cls->trigger(brick_z);
					*/
				}
				else
				{
					t_context *C = ctx_get();
					if(C->ui->show_step) printf(":VECTOR is NOT connected\n");
				}

				break;
				
			default:
				break;
		}
	}
	else
	{
	}
}

void cls_plug_flow_vector(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_vector);
}

// TRIGGER

void __cls_plug_flow_trigger(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
	t_brick *brick=plug->brick;

	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		int data;

		switch(src_type)
		{
			case(dt_int):

				data=drf_int(src_plug->data);
				if(data == 1)
					brick->action(brick);
				break;
				
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
	}
}

void cls_plug_flow_trigger(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_trigger);
}


// GENERIC

void cls_plug_flow_generic(t_plug *plug)
{
	t_plug *plug_in=plug->src;

	if(plug_in->is_connected)
	{
		t_plug *src_plug=plug_get_src(plug);
		t_data_type src_type=src_plug->data_type;

		switch(src_type)
		{
			default:
				plug_warning(plug,src_plug);
				break;
		}

	}

}

// CAMERA

void __cls_plug_flow_camera(t_plug_mode mode,t_plug *plug,t_plug *src_plug)
{
	if(src_plug)
	{
		t_data_type src_type=src_plug->data_type;

		t_camera *camera;

		switch(src_type)
		{
			case(dt_camera):
				camera = src_plug->data;
				plug->data = camera;

				break;
			default:
				plug_warning(plug,src_plug);
				break;
		}
	}
}

void cls_plug_flow_camera(t_plug *plug)
{
	_cls_flow_(plug,__cls_plug_flow_camera);
}

// CLS

// FLOAT

t_plug_class plug_float ={
	.cls_type="plug",
	.type=dt_float,
	.make=cls_plug_make_float,
	.flow=cls_plug_flow_float,
};

// INT

t_plug_class plug_int ={
	.cls_type="plug",
	.type=dt_int,
	.make=cls_plug_make_int,
	.flow=cls_plug_flow_int,
};

// STRING

t_plug_class plug_string ={
	.cls_type="plug",
	.type=dt_string,
	.make=cls_plug_make_string,
	.flow=cls_plug_flow_generic, //XXX
};

// POINTER

t_plug_class plug_pointer ={
	.cls_type="plug",
	.type=dt_pointer,
	.make=cls_plug_make_string,
	.flow=cls_plug_flow_pointer, //XXX
};

// MESH

t_plug_class plug_mesh ={
	.cls_type="plug",
	.type=dt_mesh,
	.make=cls_plug_make_mesh,
	.flow=cls_plug_flow_mesh, 
};

// VERTEX

t_plug_class plug_vertex ={
	.cls_type="plug",
	.type=dt_vertex,
	.make=cls_plug_make_vertex,
	.flow=cls_plug_flow_vertex, 
};

// FACE

t_plug_class plug_face ={
	.cls_type="plug",
	.type=dt_face,
	.make=cls_plug_make_face,
	.flow=cls_plug_flow_generic, //XXX
};

// VLST

t_plug_class plug_vlst ={
	.cls_type="plug",
	.type=dt_vlst,
	.make=cls_plug_make_vlst,
	.flow=cls_plug_flow_vlst, 
};

// LST

t_plug_class plug_lst ={
	.cls_type="plug",
	.type=dt_lst,
	.make=cls_plug_make_lst,
	.flow=cls_plug_flow_lst, 
};

// CAMERA

t_plug_class plug_camera ={
	.cls_type="plug",
	.type=dt_camera,
	.make=cls_plug_make_camera,
	.flow=cls_plug_flow_camera, 
};

// CHAR

t_plug_class plug_char ={
	.cls_type="plug",
	.type=dt_char,
	.make=cls_plug_make_char,
	.flow=cls_plug_flow_generic, //XXX
};

// OBJECT

t_plug_class plug_object ={
	.cls_type="plug",
	.type=dt_object,
	.make=cls_plug_make_object,
	.flow=cls_plug_flow_object, 
};

// SELECTOR

t_plug_class plug_selector ={
	.cls_type="plug",
	.type=dt_selector,
	.make=cls_plug_make_selector,
	.flow=cls_plug_flow_object, 
};

// TRIGGER

t_plug_class plug_trigger ={
	.cls_type="plug",
	.type=dt_trigger,
	.make=cls_plug_make_trigger,
	.flow=cls_plug_flow_trigger, 
};

// OPERATOR

t_plug_class plug_operator ={
	.cls_type="plug",
	.type=dt_operator,
	.make=cls_plug_make_operator,
	.flow=cls_plug_flow_operator, 
};

// VECTOR

t_plug_class plug_vector ={
	.cls_type="vector",
	.type=dt_vector,
	.make=cls_plug_make_vector,
	.flow=cls_plug_flow_vector,  
};


void cls_plug_make_float(t_plug *plug)
{
	plug->cls=&plug_float;
}

void cls_plug_make_int(t_plug *plug)
{
	plug->cls=&plug_int;
}

void cls_plug_make_string(t_plug *plug)
{
	plug->cls=&plug_int;
}

void cls_plug_make_pointer(t_plug *plug)
{
	plug->cls=&plug_pointer;
}


void cls_plug_make_mesh(t_plug *plug)
{
	plug->cls=&plug_mesh;
}

void cls_plug_make_vertex(t_plug *plug)
{
	plug->cls=&plug_vertex;
}

void cls_plug_make_face(t_plug *plug)
{
	plug->cls=&plug_face;
}

void cls_plug_make_vlst(t_plug *plug)
{
	plug->cls=&plug_vlst;
}

void cls_plug_make_lst(t_plug *plug)
{
	plug->cls=&plug_lst;
}

void cls_plug_make_camera(t_plug *plug)
{
	plug->cls=&plug_camera;
}

void cls_plug_make_char(t_plug *plug)
{
	plug->cls=&plug_char;
}

void cls_plug_make_object(t_plug *plug)
{
	plug->cls=&plug_object;
}

void cls_plug_make_selector(t_plug *plug)
{
	plug->cls=&plug_selector;
}

void cls_plug_make_trigger(t_plug *plug)
{
	plug->cls=&plug_trigger;
}

void cls_plug_make_operator(t_plug *plug)
{
	plug->cls=&plug_operator;
}

void cls_plug_make_vector(t_plug *plug)
{
	plug->cls=&plug_vector;
}

t_plug_class *plugs[] = {
	&plug_float,
	&plug_int,
	&plug_char,
	&plug_string,
	&plug_mesh,
	&plug_vertex,
	&plug_face,
	&plug_vlst,
	&plug_lst,
	&plug_camera,
	&plug_object,
	&plug_selector,
	&plug_trigger,
	&plug_operator,
	&plug_vector,
	};


// cls

void plug_cls_init(t_plug *plug)
{
	int i;
	int n = sizeof(plugs)/sizeof(plugs[0]);
	int found=0;

	for(i=0;i<n;i++)
	{
		if(plug->data_type==plugs[i]->type)
		{
			plugs[i]->make(plug);
			found=1;
			break;
		}
	}

	if(!found)
	{
		char *type=data_name_get(plug->data_type);
		printf("[ERROR:plug_cls_init] Unknown plug type [%s]\n",type);
	}
}

