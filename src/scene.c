/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "scene.h"

// STATIC
t_scene *SCENE=NULL;

// tmp idcol
int COLOR[3];

/***	 GET	* **/

// get scene

t_scene *scene_get(void)
{
	return SCENE;
}

// LOG

void scene_log_loop_var(t_lst *lst)
{
	t_link *l;
	t_node *n;

	log((LOG_SCENE_NODES,"lst:%s\n",lst->name));
	for(l=lst->first;l;l=l->next)
	{
		n=l->data;
		log((LOG_SCENE_NODES,"node id:%d cls:%s\n",n->id,node_name_get(n->type)));
	}
}

void scene_log_loop(t_lst *lst)
{
	t_link *l;
	t_node *n;
	t_generic *g;

	log((LOG_SCENE_NODES,"lst:%s\n",lst->name));
	for(l=lst->first;l;l=l->next)
	{
		n=l->data;
		g=(t_generic *)n->data;
		log((LOG_SCENE_NODES,"node id:%d\tcls:%s\tname:%s\n",n->id,node_name_get(n->type),g->name));
	}
}

void scene_log(t_scene *sc)
{
	//scene_log_loop(sc->nodes);
	/*
	scene_log_loop(sc->bricks);
	scene_log_loop(sc->blocks);
	scene_log_loop(sc->textures);
	scene_log_loop(sc->materials);
	scene_log_loop(sc->files);
	scene_log_loop(sc->meshes);
	scene_log_loop(sc->cameras);
	scene_log_loop(sc->lights);
	scene_log_loop(sc->objects);
	scene_log_loop(sc->screens);
	scene_log_loop(sc->lists);
	scene_log_loop(sc->images);
	scene_log_loop(sc->vlst);
	scene_log_loop(sc->dicts);
	scene_log_loop(sc->symbols);
	*/

	scene_log_loop(sc->datas);
	scene_log_loop_var(sc->vars);
}

// get lst

t_lst *scene_lst_get(t_scene *sc,const char *type)
{
	t_lst *lst=NULL;

	if(is(type,"screen"))  lst=sc->screens; 
	else if(is(type,"mesh"))  lst=sc->meshes; 
	else if(is(type,"block"))  lst=sc->blocks; 
	else if(is(type,"brick"))  lst=sc->bricks; 
	else if(is(type,"light"))  lst=sc->lights; 
	else if(is(type,"object"))  lst=sc->objects; 
	else if(is(type,"file"))  lst=sc->files; 
	else if(is(type,"material"))  lst=sc->materials; 
	else if(is(type,"list"))  lst=sc->lists; 
	else if(is(type,"link"))  lst=sc->links; 
	else if(is(type,"texture"))  lst=sc->textures; 
	else if(is(type,"camera"))  lst=sc->cameras; 
	else if(is(type,"dict"))  lst=sc->dicts; 
	else if(is(type,"symbols"))  lst=sc->symbols; 

	if(lst)
	{
		return lst;
	}
	else
	{
		printf("[ERROR scene_node_get] Unknown type:%s\n",type);
		return NULL;
	}
}

t_node *scene_get_data(t_scene *sc,void *ptr)
{
	t_link *l;
	t_node *n;
	t_data *d;

	/*
	printf("scene_get_data %p\n",ptr);
	for(l=sc->datas->first;l;l=l->next)
	{
		n=l->data;
		d=n->data;
		printf("%p\n",d->pointer);
		if(d->pointer==ptr) return n;
	}
	*/

	for(l=sc->datas->first;l;l=l->next)
	{
		n=l->data;
		d=n->data;
		if(d->pointer==ptr) return n;
	}

	printf("[ERROR scene_get_data]\n");
	return NULL;
}

// ptr was malloc'ed, find data var with ptr address

t_node *scene_get_var(t_scene *sc,void *ptr)
{
	t_link *l;
	t_node *n;

	/*
	printf("scene_get_var %p\n",ptr);

	for(l=sc->vars->first;l;l=l->next)
	{
		n=l->data;
		printf("%p\n",n->id_ptr);
	}
	*/

	for(l=sc->vars->first;l;l=l->next)
	{
		n=l->data;
		if(n->id_ptr==ptr) return n;
	}

	printf("[ERROR scene_get_var]\n");
	return NULL;
}


// get node by id

t_node *scene_node_get_by_id(t_scene *sc,const char *type,int id)
{
	t_lst *lst=(t_lst *)scene_lst_get(sc,type);

	if(lst)
	{
		t_link *l;
		t_node *n;
		for(l=lst->first;l;l=l->next)
		{
			n=l->data;
			if(n->id==id)
			{
				return n;
			}
		}
		
		printf("[ERROR scene_node_get] Node not found:%s\n",type);
		return NULL;
	}
	else
	{
		printf("[ERROR scene_node_get] Unknown type:%s\n",type);
		return NULL;
	}
}

