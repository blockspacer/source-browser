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

#ifndef SB_MEM_POINTER_H
#define SB_MEM_POINTER_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _SbMemPointer SbMemPointer;

SbMemPointer* sb_mem_pointer_new      (gpointer            data,
				       GDestroyNotify      destroy);
gpointer      sb_mem_pointer_get_data (SbMemPointer const* self);
void          sb_mem_pointer_free     (SbMemPointer      * self);

G_END_DECLS

#endif /* !SB_MEM_POINTER_H */
