/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "op.h"

void lst_show_generic(t_lst *lst)
{
	printf("lst_show %s\n",lst->name);
	printf("~\n");
	t_link *l;
	for(l=lst->first;l;l=l->next)
	{
		t_generic *g = (t_generic *)l->data;
		printf("~ %s\n",g->name);
	}
	printf("~\n");
}

t_node *lst_get_node(t_lst *lst,int id)
{
	t_link *l;
	t_node *node;
	assert(lst);

	for(l=lst->first;l;l=l->next)
	{
		node=l->data;
		if(node->id==id)
		{
			return node;
		}
	}

	return NULL;
}

void *lst_get(t_lst *lst,char *name)
{
	t_link *l;
	t_generic *g;

	if(lst->first)
	{
		for(l=lst->first;l;l=l->next)
		{
			g=l->data;
			if(is(g->name,name))
			{
				return l->data;
			}
		}

		return NULL;
	}
	else
	{
		return NULL;
	}
}

// remove node
int lst_remove_node(t_lst *lst,t_node *node)
{
	t_link *link;
	if(lst->first)
	{
		for(link=lst->first;link;link=link->next)
		{
			t_node *n = link->data;
			//match node
			if(node->id==n->id)
			{
				lst_link_remove(lst,link); 
				return 1;
			}
		}

		// no match
		return 0;
	}
	return 0;
}

void lst_node_delete_all(t_lst *lst)
{
	t_link *l=lst->first;
	t_node *n;
	if(l)
	{
		for(;l;l=l->next)
		{
			n=l->data;
			n->cls->del(n);
		}
	}	

	lst_cleanup(lst);
}

// FIND NODE BY ID

//void *lst_get_by_id(t_lst *lst,int id)
t_link *lst_get_by_id(t_lst *lst,int id)
{
	t_link *l;
	t_generic *g;

	if(lst->first)
	{
		for(l=lst->first;l;l=l->next)
		{
			g=l->data;
			if(g->id==id)
			{
				return l;
			}
		}

		return NULL;
	}
	else
	{
		return NULL;
	}
}

int lst_remove_by_id(t_lst *lst,int id)
{
	t_link *link=lst_get_by_id(lst,id);
	if(link)
	{
		lst_link_delete(lst,link);
		return 1;
	}
	else
	{
		printf("[ERROR lst_remove_by_id] Can't get link\n");
		return 0;
	}
}

// FIND NODE BY NAME

t_node *lst_find_node_by_name(t_lst *lst,const char *name)
{
	t_link *link;
	t_node *node;
	t_generic *generic;

	if(lst->first)
	{
		for(link=lst->first;link;link=link->next)
		{
			node=link->data;
			generic=node->data;

			if(is(generic->name,name))
			{
				return node;
			}
		}

		printf("[ERROR lst_find_node by name] Can't find node:%s\n",name);
		return NULL;
	}
	else
	{
		printf("[ERROR lst_find_node by name] Empty list\n");
		return NULL;
	}
}

t_link *list_add(t_lst *lst,void *data)
{
	t_context *C=ctx_get();

	t_generic *g = (t_generic *)data;

	//t_node *node=scene_add(C->scene,"link",g->name);
	t_node *node=scene_add(C->scene,nt_link,g->name);
	t_link *link=node->data;
	link->data=data;
	lst_push_back(lst,link);

	return link;
}

void list_remove(t_lst *lst);

void _link_free(t_link *link)
{
	free(link);
}


// free links && list 
// but not the data it points to
void list_free(t_lst *lst)
{
	t_context *C=ctx_get();
	t_scene *sc=C->scene;

	t_link *l;

	for(l=lst->first;l;l=l->next)
	{
		scene_struct_delete(sc,l);
	}

	free(lst);
}

void list_cleanup(t_lst *lst)
{
	t_context *C=ctx_get();
	t_scene *sc=C->scene;

	t_link *l;

	for(l=lst->first;l;l=l->next)
	{
		scene_struct_delete(sc,l);
	}

}

void list_remove_by_ptr(t_lst *lst,void *ptr)
{
	t_context *C=ctx_get();
	t_scene *sc=C->scene;

	t_link *l;
	t_link *link=NULL;

	for(l=lst->first;l;l=l->next)
	{
		if(l->data==ptr)
		{
			link=l;
			break;
		}
	}

	if(link)
	{
		lst_link_remove(lst,link);
		scene_struct_delete(sc,link);
	}
	else
	{
		printf("[ERROR lst_remove_by_ptr] Can't find link\n");
	}
}

t_link *list_add_global(t_lst *lst,void *data)
{
	t_context *C=ctx_get();

	//t_node *node=scene_add(C->scene,"link","glink");
	t_node *node=scene_add(C->scene,nt_link,"glink");
	t_link *link=node->data;
	link->data=data;
	lst_push_back(lst,link);

	return link;
}

t_lst *lst_rebind(t_scene *sc,void *ptr)
{
	t_lst *lst=(t_lst *)ptr;

	check_init("LIST",lst->name);

	rebind(sc,"list","first",(void **)&lst->first);
	rebind(sc,"list","last",(void **)&lst->last);
	rebind(sc,"list","current",(void **)&lst->current);

	check_check("LIST",lst->name);

	return lst;
}

t_link *link_rebind(t_scene *sc,void *ptr)
{
	t_link *link=(t_link *)ptr;

	check_init("LINK",link->name);

	rebind(sc,"link","next",(void **)&link->next);
	rebind(sc,"link","prev",(void **)&link->prev);
	rebind(sc,"link",link->name,(void **)&link->data);

	link->parent=NULL;
	link->child=NULL;

	check_check("LINK",link->name);

	return link;
}