t_node *scene_node_get_by_id_global(t_scene *sc,int id)
{
	t_link *l;
	t_node *n;

	t_node *node=NULL;

	for(l=sc->nodes->first;l;l=l->next)
	{
		n=l->data;
		if(n->id==id)
		{
			node=n;
			break;
		}
	}

	if(node)
	{
		return node;
	}
	else
	{
		printf("[ERROR scene_node_get_by_id_global] Node not found\n");
		return NULL;
	}
}

// get node by old id

t_node *scene_node_get_by_id_old(t_scene *sc,const char *type,int id)
{
	t_lst *lst=(t_lst *)scene_lst_get(sc,type);

	if(lst)
	{
		t_link *l;
		t_node *n;
		for(l=lst->first;l;l=l->next)
		{
			n=l->data;
			if(n->id_old==id)
			{
				return n;
			}
		}
		
		printf("[ERROR scene_node_get_by_id_OLD] Node not found\n");
		return NULL;
	}
	else
	{
		printf("[ERROR scene_node_get] Unknown type:%s\n",type);
		return NULL;
	}
}


// get node by type/name

t_node *scene_node_get(t_scene *sc,const char *type,const char *name)
{
	t_lst *lst=scene_lst_get(sc,type);

	if(lst)
	{
		return lst_find_node_by_name(lst,name);
	}
	else
	{
		printf("[ERROR scene_node_get] Unknown type:%s\n",type);
		return NULL;
	}
}




/***	 COLOR	***/




// switch tmp mode

void scene_color_switch_mode(t_scene *sc)
{
	if(sc->use_tmp_colors)
	{
		sc->use_tmp_colors=0;
	}
	else
	{
		sc->use_tmp_colors=1;
		bzero(COLOR,3);
	}
}

// set single id col

void scene_set_color(t_scene *sc)	
{
	int step = sc->step;
	int *color;

	if(sc->use_tmp_colors)
	{
		color=COLOR;
	}
	else
	{
		color=sc->color;
	}

	while(step>0)
	{
		color[0]++;

	   	if(color[0] > 255)
		{
			color[0] = 0;
			color[1]++;
			if(color[1] > 255)
			{
			     color[1] = 0;
			     color[2]++;
			}
		}
		step--;
	}
}

// get id col

void scene_color_get(t_scene *sc,int *color)
{
	int *col;

	if(sc->use_tmp_colors)
	{
		col=COLOR;
	}
	else
	{
		col=sc->color;
	}

	vseti(color,col[0],col[1],col[2]);

	// increment
	scene_set_color(sc);
}


/***	 ID	***/


int scene_id_get(t_scene *sc)
{
	sc->id++;
	return sc->id;
}




/***	 FREE	***/




// delete by id

int scene_delete_by_id(t_scene *sc,const char *type,int id)
{
	printf("scene_delete_by_id\n");
	t_node *node = scene_node_get_by_id(sc,type,id);

	if(node)
	{
		scene_node_delete(sc,node);
		return 1;
	}
	else
	{
		return 0;
	}
}

void scene_mem_remove(t_scene *sc,t_node *node)
{
	mem_remove(node->id_chunk);
	mem_remove(node->id_chunk_self);
}

void scene_light_free(t_scene *sc,t_node *node) {}
void scene_screen_free(t_scene *sc,t_node *node) {}
void scene_file_free(t_scene *sc,t_node *node) {}
void scene_image_free(t_scene *sc,t_node *node) {}
void scene_material_free(t_scene *sc,t_node *node) {}
void scene_option_free(t_scene *sc,t_node *node) {}


void scene_generic_free(t_scene *sc,t_node *node)
{
}
void scene_texture_free(t_scene *sc,t_node *node)
{
	texture_free(node->data);
	free(node);
}

void scene_vector_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	vector_free(node->data);
	free(node);
}

void scene_list_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	list_free(node->data);
	free(node);
}

void scene_camera_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	free(node->data);
	free(node);
}

void scene_mesh_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	t_mesh *mesh = node->data;
	mesh_free(mesh);
	free(node);
}

void scene_brick_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	brick_free(node->data);
	free(node);
}

void scene_block_free(t_scene *sc,t_node *node)
{
	t_block *block=node->data;
	scene_mem_remove(sc,node);
	block_free(block);
	free(node);
}

void scene_object_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	object_free(node->data);
	free(node);
}

void scene_data_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	data_free(node->data);
	free(node);
}

void scene_vlst_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	vlst_free(node->data); //free vector
	free(node);
}

void scene_link_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	_link_free(node->data); //XXX void
	free(node);
}

void scene_dict_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	dict_free(node->data);
	free(node);
}

void scene_symbol_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	symbol_free(node->data);
	free(node);
}

