/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "op.h"

/***		VLST		***/


void __vlst_update_data(t_vlst *vlst,t_vlst *caller);

void exe_vlst(t_dict *args)
{
	t_vlst *vlst = dict_pop_data(args,"vlst");
	t_vlst *caller = dict_pop_data(args,"caller");

	__vlst_update_data(vlst,caller);
}

void vlst_update_data(t_vlst *vlst,t_vlst *caller)
{
	t_action *action = action_new("action");

	action->act = exe_vlst;

	t_node *node_dict = dict_add("args");
	t_dict *dict = node_dict->data;
	action->args = dict;

	dict_symbol_add(action->args,"vlst",dt_null,vlst);
	dict_symbol_add(action->args,"caller",dt_null,caller);

	exe_add_action(action);
}

void __vlst_update_data(t_vlst *vlst,t_vlst *caller)
{
	t_context *C=ctx_get();

	if(vlst->count != vlst->count_new)
	{
		void *old_ptr=vlst->data;

		// get data,var
		t_node *node_data=scene_get_data(C->scene,old_ptr);
		t_node *node_var=scene_get_var(C->scene,old_ptr);

		// remove data,var
		scene_node_free(C->scene,node_data);
		scene_node_free(C->scene,node_var);

		// set count
		vlst->count=vlst->count_new;

		// realloc
		int new_size=(vlst->size)*(vlst->length)*(vlst->count);
		void *new_ptr=realloc(vlst->data,new_size);

		if(!new_ptr) printf("[ERROR vls_update_data]\n"); 

		// set new ptr
		vlst->data=new_ptr;

		// add new data,var
		C->scene->store=1;

		scene_add_data_var(C->scene,"vlst_data","v_data",new_size,vlst->data);

		C->scene->store=0;

		// shared vlst (vertex, color , ....)
		if(vlst->is_linked)
		{
			t_vlst *link_vlst = vlst->link;
			link_vlst->count_new = vlst->count;

			if(caller)
			{
				// prevent infinite loop
				if(link_vlst->id != caller->id)
				{
					vlst_update_data(link_vlst,vlst);
				}
				
			}
			else
			{
				vlst_update_data(link_vlst,vlst);
			}
		}
	}
}

void vlst_float_add(t_vlst *vlst,t_data_type type,void *data)
{
	int count=vlst->count;
	int length=vlst->length; 
	int i,j;

	float *data_dst=vlst->data;

	for(i=0;i<count;i++)
	{
		for(j=0;j<length;j++)
		{
			switch(type)
			{
				case(dt_int): *data_dst += (float)drf_int(data); break;
				case(dt_float): *data_dst += drf_float(data); break;
				default:printf("? ? !!\n");
			}

			data_dst++;
		}
	}
}


void vlst_add_number(t_vlst *vlst,t_data_type type,void *data)
{
	int is_float=0;
	//int is_int=0;

	switch(vlst->type)
	{
		case(_1f): is_float=1;break;
		case(_2f): is_float=1;break;
		case(_3f): is_float=1;break;
		case(_4f): is_float=1;break;
		default:printf("???????\n");
	}

	if(is_float) vlst_float_add(vlst,type,data);
	else printf("...???\n");

}


void vlst_add_vlst(t_vlst *vlst_dst,t_vlst *vlst_src) 
{
	int i,j;
	int p=0;

	if(vlst_dst->type==vlst_src->type)
	{
		void *data_dst=vlst_dst->data;
		void *data_src=vlst_src->data;

		for(i=0;i<vlst_dst->count;i++)
		{
			for(j=0;j<vlst_dst->length;j++)
			{
				if(vlst_dst->data_type==dt_float)
				{
					float *d=(float *)data_dst;
					float *s=(float *)data_src;

					d[p]+=(s[p]);
				}
				p++;
			}

		}
	}
	else
	{
		printf("[WARNING vlst_add_vlst] vlst are not equal\n");
	}
}

