/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "draw.h"
#include "sketch.h"

t_draw *DRAW;

GLenum LightList[] = {
	GL_LIGHT0,
	GL_LIGHT1,
	GL_LIGHT2,
	GL_LIGHT3,
};

void draw_switch_axis_world(t_draw *draw)
{
	if (draw->show_axis_world) draw->show_axis_world=0;
	else draw->show_axis_world=1;
}

void draw_clear(float r,float g,float b,float a)
{
	glClearColor(r,g,b,a);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT);
	
}

void draw_add_light(t_node *node)
{
	t_light *light = node->data;

	float *p=light->pos;

	float pos[4];
	pos[0]=p[0];
	pos[1]=p[1];
	pos[2]=p[2];
	pos[3]=1;

	GLenum id = LightList[light->id_gl];

	glLightfv(id, GL_POSITION,pos);
	glLightfv(id, GL_DIFFUSE, light->diffuse);
	glLightfv(id, GL_SPECULAR, light->specular);

	glEnable(id);
}

void draw_light(t_draw *draw, t_node *node)
{
	t_light *light = node->data;
	float *pos=light->pos;

	if(draw->mode==mode_selection)
	{	
		int *col=light->idcol;
		float c[3];
		cconv(c,col);

		skt_point(pos,20,c);
	}
	else
	{
		float color[]={0,0,0};
		int width=5;
		skt_point(pos,width,color);
	}
}

void draw_mesh_line(t_draw *draw, t_mesh *mesh)
{
	/*
	if(mesh->lines)
	{
		int i;
		int pos=0;
		for(i=0;i<mesh->var.totline;i+=2)
		{
			int id1 = mesh->lines[pos];
			int id2 = mesh->lines[pos+1];

			t_vertex *v1 = &mesh->verts[id1];
			t_vertex *v2 = &mesh->verts[id2];

			float *ve1 = v1->co;
			float *ve2 = v2->co;

			// selection pass
			if(draw->mode==mode_selection) 
			{
			}
			// render pass
			else
			{
				float *color=draw->front_color;
				if(mesh->state.is_selected)
				{
					skt_line(ve1,ve2,2,color);
				}
				else
				{
					skt_line(ve1,ve2,1,color);
				}
			}

			pos+=2;
		}
	}
	*/
}

void draw_points(t_draw *draw,t_mesh *mesh)
{
	int i;

	float *v=mesh->vertex->data;

	for(i=0;i<mesh->var.tot_vertex;i++)
	{
		/*
		if(draw->with_point_id && mesh->vertex)
		{
			glEnable(GL_LIGHTING);

				char text[5];
				sprintf(text,"%d,",i);
				float v[3];
				float a[3]={.1,.1,.1};

				vcp(v,mesh->verts[i].co);
				vadd(v,v,a);
				type_font_3d(text,mesh->verts[i].co);

			glDisable(GL_LIGHTING);
		}
		*/

		float *color=draw->front_color;
		//float *pos = mesh->verts[i].co;
		
		float pos[3];
		pos[0]=v[0];
		pos[1]=v[1];
		pos[2]=v[2];

		skt_point(pos,1,color);

		v+=3;
	}
}

