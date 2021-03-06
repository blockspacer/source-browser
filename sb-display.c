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

#include <gfc/gfc-job.h>
#include <gfc/gfc-spawn-screen.h>

#include "sb-annotations.h"
#include "sb-callback-data.h"
#include "sb-comparable.h"
#include "sb-marshallers.h"
#include "sb-reference.h"
#include "sb-settings.h"

struct _SbDisplayPrivate {
	SbAnnotations* annotations;
	GtkTextView  * text_view;

	/* these two are ours */
	GtkAdjustment* horizontal;
	GtkAdjustment* vertical;

	/* the two are for the annotation */
	GtkAdjustment* anno_horizontal;
	GtkAdjustment* anno_vertical;

	/* the following are only valid during history loading */
	// FIXME: move them into an SbHistoryLoader
	GList        * references;
	GfcReader    * reader;
	GHashTable   * revisions;
	SbRevision   * revision; // FIXME: drop this one
	SbReference  * reference;
};

enum {
	LOAD_STARTED,
	LOAD_PROGRESS,
	LOAD_DONE,
	// FIXME: LOAD_CANCELLED,
	N_SIGNALS
};

static guint signals[N_SIGNALS] = {0};

G_DEFINE_TYPE (SbDisplay, sb_display, GTK_TYPE_HBOX);

static void
sb_display_init (SbDisplay* self)
{
	GtkWidget* widget;

	self->_private = G_TYPE_INSTANCE_GET_PRIVATE (self,
						      SB_TYPE_DISPLAY,
						      SbDisplayPrivate);

	gtk_box_set_spacing (GTK_BOX (self), 6);

	widget = sb_annotations_new ();
	gtk_widget_show (widget);
	gtk_box_pack_start (GTK_BOX (self),
			    widget,
			    FALSE,
			    FALSE,
			    0);
	self->_private->annotations = SB_ANNOTATIONS (widget);

	widget = gtk_text_view_new ();
	gtk_widget_show (widget);
	gtk_box_pack_start_defaults (GTK_BOX (self),
				     widget);
	self->_private->text_view = GTK_TEXT_VIEW (widget);
	/* FIXME: update the annotations' layout when the allocation of the
	 * text view changes
	 */

	sb_annotations_set_text_view (self->_private->annotations,
				      self->_private->text_view);

	self->_private->revisions = g_hash_table_new_full ((GHashFunc)sb_comparable_hash,
							   (GEqualFunc)sb_comparable_equals,
							   NULL,
							   g_object_unref);
}

static void
display_finalize (GObject* object)
{
	SbDisplay* self = SB_DISPLAY (object);

	// FIXME: g_warn_if_fail (!self->_private->horizontal)
	// FIXME: g_warn_if_fail (!self->_private->vertical)
	g_hash_table_destroy (self->_private->revisions);

	G_OBJECT_CLASS (sb_display_parent_class)->finalize (object);
}

static void
update_range (GtkAdjustment* master,
	      SbDisplay    * self)
{
	gtk_layout_set_size (GTK_LAYOUT (self->_private->annotations),
			     100, // FIXME: get the 100 scalable in some way
			     master->upper - master->lower);
}

static void
update_value (GtkAdjustment* master,
	      SbDisplay    * self)
{
	gtk_adjustment_set_value (self->_private->anno_vertical, master->value);
}

static void
display_set_scroll_adjustments (SbDisplay    * self,
				GtkAdjustment* horizontal,
				GtkAdjustment* vertical)
{
	if (self->_private->horizontal) {
		self->_private->anno_horizontal = NULL;
		g_object_unref (self->_private->horizontal);
		self->_private->horizontal = NULL;
	}

	if (horizontal) {
		self->_private->horizontal = g_object_ref_sink (horizontal);
		self->_private->anno_horizontal = g_object_new (GTK_TYPE_ADJUSTMENT, NULL);
	}

	if (self->_private->vertical) {
		g_signal_handlers_disconnect_by_func (self->_private->vertical, update_range, self);
		g_signal_handlers_disconnect_by_func (self->_private->vertical, update_value, self);
		self->_private->anno_vertical = NULL;
		g_object_unref (self->_private->vertical);
		self->_private->vertical = NULL;
	}

	if (vertical) {
		self->_private->vertical = g_object_ref_sink (vertical);
		self->_private->anno_vertical = g_object_new (GTK_TYPE_ADJUSTMENT, NULL);
		g_signal_connect (self->_private->vertical, "changed",
				  G_CALLBACK (update_range), self);
		g_signal_connect (self->_private->vertical, "value-changed",
				  G_CALLBACK (update_value), self);
	}

	// FIXME: the display should have its own pair of scroll adjustments and sync the other two
	gtk_widget_set_scroll_adjustments (GTK_WIDGET (self->_private->annotations),
					   self->_private->anno_horizontal,
					   self->_private->anno_vertical);
	gtk_widget_set_scroll_adjustments (GTK_WIDGET (self->_private->text_view),
					   horizontal,
					   vertical);
}

