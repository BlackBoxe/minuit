/* 
 * Copyright (c) 2012 Milovann Yanatchkov 
 *
 * This file is part of Minuit, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "util.h"

int u_randn(int density)
{
	int num = rand()%density;

	return num;
}

int u_randrange(int start,int end)
{
	int n;
	if (start != end)  n = rand()%(end-start) + start;
	else n =start;
	return n;
}