void draw_mesh_direct(t_draw *draw,t_scene *scene,t_mesh *mesh)
{
	t_vlst *vertex=mesh->vertex;
	t_vlst *quads=mesh->quads;
	GLfloat *v=vertex->data;	// vertices 
	int *q=quads->data;		// quad indices

	int i,j,n;

	t_skt *skt=skt_get();

	// BUFFERS

	if(mesh->state.buffer_type!=buffer_direct) mesh_init_buffers(mesh,buffer_direct); 

	if (draw->with_material)
	{
		t_material *material = mesh->material;

		if(material)
		{
			float color[4];

			color[0]=material->color[0];
			color[1]=material->color[1]; 
			color[2]=material->color[2];
			color[3]=material->color[3];

			glMaterialfv(GL_FRONT,GL_SPECULAR, mesh->material->specular);
			glMaterialfv(GL_FRONT,GL_SHININESS, mesh->material->shininess);
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, color);
		}
	}

	//points
	if(draw->with_point)
	{
		t_vlst *vlst_color;
		float *color;

		if(mesh->state.has_color) 
		{
			vlst_color=mesh->colors;
			color=vlst_color->data;
		}
		else
		{
			color=draw->front_color;
		}


		glEnable(GL_POINT_SMOOTH);
		glPointSize(1*skt->point_size*skt->scale);

		if(!mesh->state.has_color)
		{
			glColor3f(color[0],color[1],color[2]);
		}

		glBegin(GL_POINTS);

		j=0;
		for(i=0;i<vertex->count;i++)
		{
			if(mesh->state.has_color)
			{
				glColor3f(color[0],color[1],color[2]);
				//XXX
				color+=3;
			}

			glVertex3f(v[j],v[j+1],v[j+2]);

			j+=3;
		}
		glEnd();
	}

	glShadeModel(GL_FLAT);

	if (draw->with_light)
	{
		glEnable(GL_LIGHTING);
		GLfloat model_ambient[] = {1,1,1,1};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,model_ambient);
	}

	
	// quads
	if(q && draw->with_face)
	{

		t_vlst *vlst_quad=mesh->quad_normal;
		float *normals=vlst_quad->data;

		/*
		t_vlst *vlst_colors=mesh->colors;
		float *colors=vlst_colors->data;
		*/

		// draw faces
		j=0;
		for(i=0;i<quads->count;i++)
		{
			glBegin(GL_QUADS);
			for(n=0;n<4;n++)
			{
				//int indice=q[j];
				//float *c=colors+(indice*4);

				// normal
				if(draw->with_normal)
					glNormal3f(normals[(i*3)+0],normals[(i*3)+1],normals[(i*3)+2]);

				// color
				//glColor4f(c[0],c[1],c[2],c[3]);
				//glColor3f((float)u_randrange(0,255)/255,1,1);
				//glColor3f(1,0,0);

				// vertex
				glVertex3f(v[(q[j]*3)],v[(q[j]*3)+1],v[(q[j]*3)+2]);
				j++;
			}
			glEnd();
		}
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
}

