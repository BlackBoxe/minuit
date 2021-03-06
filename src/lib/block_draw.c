/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "ui.h"
#include "context.h"

// update txt && mouse over
void block_update_data(t_block *block)
{
	t_link *link;
	t_brick *brick;

	// loop over bricks
	for(link=block->bricks->first;link;link=link->next)
	{
		brick = link->data;

		// update txt
		if(brick->state.draw_value)
		{
			if(is(brick->name,"camera_main")) printf("block:brick\n");
			brick_build_txt(brick);
		}

		// build width (once)
		if(block->state.set_global_width)
		{
			brick_build_width(brick);
		}
	}
}


// update geometry

void block_update_width(t_block *block)
{
	if(block->state.set_global_width)
	{
		t_link *link;

		float block_width=0;
		float width=0;

		// switch state
		block->state.set_global_width=0;

		// reset block height
		block->height=0;


		// loop over bricks
		for(link=block->bricks->first;link;link=link->next)
		{
			t_brick *brick = link->data;

			// store brick height
			block->height=brick->geom.height;

			// set draw plugs
			if(brick->state.draw_plugs)
			{
				block->state.draw_plugs=1;
			}

			// find max width

			width = brick->geom.width;

			if(width >= block_width)
			{
				block_width=width;
			}
		}

		// store max width
		block->width=block_width;
	}

}

int block_ref_draw_bricks(t_block *block)
{
	t_link *link;
	t_brick *brick;

	int pos = block->cursor;

	glPushMatrix();
	//glLoadIdentity();

	// translate to block pos
	glTranslatef(block->pos[0],block->pos[1],0);

	// loop over bricks
	for(link=block->bricks->first;link;link=link->next)
	{
		brick = link->data;

		// pos++
		pos++;

		// draw brick
		brick->cls->draw(brick);

		// translate y
		glTranslatef(0,brick->geom.height,0);
	}

	glPopMatrix();

	return pos;
}

int block_draw_bricks(t_block *block)
{
	t_link *link;
	t_brick *brick;

	int pos = block->cursor;
	int offset=2;


	// root
	if(block->state.is_root)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(block->pos[0],block->pos[1],0);
		//glLoadIdentity();
	}

	// translate to block pos
	//glTranslatef(block->pos[0],block->pos[1],0);
	//glPushMatrix();
	//glLoadIdentity();



	// loop over bricks
	for(link=block->bricks->first;link;link=link->next)
	{
		brick = link->data;

		// pos++
		pos++;

		// draw brick
		brick->cls->draw(brick);

		// translate y
		if(brick->state.is_contextual)
		{

			t_context *C=ctx_get();

			if(C->scene->selected)
			{
				t_node *node=C->scene->selected;
				if(node->type==brick->context)
				{
					glTranslatef(0,brick->geom.height,0);
				}
			}
		}
		else
		{
			glTranslatef(0,brick->geom.height,0);
		}

		// if menu brick
		if(brick->cls->type==bt_menu)
		{
			if(brick->state.show_menu)
			{
				// leaf
				glPushMatrix();

					// translate left
					glTranslatef(((block->width)+offset),0,0);
					// translate down
					glTranslatef(0,-brick->geom.height,0);

					// draw sub-block
					t_block *menu = brick->menu;
					menu->draw(menu);

				glPopMatrix();
			}
		}
	}

	// root
	if(block->state.is_root)
	{
		glPopMatrix();
	}

	return pos;
}

void block_draw_outline(t_block *block)
{
	t_context *C=ctx_get();

	if(block->state.draw_outline)
	{
		int tot=4;
		float *color=C->ui->front_color;
		float points[tot*3];
		float *pos=block->pos;
		int line_width;

		// set line width
		if(block->state.is_mouse_over) line_width=2; 
		else  line_width=1; 

		float width=block->width;

		if(block->state.draw_plugs)
		{
			t_link *link=block->bricks->first;
			t_brick *brick=link->data;
			float h=brick->geom.height;
			width+=(h*2);
		}

		float a[3]={pos[0],pos[1],0};
		float b[3]={a[0],a[1]+(block->height*block->tot_bricks),0};
		float c[3]={b[0]+width,b[1],0};
		float d[3]={c[0],c[1]-(block->height*block->tot_bricks),0};

		points[0]=a[0];
		points[1]=a[1];
		points[2]=a[2];

		points[3]=b[0];
		points[4]=b[1];
		points[5]=b[2];

		points[6]=c[0];
		points[7]=c[1];
		points[8]=c[2];

		points[9]=d[0];
		points[10]=d[1];
		points[11]=d[2];

		skt_closedline(points,tot,color,line_width);
	}
}

void block_draw(t_block *block)
{
	block_update_data(block);
	block_update_width(block);
	block_draw_bricks(block);
	block_draw_outline(block);
}

void cls_block_draw_generic(t_block *self)
{
}
void cls_block_draw_menu(t_block *self)
{
}
void cls_block_draw_ref(t_block *block)
{
	block_update_data(block);
	block_update_width(block);
	block_ref_draw_bricks(block);
	block_draw_outline(block);
}

void cls_block_draw_block(t_block *block)
{
	block_update_data(block);
	block_update_width(block);
	block_ref_draw_bricks(block);
	block_draw_outline(block);
}
	
