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

#include "sb-display.h"

G_DEFINE_TYPE (SbDisplay, sb_display, GTK_TYPE_TEXT_VIEW);

static void
sb_display_init (SbDisplay* self)
{}

static void
sb_display_class_init (SbDisplayClass* self_class)
{}

GtkWidget*
sb_display_new (void)
{
	return g_object_new (SB_TYPE_DISPLAY, NULL);
}

void
sb_display_load_path (SbDisplay  * self,
		      gchar const* path,
		      GError     **error)
{
	GtkTextBuffer* buffer;
	GMappedFile  * file;

	file = g_mapped_file_new (path, FALSE, error);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self));

	gtk_text_buffer_set_text (buffer,
				  g_mapped_file_get_contents (file),
				  g_mapped_file_get_length (file));

	g_mapped_file_free (file);
}