void vlst_show(t_vlst *vlst)
{
	if(vlst)
	{
		int i;
		int count=vlst->count;
		int length=vlst->length;
		void *dat=vlst->data;

		printf("[VLST] %s count:%d length:%d\n",vlst->name,count,length);

		if(vlst->type==_3f)
		{
			float *d=(float *)dat;
			for(i=0;i<count;i++)
			{
				printf("[%d] %f %f %f\n",i,d[0],d[1],d[2]);
				d+=length;
			}
		}
		if(vlst->type==_4f)
		{
			float *d=(float *)dat;
			for(i=0;i<count;i++)
			{
				printf("[%d] %f %f %f %f\n",i,d[0],d[1],d[2],d[3]);
				d+=length;
			}
		}
		if(vlst->type==_3ui)
		{
			unsigned int *d=(unsigned int *)dat;
			for(i=0;i<count;i++)
			{
				printf("[%d] %u %u %u \n",i,d[0],d[1],d[2]);
				d+=length;
			}
		}
		if(vlst->type==_4ui)
		{
			unsigned int *d=(unsigned int *)dat;
			for(i=0;i<count;i++)
			{
				printf("[%d] %u %u %u %u\n",i,d[0],d[1],d[2],d[3]);
				d+=length;
			}
		}

		if(vlst->type==_4i)
		{
			int *d=(int *)dat;
			for(i=0;i<count;i++)
			{
				printf("[%d] %d %d %d %d\n",i,d[0],d[1],d[2],d[3]);
				d+=length;
			}
		}
	}
}

void _loop_int(void *dst,void *src,int count,int size)
{
	int i;
	int *s=(int *)src;
	int *d=(int *)dst;

	int p=0;

	for(i=0;i<count;i++)
	{
		if(size>=1) d[p+0]=s[p+0];
		if(size>=2) d[p+1]=s[p+1];
		if(size>=3) d[p+2]=s[p+2];
		if(size>=4) d[p+3]=s[p+3];

		p+=size;
	}
}

void _loop_float(void *dst,void *src,int count,int size)
{

	if(src)
	{
		int i;
		float *s=(float *)src;
		float *d=(float *)dst;

		int p=0;

		for(i=0;i<count;i++)
		{
			if(size>=1) d[p+0]=s[p+0];
			if(size>=2) d[p+1]=s[p+1];
			if(size>=3) d[p+2]=s[p+2];
			if(size>=4) d[p+3]=s[p+3];

			p+=size;
		}
	}
	else
	{
		int i;
		float *d=(float *)dst;

		int p=0;

		float pp=.0001;

		for(i=0;i<count;i++)
		{
			if(size>=1) d[p+0]+=u_randrange(0,100)*pp;
			if(size>=2) d[p+1]+=u_randrange(0,100)*pp;
			if(size>=3) d[p+2]+=u_randrange(0,100)*pp;
			if(size>=4) d[p+3]+=u_randrange(0,100)*pp;

			p+=size;
		}
	}
}

void _loop_unsigned_int(void *dst,void *src,int count,int size)
{
	int i;
	unsigned int *s=(unsigned int *)src;
	unsigned int *d=(unsigned int *)dst;

	int p=0;

	for(i=0;i<count;i++)
	{
		if(size>=1) d[p+0]=s[p+0];
		if(size>=2) d[p+1]=s[p+1];
		if(size>=3) d[p+2]=s[p+2];
		if(size>=4) d[p+3]=s[p+3];

		p+=size;
	}
}

void _face_int(void *src,void *dst,int count,int size)
{
	int i;
	int *d=(int *)dst;

	int offset=0;

	for(i=0;i<count;i++)
	{
		if(size>=1) d[0]=offset;
		if(size>=2) d[1]=offset+1;
		if(size>=3) d[2]=offset+2;
		if(size>=4) d[3]=offset+3;

		d+=size;
		offset+=size;
	}
}

void _face_float(void *src,void *dst,int count,int size)
{
	printf("Errrr\n");
	/*
	*
	*
	*/
}