void scene_var_free(t_scene *sc,t_node *node)
{
	scene_mem_remove(sc,node);
	// var->data freed elsewhere
	free(node);
}

void scene_node_free(t_scene *sc,t_node *node)
{
	//printf("node free %d\n",node->id);
	lst_remove_node(node->cls->lst,node);
	node->cls->free(sc,node);
	lst_remove_node(sc->nodes,node);
}

// cleanup (free stack)

void scene_cleanup(t_scene *sc)
{
	// lock scene
	sc->is_ready=0;
	t_link *link;

	for(link=sc->stack->first;link;link=link->next)
	{
		if(link)
		{
			t_node *node = link->data;
			scene_node_free(sc,node);
			lst_remove_node(sc->stack,node);
		}
	}

	lst_cleanup(sc->stack);

	// unlock scene
	sc->is_ready=1;
}

// delete (add to stack)

int scene_node_delete(t_scene *sc,t_node *node)
{
	if(sc->is_ready)
	{
		lst_add(sc->stack,node,node_name_get(node->type));
		return 1;
	}
	else
	{
		return 0;
	}
}

void scene_struct_delete(t_scene *sc,void *ptr)
{
	//XXX!!! var is not generic
	t_generic *g=(t_generic *)ptr;

	t_link *l;
	t_node *n;
	t_node *node=NULL;

	for(l=sc->nodes->first;l;l=l->next)
	{
		n=l->data;
		if(n->type!=nt_var)
		{
			if(n->id==g->id)
			{
				node=n;
				break;
			}
		}
	}

	if(node)
	{
		scene_node_free(sc,node);
	}
	else
	{
		if(!ptr)
		{
			printf("[ERROR scene_struct_delete] pointer is void\n");
		}
		else
		{
			printf("[ERROR scene_struct_delete] Can't find node of [%s]\n",g->name);
		}
	}
}


// free

void scene_free(t_scene *sc)
{
}


/***	 LOAD	***/



// get new id's for app_node
void scene_data_load(t_scene *sc)
{
	t_link *l;
	t_node *node;

	for(l=sc->datas->first;l;l=l->next)
	{
		node=l->data;

		if(node->type==nt_data)
		{	
			t_data *data=(t_data *)node->data;

			if(is(data->type,"app_node"))
			{
				t_node *old=scene_node_get_by_id_old(sc,data->target,data->id_node);

				printf(">> %s\n",data->name);

				if(old)
				{
					//set new id
					data->id_node=old->id;
				}
				else
				{
					printf("[ERROR scene_data_load]\n");
				}
			}
		}
	}
}


void scene_node_load(t_scene *sc,t_node *node)
{
	log((LOG_SCENE,"scene_node_load\n"));

	// INIT NODE && DATA
	node->cls->init(node);


	if(node->type==nt_var)
	{
		lst_add(node->cls->lst,node,node_name_get(node->type));
		lst_add(sc->nodes,node,"var");
	}
	else
	{

		t_generic *g=(t_generic *)node->data;
		lst_add(sc->nodes,node,g->name);

		// ADD TO GLOBAL LIST
		if(is(g->name,"global"))
		{
			log((LOG_SCENE,"loading global\n"));

			t_lst *lst=node->data;


			// get global list
			t_node *node_global=scene_node_get(sc,"list","global");
			t_lst *list=node_global->data;

			// store links
			sc->store=1;

			t_link *l;
			for(l=lst->first;l;l=l->next)
			{
				log((LOG_SCENE,"add %s\n",l->name));

				// ADD TO GLOBAL LIST
				list_add_global(list,l->data);
			}

			sc->store=0;
		}

		// ADD TO LIST
		else
		{
			if(node->cls->lst)
			{
				lst_add(node->cls->lst,node,node_name_get(node->type));
			}
		}
	}
}




/***	ADD	***/



// node

t_node *scene_add_node(t_scene *sc,t_node_type type,const char *name)
{
	// new node
	t_node *node = node_new(type);

	// make node (init cls,size,lst)
	node_init(node,type);
	//node->cls->init(node);

	// build data (allocate struct data (!var), add node->id=g->id (!var))
	node->cls->build(node,name);

	// add to local list
	if(node->cls->lst) lst_add(node->cls->lst,node,name); 

	// add to nodes list
	//printf("scene add node:%s %d\n",name,node->id);
	lst_add(sc->nodes,node,name);

	// store data
	if(sc->store)
	{
		if(type==nt_var)
		{
			// stored by scene_add_data_var
		}
		else
		{
			// store node && get chunk indice
			node->id_chunk_self=mem_store(ct_node,type,sizeof(t_node),1,node);

			// store data && get chunk indice
			node->id_chunk=mem_store(ct_data,node->type,node->cls->size,1,node->data);

			// copy chunk indice to generic
			t_generic *g=(t_generic *)node->data;
			g->id_chunk=node->id_chunk;

		}
	}

	return node;
}