void draw_mesh(t_draw *draw, t_scene *scene, t_mesh *mesh)
{
	t_node *node;
	t_texture *texture=NULL;

	// BUFFERS
	if(mesh->state.buffer_type!=buffer_vbo) mesh_init_buffers(mesh,buffer_vbo); 

	float *color=draw->front_color;

	if(draw->mode==mode_selection) glEnableClientState(GL_COLOR_ARRAY);
	if(draw->with_normal) glEnableClientState(GL_NORMAL_ARRAY);

	// TEXTURE
	if(draw->with_texture && mesh->state.with_texture)
	{
		node = scene_node_get(scene,"texture",mesh->texture_name);
		texture=node->data;

		if(texture && mesh->quad_uv)
		{
			glEnable(GL_TEXTURE_2D);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_INT,0,mesh->quad_uv->data);
			glBindTexture(GL_TEXTURE_2D,texture->id_gl); 

		}
	}

	// MATERIAL
	if (draw->with_material)
	{
		t_material *material = mesh->material;

		if(material)
		{
			float color[4];

			color[0]=material->color[0];
			color[1]=material->color[1]; 
			color[2]=material->color[2];
			color[3]=material->color[3];

			glMaterialfv(GL_FRONT,GL_SPECULAR, mesh->material->specular);
			glMaterialfv(GL_FRONT,GL_SHININESS, mesh->material->shininess);
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE, color);
		}
	}


	// vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);

	glShadeModel(GL_FLAT);

	// draw node

	// SELECTION MODE for drawing wire cube to solid cube
	if(draw->mode==mode_selection)
	{
		if (draw->with_light)
		{
			glEnable(GL_LIGHTING);
			GLfloat model_ambient[] = {1,1,1,1};
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,model_ambient);
		}
		// quads 
		//if (mesh->quad_faces && mesh->state.has_quad)
		if (mesh->state.has_quad)
		{
			glVertexPointer(3,GL_FLOAT,0,(float *)mesh->quad_vertex->data);
			if(mesh->quad_normal && draw->with_normal) glNormalPointer(GL_FLOAT,0,mesh->quad_normal->data);
			if(mesh->quad_color && draw->mode==mode_selection) glColorPointer(3,GL_FLOAT,0,mesh->quad_color->data);
			if(mesh->quad_face) glDrawElements(GL_QUADS,mesh->var.tot_quad_face*4,GL_UNSIGNED_INT,mesh->quad_face->data); 
		}

		// triangles
		if(mesh->state.has_tri)
		{
			glVertexPointer(3,GL_FLOAT,0,(float *)mesh->tri_vertex->data);
			if(mesh->tri_normal && draw->with_normal) glNormalPointer(GL_FLOAT,0,mesh->tri_normal->data);
			if(mesh->tri_color && draw->mode==mode_selection) glColorPointer(3,GL_FLOAT,0,mesh->tri_color->data);
			if(mesh->tri_face) glDrawElements(GL_TRIANGLES,mesh->var.tot_tri_face*3,GL_UNSIGNED_INT,mesh->tri_face->data); 

		}
	}
	// DRAW MODE draw faces
	//else if(draw->with_face && mesh->state.has_face)
	else if(draw->with_face)
	{
		if (draw->with_light)
		{
			glEnable(GL_LIGHTING);
			GLfloat model_ambient[] = {1,1,1,1};
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,model_ambient);
		}

		// quads 

		if (mesh->state.has_quad)
		{
			glVertexPointer(3,GL_FLOAT,0,(float *)mesh->quad_vertex->data);
			if(mesh->quad_normal && draw->with_normal) glNormalPointer(GL_FLOAT,0,mesh->quad_normal->data);
			if(mesh->quad_color && draw->mode==mode_selection) glColorPointer(3,GL_FLOAT,0,mesh->quad_color->data);
			if(mesh->quad_face) glDrawElements(GL_QUADS,mesh->var.tot_quad_face*4,GL_UNSIGNED_INT,mesh->quad_face->data); 
		}


		// triangles

		if(mesh->state.has_tri)
		{
			if (mesh->tri_vertex)
			{
				glVertexPointer(3,GL_FLOAT,0,mesh->tri_vertex->data);
				if(mesh->tri_normal && draw->with_normal) glNormalPointer(GL_FLOAT,0,mesh->tri_normal->data);
				if(mesh->tri_color && draw->mode==mode_selection) glColorPointer(3,GL_FLOAT,0,mesh->tri_color->data);//XXX
				if(mesh->tri_face) glDrawElements(GL_TRIANGLES,mesh->var.tot_tri_face*3,GL_UNSIGNED_INT,mesh->tri_face->data);
			}
		}
	}

	// lines
	//if(mesh->state.with_line) draw_mesh_line(draw,mesh);

	// outline
	if(
		mesh->state.is_selected
		&& draw->mode == mode_draw
		&& draw->with_face)
	{
		if(mesh->state.has_face)
		{
			glDisable(GL_LIGHTING);
			glPushAttrib( GL_ALL_ATTRIB_BITS );
			glEnable( GL_POLYGON_OFFSET_FILL );
			glPolygonOffset( -2.5f, -2.5f );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glLineWidth( 4.0f );
			glColor3f(color[0],color[1],color[2]);

			if(mesh->state.has_quad) glDrawElements(GL_QUADS,mesh->var.tot_quad_face*4,GL_UNSIGNED_INT,mesh->quad_face->data); 

			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glEnable( GL_LIGHTING );
			glColor3f( 0.0f, 0.0f, 0.0f );

			if(mesh->state.has_quad) glDrawElements(GL_QUADS,mesh->var.tot_quad_face*4,GL_UNSIGNED_INT,mesh->quad_face->data); 

			glPopAttrib();

		}
	}


	// unbind texture
	if(texture) glBindTexture(GL_TEXTURE_2D,0); 

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	// points
	if(draw->with_point) draw_points(draw,mesh);
}

void draw_lights(t_draw *draw, t_scene *scene)
{
	t_node *node;
	t_link *link;

	// draw lights nodes
	for(link=scene->lights->first;link;link=link->next)
	{
		node=link->data;
		draw_light(draw,node);
	}

	for(link=scene->lights->first;link;link=link->next)
	{
		node=link->data;
		draw_add_light(node); 
	}
}

