/* This file is part of source browser
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2007  Sven Herzberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "sb-callback-data.h"

SbCallbackData*
sb_callback_data_new  (gchar const* name,
		       ...)
{
	SbCallbackData* self = g_new0 (gpointer, 2);
	gsize len = 0;
	if (name) {
		va_list argv;
		gpointer data;
		GDestroyNotify notify;

		va_start (argv, name);

		data   = va_arg (argv, gpointer);
		notify = va_arg (argv, GDestroyNotify);

		self[len] = data;

		name = va_arg (argv, gchar const*);
	}
	return self;
}

void
sb_callback_data_free (gpointer self)
{
	g_free (self);
}