// add regular struct

t_node *scene_add(t_scene *sc,t_node_type type,const char *name)
{
	log((LOG_SCENE,"scene_add %s %s \n",node_name_get(type),name));

	t_node *node=scene_add_node(sc,type,name);

	return node;
}

// data register (app_data)(app|func)

t_node *scene_add_data(t_scene *sc,const char *type,const char *target,const char *name,void *ptr)
{
	log((LOG_SCENE,"scene_add_data %s %s %s\n",type,target,name));

	t_node *node=scene_add_node(sc,nt_data,name);
	t_data *data=node->data;

	set_name(data->type,type);
	set_name(data->target,target);

	data->pointer=ptr;

	return node;
}

// data node (app_node)(object|)
// retrieve node's struct by struct's id

t_node *scene_add_data_node(t_scene *sc,const char *type,const char *target,const char *name,void *ptr)
{
	log((LOG_SCENE,"scene_add_node %s %s %s\n",type,target,name));

	t_node *node=scene_add_node(sc,nt_data,name);
	t_data *data=node->data;

	set_name(data->type,type);
	set_name(data->target,target);

	data->pointer=ptr;

	t_node *n=(t_node *)ptr;
	data->id_node=n->id;

	return node;
}

void scene_remove_data_node(t_scene *sc,void *ptr)
{
	t_link *l;
	t_node *n;
	t_node *node=NULL;
	t_data *d;

	for(l=sc->datas->first;l;l=l->next)
	{
		n=l->data;
		d=n->data;

		if(d->pointer==ptr)
		{
			node=n;
			break;
		}
	}

	if(node)
	{
		scene_node_free(sc,node);
	}
	else
	{
		printf("[ERROR scene_remove_data_node] Can't find node\n");
	}

}

// data ref (struct_ref)(object|)(loc_x|)

t_node *scene_add_ref(t_scene *sc,const char *type,const char *target,const char *name,void *ptr,void *ref)
{
	log((LOG_SCENE,"scene_add_ref %s %s %s\n",type,target,name));

	t_node *node=scene_add_node(sc,nt_data,name);
	t_data *data=node->data;

	set_name(data->type,type); 
	set_name(data->target,target);

	data->pointer=ptr;
	data->ref=ref;

	return node;
}

void scene_remove_ref(t_scene *sc,void *ptr)
{
	scene_remove_data_node(sc,ptr);
}

// data var (name)(name)(size)(ptr)

void scene_add_data_var(t_scene *sc,const char *name,const char *name_var,int size,void *ptr)
{
	log((LOG_SCENE,"scene_add_data_var %s %s %d\n",name,name_var,size));

	// data 
	t_node *node_data=scene_add_data(sc,"dynamic","void",name,ptr);
	t_data *data=node_data->data;

	// store size
	data->size=size;

	// VAR
	t_node *node_var=scene_add(sc,nt_var,"var");
	node_var->data=ptr;
	node_var->id_ptr=ptr;

	// store 
	node_var->id_chunk_self=mem_store(ct_node,nt_var,sizeof(t_node),1,node_var);
	node_var->id_chunk=mem_store(ct_data,nt_var,size,1,ptr);
}




/***	INIT	***/




t_scene *scene_init(void)
{
	// new scene
	t_scene *sc = (t_scene *)malloc(sizeof(t_scene));

	// init
	sc->id=1;
	sc->store=0;
	sc->step=1;
	vset3i(sc->color,0,0,0);
	vset3i(COLOR,0,0,0);
	sc->use_tmp_colors=0;
	sc->selected=NULL;
	sc->is_ready=1;

	// build lists
	sc->nodes=lst_new("nodes");
	sc->blocks=lst_new("blocks");	
	sc->bricks=lst_new("bricks");	
	sc->textures=lst_new("textures");	
	sc->materials=lst_new("materials");
	sc->files=lst_new("files");	
	sc->meshes=lst_new("meshes");	
	sc->cameras=lst_new("cameras");	
	sc->lights=lst_new("lights");	
	sc->objects=lst_new("objects");
	sc->screens=lst_new("screens");
	sc->lists=lst_new("lists");
	sc->links=lst_new("links");
	sc->datas=lst_new("datas");
	sc->vars=lst_new("vars");
	sc->images=lst_new("images");
	sc->stack=lst_new("stack");
	sc->selections=lst_new("selections");
	sc->vlst=lst_new("vlst");
	sc->dicts=lst_new("dicts");
	sc->symbols=lst_new("symbols");
	sc->vectors=lst_new("vectors");

	sc->tmp_colors=lst_new("tmp_colors");

	// global
	sc->global=NULL;

	// load file
	sc->tmp_node=NULL;
	sc->tmp_data=NULL;
	
	// store localy
	SCENE=sc;

	return sc;
}