void type_font_3d(char* string,float pos[3])
{
	glRasterPos3f(pos[0],pos[1],pos[2]);

	int n = (int)strlen(string);
	void * font = GLUT_BITMAP_8_BY_13;
	int i;
	for (i=0;i<n;i++)
	{
    		glutBitmapCharacter(font, string[i]);
	}
}

void draw_objects(t_draw *draw, t_scene *scene)
{
	t_node *node;
	t_link *link;

	for(link=scene->objects->first;link;link=link->next)
	{
		node=link->data;
		t_object *object = node->data;

		if(!object)
		{
			printf("[ERROR draw_objects] No data\n");
			break;
		}
		else
		{
			object->cls->draw(object);
		}
	}
}


/*axis*/

void draw_axis_world(t_draw *draw)
{
	if (draw->show_axis_world) 
	{
		float v0[] = {0,0,0};
		float vx[] = {1,0,0};
		float vy[] = {0,1,0};
		float vz[] = {0,0,1};
		float red[]={1,0,0};
		float green[]={0,1,0};
		float blue[]={0,0,1};

		glDisable(GL_DEPTH_TEST);

		skt_vector(v0,vx,1,red);
		skt_vector(v0,vy,1,green);
		skt_vector(v0,vz,1,blue);

		glEnable(GL_DEPTH_TEST);
	}
}

void draw_init(t_draw *draw)
{
	float c[4];
	bzero(c,4);
	cconv(c,draw->background_color);

	// clear
	if(draw->with_clear)
	{
		draw_clear(c[0],c[1],c[2],c[3]);
	}

	// set depth
	if(draw->with_depth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	
	// polygon offset
	if(draw->with_polygon_offset)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0,1.0);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	if(draw->with_blend)
	{
		glEnable(GL_BLEND);
    		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}

}

void draw_scene(t_draw *draw, t_scene *scene)
{
	// INIT
	draw_init(draw);

	// DRAW

	draw_lights(draw,scene);
	draw_objects(draw,scene);
	draw_axis_world(draw);

	// UI
	ui_draw();
}

t_draw *draw_new(void)
{
	t_draw *draw = (t_draw *)malloc(sizeof(t_draw));

	DRAW=draw;
	
	draw->background_color[0]=DRAW_COLOR_BACKGROUND_RED;
	draw->background_color[1]=DRAW_COLOR_BACKGROUND_GREEN;
	draw->background_color[2]=DRAW_COLOR_BACKGROUND_BLUE;
	draw->background_color[3]=DRAW_COLOR_BACKGROUND_ALPHA;

	vset4f(draw->front_color,0,0,0,0);

	draw->mode=mode_draw;
	//draw->color=color_white;
	draw->color=DRAW_COLOR;

	draw->with_selection_pass=DRAW_SELECTION_PASS; 
	draw->with_draw_pass=DRAW_WITH_DRAW;

	draw->with_clear=DRAW_WITH_CLEAR;
	draw->with_ui=DRAW_WITH_UI;

	draw->with_restrict_matrix=1;

	draw->mode_direct=1;

	draw->show_axis_world=DRAW_SHOW_AXIS_WORLD;
	draw->with_normal=DRAW_WITH_NORMAL;
	draw->with_texture=DRAW_WITH_TEXTURE;
	draw->with_material=DRAW_WITH_MATERIAL;
	draw->with_point=DRAW_WITH_POINT;
	draw->with_point_id=DRAW_WITH_POINT_ID;
	draw->with_face=DRAW_WITH_FACE;
	draw->with_highlight=DRAW_WITH_HIGHLIGHT;
	draw->with_light=DRAW_WITH_LIGHT;
	draw->with_depth=DRAW_WITH_DEPTH;
	draw->with_polygon_offset=DRAW_WITH_POLYGON_OFFSET;
	draw->with_alpha=DRAW_WITH_ALPHA;
	draw->with_blend=DRAW_WITH_BLEND;
	draw->with_grid=0;
	draw->img_with_alpha=0;
	draw->divx=2;
	draw->divy=2;

	return draw;
}