static void
sb_display_class_init (SbDisplayClass* self_class)
{
	GObjectClass* object_class = G_OBJECT_CLASS (self_class);
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (self_class);

	object_class->finalize             = display_finalize;

	self_class->set_scroll_adjustments = display_set_scroll_adjustments;

	g_type_class_add_private (self_class, sizeof (SbDisplayPrivate));

	widget_class->set_scroll_adjustments_signal =
				g_signal_new ("set-scroll-adjustments",
					      SB_TYPE_DISPLAY,
					      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (SbDisplayClass, set_scroll_adjustments),
					      NULL, NULL,
					      sb_cclosure_marshal_VOID__BOXED_BOXED,
					      G_TYPE_NONE, 2,
					      GTK_TYPE_ADJUSTMENT,
					      GTK_TYPE_ADJUSTMENT);

	signals[LOAD_STARTED] = g_signal_new ("load-started",
					      SB_TYPE_DISPLAY,
					      0, 0,
					      NULL, NULL,
					      g_cclosure_marshal_VOID__VOID,
					      G_TYPE_NONE, 0);
	signals[LOAD_PROGRESS] = g_signal_new ("load-progress",
					       SB_TYPE_DISPLAY,
					       0, 0,
					       NULL, NULL,
					       g_cclosure_marshal_VOID__INT,
					       G_TYPE_NONE, 1,
					       G_TYPE_INT);
	signals[LOAD_DONE] = g_signal_new     ("load-done",
					       SB_TYPE_DISPLAY,
					       0, 0,
					       NULL, NULL,
					       g_cclosure_marshal_VOID__VOID,
					       G_TYPE_NONE, 0);
}

GtkWidget*
sb_display_new (void)
{
	return g_object_new (SB_TYPE_DISPLAY, NULL);
}

gint
sb_display_get_n_lines (SbDisplay const* self)
{
	g_return_val_if_fail (SB_IS_DISPLAY (self), 0);

	return gtk_text_buffer_get_line_count (gtk_text_view_get_buffer (self->_private->text_view));
}

static gint
sort_refs_by_target_line (gconstpointer a,
			  gconstpointer b)
{
	return sb_reference_get_current_start (a) - sb_reference_get_current_start (b);
}

static void
display_parse_line (GfcReader  * reader,
		    gchar const* line,
		    SbDisplay  * self)
{
#undef DEBUG_DISPLAY
	g_return_if_fail (SB_IS_DISPLAY (self));

	// FIXME: make sure we have a new hash table each time
	if (G_UNLIKELY (!self->_private->revision)) {
		/* from git-annotate (1)
		 * "<40-byte hex sha1> <sourceline> <resultline> <num_lines>"
		 */
		gchar     **words    = g_strsplit (line, " ", -1);
		SbRevision* revision = sb_revision_new (words[0]);
		gint        n_lines  = atoi (words[3]);

		self->_private->revision = g_hash_table_lookup (self->_private->revisions,
								revision);

		if (!self->_private->revision) {
			self->_private->revision = g_object_ref (revision);
			// FIXME: create an API for SbHashedVector
			g_hash_table_insert (self->_private->revisions,
					     self->_private->revision,
					     self->_private->revision);
		}

		self->_private->reference = sb_reference_new (self->_private->revision,
							      atoi (words[2]),
							      atoi (words[2]) + n_lines - 1);

		g_signal_emit (self,
			       signals[LOAD_PROGRESS],
			       0,
			       n_lines);

		g_object_unref (revision);
		g_strfreev (words);
	} else if (g_str_has_prefix (line, "filename ")) {
		gchar** vector = g_strsplit (line, " ", 2);
#ifdef DEBUG_DISPLAY
		g_print ("%s\n", sb_revision_get_name (self->_private->revision));
#endif
		self->_private->revision = NULL;
		sb_reference_set_filename (self->_private->reference,
					   vector[1]);
		// FIXME: use a GSequence for the revisions (maybe that would make the API nicer too)
		self->_private->references = g_list_insert_sorted (self->_private->references,
								   g_object_ref (self->_private->reference),
								   sort_refs_by_target_line);
		g_object_unref (self->_private->reference);
		self->_private->reference = NULL;
		g_strfreev (vector);
	} else if (g_str_has_prefix (line, "summary ")) {
		gchar** vector = g_strsplit (line, " ", 2);
		sb_revision_set_summary (self->_private->revision,
					 vector[1]);
		g_strfreev (vector);
#ifdef DEBUG_DISPLAY
	} else {
		// FIXME: meta-information about the commit
		g_debug ("Got unknown line: %s",
			 line);
#endif
	}
}

