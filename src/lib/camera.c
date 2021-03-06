/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "op.h"

void cls_camera_init(t_camera *camera)
{
}

t_camera_cls cls_camera=
{
	.cls_type="camera",
	.init=cls_camera_init,
};

t_camera *camera_clone(t_camera *source)
{
	t_node *node_target = camera_make("clone");
	t_camera *target = node_target->data;
	camera_copy(target,source);

	return target;
}

void camera_copy(t_camera *target,t_camera *source)
{
	target->type = source->type;

	vcp(target->cross,source->cross);
	vcp(target->pos,source->pos);
	vcp(target->eye,source->eye);
	vcp(target->target,source->target);
	vcp(target->up,source->up);

	target->ortho_view = source->ortho_view;
	target->ortho_zoom = source->ortho_zoom;

	vcp(target->ortho_location,source->ortho_location);
	vcp(target->ortho_rotation,source->ortho_rotation);

	target->ortho_near=source->ortho_near;
	target->ortho_far=source->ortho_far;

	target->o_left=source->o_left;
	target->o_right=source->o_right;
	target->o_bottom=source->o_bottom;
	target->o_top=source->o_top;

	target->f_aspect=source->f_aspect;
	target->f_fovy=source->f_fovy;
	target->f_left=source->f_left;
	target->f_right=source->f_right;
	target->f_bottom=source->f_bottom;
	target->f_top=source->f_top;
	target->f_near=source->f_near;
	target->f_far=source->f_far;

	target->zenith=source->zenith;

	target->frame=source->frame;
}

void camera_free(t_camera *camera)
{
	free(camera);
}

t_node *camera_make(const char *name)
{
	t_context *C=ctx_get();
	t_node *node_camera=scene_add(C->scene,nt_camera,name);
	return node_camera;
}

void camera_init(t_camera *camera)
{
	camera->cls=&cls_camera;
}

t_camera *camera_new(const char *name)
{
	t_camera *camera  = (t_camera *)malloc(sizeof(t_camera));

	camera->id=0;
	camera->id_chunk=0;
	set_name(camera->name,name);
	camera->users=0;

	camera->restrict_matrix=0;
	camera->type = camera_frustum;

	vset(camera->cross,0,0,0);
	vset(camera->pos,0,0,0);
	vset(camera->eye,CAM_EYE,CAM_EYE,CAM_EYE);
	vset(camera->target,0,0,0);
	vset(camera->up,0,1,0);

	camera->ortho_view = 0;
	camera->ortho_zoom = CAM_ORTHO_ZOOM;

	vset(camera->ortho_location,0,0,0);
	vset(camera->ortho_rotation,1,0,0);

	camera->ortho_near=CAM_ORTHO_NEAR;
	camera->ortho_far=CAM_ORTHO_FAR;
	camera->new_ortho_zoom=0.01;

	camera->f_near=CAM_NEAR;
	camera->f_far=CAM_FAR;
	camera->f_left=0;
	camera->f_right=0;
	camera->f_bottom=0;
	camera->f_top=0;

	camera->o_left=0;
	camera->o_right=0;
	camera->o_bottom=0;
	camera->o_top=0;

	camera->ui_left=0;
	camera->ui_right=0;
	camera->ui_bottom=0;
	camera->ui_top=0;
	camera->ui_zoom=0.01;

	camera->zenith=0;
	camera->frame=0;

	camera->is_moving=CAM_IS_MOVING;
	camera->rec_mode=CAM_REC_MODE;

	return camera;
}