void _face_unsigned_int(void *src,void *dst,int count,int size)
{
	int i;
	unsigned int *d=(unsigned int *)dst;

	int offset=0;

	for(i=0;i<count;i++)
	{
		if(size>=1) d[0]=(unsigned int)offset;
		if(size>=2) d[1]=(unsigned int)offset+1;
		if(size>=3) d[2]=(unsigned int)offset+2;
		if(size>=4) d[3]=(unsigned int)offset+3;

		d+=size;
		offset+=size;
	}
}

void vlst_color_3f(t_vlst *vlst,int *color)
{
	if(vlst)
	{
		float r=((float)color[0])/255.0f;
		float g=((float)color[1])/255.0f;
		float b=((float)color[2])/255.0f;

		int i;
		float *d=(float *)vlst->data;

		int pos=0;

		for(i=0;i<vlst->count;i++)
		{

			d[pos]=r;
			d[pos+1]=g;
			d[pos+2]=b;

			pos+=3;
		}
	}
}

void vlst_normal_3f(t_vlst *dst,t_vlst *vertex,t_vlst *face)
{
	int i;
	int pos=0;
	int _pos=0;

	float *v=(float *)vertex->data;
	float *d=(float *)dst->data;

	for(i=0;i<face->count;i++)
	{
		float v1[3]={v[pos],v[pos+1],v[pos+2]};
		float v2[3]={v[pos+3],v[pos+4],v[pos+5]};
		float v3[3]={v[pos+6],v[pos+7],v[pos+8]};

		float result[3];

		calc_normals(v1,v2,v3,result);

		int k;
		for(k=0;k<3;k++)
		{
			d[_pos+0]=result[0];
			d[_pos+1]=result[1];
			d[_pos+2]=result[2];

			_pos+=3;
		}

		pos+=9; // 4 vertices
	}	
}

void vlst_normal_4f(t_vlst *dst,t_vlst *vertex,t_vlst *face)
{
	int i;
	int pos=0;
	int _pos=0;

	float *v=(float *)vertex->data;
	float *d=(float *)dst->data;

	for(i=0;i<face->count;i++)
	{
		float v1[3]={v[pos],v[pos+1],v[pos+2]};
		float v2[3]={v[pos+3],v[pos+4],v[pos+5]};
		float v3[3]={v[pos+6],v[pos+7],v[pos+8]};

		float result[3];

		calc_normals(v1,v2,v3,result);

		int k;
		for(k=0;k<4;k++)
		{
			d[_pos+0]=result[0];
			d[_pos+1]=result[1];
			d[_pos+2]=result[2];

			_pos+=3;
		}

		pos+=12; // 4 vertices
	}	
}

void vlst_normal_4f_direct(t_vlst *dst,t_vlst *vertex,t_vlst *quads)
{
	int i,n,j;
	float vectors[3*4];
	int *q=quads->data;
	float *v=vertex->data;
	float *normal=dst->data;

	// calc normals
	j=0;
	for(i=0;i<quads->count;i++)
	{
		for(n=0;n<4;n++)
		{
			vectors[(n*3)+0]=v[(q[j]*3)];
			vectors[(n*3)+1]=v[(q[j]*3)+1];
			vectors[(n*3)+2]=v[(q[j]*3)+2];
			j++;
		}

		calc_normals(vectors,vectors+3,vectors+6,normal+(i*3));
	}
}

