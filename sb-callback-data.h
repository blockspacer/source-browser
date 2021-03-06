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

#ifndef SB_CALLBACK_DATA_H
#define SB_CALLBACK_DATA_H

#include <glib.h>

G_BEGIN_DECLS

typedef gpointer SbCallbackData;

SbCallbackData* sb_callback_data_new  (gchar const         * name,
				       ...);
gpointer        sb_callback_data_peek (SbCallbackData const* self,
				       gchar const         * name);
void            sb_callback_data_free (gpointer              self); // self as SbCallbackData

G_END_DECLS

#endif /* !SB_CALLBACK_DATA_H */