static void
job_done_cb (SbDisplay* self,
	     GfcJob   * job)
{
	gfc_reader_flush (self->_private->reader);

	sb_annotations_set_references (self->_private->annotations,
				       self->_private->references);
	self->_private->references = NULL;

	g_object_unref (self->_private->reader);
	self->_private->reader = NULL;
	g_object_unref (job);

	g_signal_emit (self,
		       signals[LOAD_DONE],
		       0);
}

static inline void // FIXME: rename function
load_history (SbDisplay  * self,
	      gchar const* file_path)
{
	gchar* working_folder;
	gchar* basename;
	GPtrArray* array;
	GfcJob* job;
	GPid pid = 0;
	gint out_fd = 0;

	g_return_if_fail (!self->_private->reader); // protect against multiple execution

	g_list_foreach (self->_private->references, (GFunc)g_object_unref, NULL);
	g_list_free    (self->_private->references);
	self->_private->references = NULL;

	array = g_ptr_array_sized_new (1   /* command */
				       + 1 /* --incremental */
				       + 1 /* -M (?) */
				       + 1 /* -C (?) */
				       + 1 /* basename */
				       + 1 /* ignore whitespaces */
				       + 1 /* NULL */);
	g_ptr_array_add (array, "git-blame");
	g_ptr_array_add (array, "--incremental");

	if (sb_settings_get_follow_moves ()) {
		g_ptr_array_add (array, "-M");
	}
	if (sb_settings_get_follow_copies ()) {
		g_ptr_array_add (array, "-C");
	}
	if (sb_settings_get_ignore_whitespaces ()) {
		g_ptr_array_add (array, "-w");
	}

	working_folder = g_path_get_dirname (file_path);
	basename = g_path_get_basename (file_path);
	g_ptr_array_add (array, basename);
	g_ptr_array_add (array, NULL);

	job = gfc_job_new_full (working_folder,
				(gchar const**)array->pdata,
				gfc_spawn_screen_new (gtk_widget_get_screen (GTK_WIDGET (self))));
	self->_private->reader = g_object_ref (gfc_job_get_out_reader (job));
	g_signal_connect_swapped (job, "done",
				  G_CALLBACK (job_done_cb), self);

	g_free (basename);
	g_free (working_folder);

	g_signal_connect (self->_private->reader, "read-line",
			  G_CALLBACK (display_parse_line), self);
}

void
sb_display_load_path (SbDisplay  * self,
		      gchar const* path,
		      GError     **error)
{
	GtkTextBuffer* buffer;
	GMappedFile  * file;

	file = g_mapped_file_new (path, FALSE, error);
	buffer = gtk_text_view_get_buffer (self->_private->text_view);

	gtk_text_buffer_set_text (buffer,
				  g_mapped_file_get_contents (file),
				  g_mapped_file_get_length (file));

	g_signal_emit (self, signals[LOAD_STARTED], 0);

	g_mapped_file_free (file);

	load_history (self,
		      path);

	// FIXME: disable loading of new files until the history is loaded
	// FIXME: make history loading cancellable
}