void vlst_vertex(const char *type,t_vlst *dst,t_vlst *vertex,t_vlst *face)
{
	int i;
	int *f=face->data;
	float *v=(float *)vertex->data;
	float *d=(float *)dst->data;

	int j=0;
	int pos=0;

	for(i=0;i<face->count;i++)
	{
		if(face->length>=1)
		{
			d[pos+0] = v[(f[j+0]*3)+0];
			d[pos+1] = v[(f[j+0]*3)+1];
			d[pos+2] = v[(f[j+0]*3)+2];
		}
		if(face->length>=2)
		{
			d[pos+3] = v[(f[j+1]*3)+0];
			d[pos+4] = v[(f[j+1]*3)+1];
			d[pos+5] = v[(f[j+1]*3)+2];
		}
		if(face->length>=3)
		{
			d[pos+6] = v[(f[j+2]*3)+0];
			d[pos+7] = v[(f[j+2]*3)+1];
			d[pos+8] = v[(f[j+2]*3)+2];
		}
		if(face->length>=4)
		{
			d[pos+9]  = v[(f[j+3]*3)+0];
			d[pos+10] = v[(f[j+3]*3)+1];
			d[pos+11] = v[(f[j+3]*3)+2];
		}

		pos+=(face->length*3);
		j+=face->length;

		//printf("done\n");
	}
}

// COPY

void vlst_add_data(t_vlst *vlst,void *ptr)
{
	t_vlst_type t=vlst->type;

	if	(t==_1i || t==_2i || t==_3i || t==_4i)
		 _loop_int(vlst->data,ptr,vlst->count,vlst->length);
	else if	(t==_1f || t==_2f || t==_3f || t==_4f)
		 _loop_float(vlst->data,ptr,vlst->count,vlst->length);
	else if	(t==_1ui || t==_2ui || t==_3ui || t==_4ui)
		 _loop_unsigned_int(vlst->data,ptr,vlst->count,vlst->length);
	else 	
		printf("[ERROR vlst_add_data] Unknown type %d\n",t);
}

void vlst_copy(t_vlst *dst,t_vlst *src)
{
	printf("VLST COPY\n");
	if(src->type==dst->type)
	{
		t_vlst_type t=src->type;

		if	(t==_1i || t==_2i || t==_3i || t==_4i)
			 _loop_int(dst->data,src->data,src->count,src->length);
		else if	(t==_1f || t==_2f || t==_3f || t==_4f)
			 _loop_float(dst->data,src->data,src->count,src->length);
		else if	(t==_1ui || t==_2ui || t==_3ui || t==_4ui)
			 _loop_unsigned_int(dst->data,src->data,src->count,src->length);
		else 	
			printf("[ERROR vlst_copy] Unknown type %d\n",t);
	}
	else
	{
		printf("[ERROR vlst_copy]Vlists don't match\n");
	}

	vlst_show(dst);
}

// SET

void vlst_set(t_vlst *vlst,int i)
{
	printf("VLST SET\n");
	vlst_copy(vlst,NULL);
}

// BUILD

void vlst_build(t_vlst *vlst,void *ptr)
{
	t_vlst_type t=vlst->type;

	if	(t==_1i || t==_2i || t==_3i || t==_4i)
		 _face_int(ptr,vlst->data,vlst->count,vlst->length);
	else if	(t==_1f || t==_2f || t==_3f || t==_4f)
		 _face_float((float *)vlst->data,(float *)ptr,vlst->count,vlst->length);
	else if	(t==_1ui || t==_2ui || t==_3ui || t==_4ui)
		 _face_unsigned_int(ptr,vlst->data,vlst->count,vlst->length);
	else 	
		printf("[ERROR _copy] Unknown type %d\n",t);
}

// MAKE

