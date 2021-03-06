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

#include "sb-reference.h"

struct _SbReferencePrivate {
	SbRevision* revision;
	gchar*      filename;
	guint       current_start;
	guint       current_end;
};

enum {
	PROP_0,
	PROP_CURRENT_START,
	PROP_CURRENT_END,
	PROP_REVISION
};

G_DEFINE_TYPE (SbReference, sb_reference, G_TYPE_OBJECT);

static void
sb_reference_init (SbReference* self)
{
	self->_private = G_TYPE_INSTANCE_GET_PRIVATE (self,
						      SB_TYPE_REFERENCE,
						      SbReferencePrivate);
}

static void
reference_finalize (GObject* object)
{
	SbReference* self = SB_REFERENCE (object);

	g_object_unref (self->_private->revision);
	g_free (self->_private->filename);

	G_OBJECT_CLASS (sb_reference_parent_class)->finalize (object);
}

static void
reference_get_property (GObject   * object,
			guint       prop_id,
			GValue    * value,
			GParamSpec* pspec)
{
	SbReference* self = SB_REFERENCE (object);

	switch (prop_id) {
	case PROP_CURRENT_START:
		g_value_set_uint (value, sb_reference_get_current_start (self));
		break;
	case PROP_CURRENT_END:
		g_value_set_uint (value, sb_reference_get_current_end (self));
		break;
	case PROP_REVISION:
		g_value_set_object (value, sb_reference_get_revision (self));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
reference_set_property (GObject     * object,
			guint         prop_id,
			GValue const* value,
			GParamSpec  * pspec)
{
	SbReference* self = SB_REFERENCE (object);

	switch (prop_id) {
	case PROP_CURRENT_START:
		self->_private->current_start = g_value_get_uint (value);
		g_object_notify (object, "current-start");
		break;
	case PROP_CURRENT_END:
		self->_private->current_end = g_value_get_uint (value);
		g_object_notify (object, "current-end");
		break;
	case PROP_REVISION:
		// FIXME: drop the cast from glib 2.14 onwards
		self->_private->revision = SB_REVISION (g_value_dup_object (value));
		g_object_notify (object, "revision");
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
sb_reference_class_init (SbReferenceClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);

	object_class->finalize     = reference_finalize;
	object_class->get_property = reference_get_property;
	object_class->set_property = reference_set_property;

	g_object_class_install_property (object_class, PROP_CURRENT_START,
					 g_param_spec_uint ("current-start", "current-start", "current-start",
							    0, G_MAXUINT, 0,
							    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class, PROP_CURRENT_END,
					 g_param_spec_uint ("current-end", "current-end", "current-end",
							    0, G_MAXUINT, 0,
							    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class, PROP_REVISION,
					 g_param_spec_object ("revision", "revision", "revision",
							      SB_TYPE_REVISION,
							      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_type_class_add_private (self_class, sizeof (SbReferencePrivate));
}

SbReference*
sb_reference_new (SbRevision* revision,
		  guint       current_start,
		  guint       current_end)
{
	return g_object_new (SB_TYPE_REFERENCE,
			     "current-start", current_start,
			     "current-end",   current_end,
			     "revision",      revision,
			     NULL);
}

guint
sb_reference_get_current_start (SbReference const* self)
{
	g_return_val_if_fail (SB_IS_REFERENCE (self), 0);

	return self->_private->current_start;
}

guint
sb_reference_get_current_end (SbReference const* self)
{
	g_return_val_if_fail (SB_IS_REFERENCE (self), 0);

	return self->_private->current_end;
}

gchar const*
sb_reference_get_filename (SbReference const* self)
{
	g_return_val_if_fail (SB_IS_REFERENCE (self), NULL);

	return self->_private->filename;
}

SbRevision*
sb_reference_get_revision (SbReference const* self)
{
	g_return_val_if_fail (SB_IS_REFERENCE (self), NULL);

	return self->_private->revision;
}

void
sb_reference_set_filename (SbReference* self,
			   gchar const* filename)
{
	g_return_if_fail (SB_IS_REFERENCE (self));

	if (self->_private->filename == filename) {
		return;
	}

	g_free (self->_private->filename);
	self->_private->filename = g_strdup (filename);

	// FIXME: make a GObject property
	// g_object_notify (G_OBJECT (self), "filename");
}