t_vlst *vlst_make(const char *name,t_vlst_type type,int count)
{
	t_context *C=ctx_get();

	t_node *node_vlst=scene_add(C->scene,nt_vlst,name);
	t_vlst *vlst=node_vlst->data;

	vlst->type=type;
	vlst->count=count;
	vlst->count_new=vlst->count;


	if(type==_1i)
	{
		vlst->size=sizeof(int);
		vlst->length=1;
		vlst->data_type=dt_int;
	}
	else if(type==_2i)
	{
		vlst->size=sizeof(int); 
		vlst->length=2;
		vlst->data_type=dt_int;
	}
	else if(type==_3i) 
	{
		vlst->size=sizeof(int);
		vlst->length=3;
		vlst->data_type=dt_int;
	}
	else if(type==_4i)
	{ 
		vlst->size=sizeof(int);
		vlst->length=4;
		vlst->data_type=dt_int;
	}
	else if(type==_1f) 
	{
		vlst->size=sizeof(float); 
		vlst->length=1; 
		vlst->data_type=dt_float;
	}
	else if(type==_2f)
	{ 
		vlst->size=sizeof(float); 
		vlst->length=2;
		vlst->data_type=dt_float;
	}
	else if(type==_3f)
	{ 
		vlst->size=sizeof(float); 
		vlst->length=3;
		vlst->data_type=dt_float;
	}
	else if(type==_4f)
	{ 
		vlst->size=sizeof(float); 
		vlst->length=4;
		vlst->data_type=dt_float;
	}
	else if(type==_1ui)
	{
		vlst->size=sizeof(unsigned int); 
		vlst->length=1;
		vlst->data_type=dt_uint;
	}
	else if(type==_2ui)
	{
		vlst->size=sizeof(unsigned int);
		vlst->length=2;
		vlst->data_type=dt_uint;
	}
	else if(type==_3ui) 
	{
		vlst->size=sizeof(unsigned int);
		vlst->length=3;
		vlst->data_type=dt_uint;
	}
	else if(type==_4ui)
	{
		vlst->size=sizeof(unsigned int);
		vlst->length=4;
		vlst->data_type=dt_uint;
	}

	else printf("[ERROR vlst_make] Unknown type %d\n",type);

	vlst->data=malloc((vlst->size)*(vlst->length)*(vlst->count));

	if(C->scene->store)
	{
		 scene_add_data_var(C->scene,"vlst_data","v_data",((vlst->size)*(vlst->length)*(vlst->count)),vlst->data);
	}

	return vlst;
}

// CLONE

t_vlst *vlst_duplicate(t_vlst *vlst)
{
	printf("VLST DUPLICATE\n");
	t_vlst *vlst_new=vlst_make(vlst->name,vlst->type,vlst->count);
	vlst_copy(vlst_new,vlst->data);

	return vlst_new;
}

// INIT

void vlst_init(t_vlst *vlst)
{
	int size=vlst->count*vlst->length*vlst->size;
	bzero(vlst->data,size);
}


// REBIND

t_vlst *vlst_rebind(t_scene *sc,void *ptr)
{
	t_vlst *vlst=(t_vlst *)ptr;

	check_init("VLST",vlst->name);

	// REBIND

	rebind(sc,"vlst","data",(void **)&vlst->data);
	rebind(sc,"vlst","link",(void **)&vlst->link);

	check_check("VLST",vlst->name);

	return vlst;
}

// NEW

t_vlst *vlst_new(const char *name)
{
	t_vlst *vlst=(t_vlst *)malloc(sizeof(t_vlst));

	vlst->id=0;
	vlst->id_chunk=0;
	set_name(vlst->name,name);
	vlst->users=0;
	vlst->type=_0;
	vlst->count=0;
	vlst->count_new=0;
	vlst->data=NULL;
	vlst->need_update=0;
	vlst->is_linked=0;
	vlst->link=NULL;

	return vlst;
}

void vlst_free(t_vlst *vlst)
{
	if(vlst->data) free(vlst->data);
}

void cls_vector_init(t_vector *vector)
{
	vector->data = NULL;
}

t_vector_cls cls_vector=
{
	.init=cls_vector_init
};

void vector_free(t_vector *vector)
{
}

t_node *vector_add(const char *name)
{
	t_context *C = ctx_get();
	t_node *node = scene_add(C->scene,nt_vector,name);
	 return node;
}

t_vector *vector_new(const char *name)
{
	t_vector *vector = (t_vector *) malloc(sizeof(t_vector));

	vector->id=0;
	vector->id_chunk=0;
	set_name(vector->name,name);
	vector->users=0;

	vector->type=dt_null;
	vector->length=0;
	vector->data=NULL;

	vector->cls=&cls_vector;

	return vector;
}
