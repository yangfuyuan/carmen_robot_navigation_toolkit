/*********************************************************
 *
 * This source code is part of the Carnegie Mellon Robot
 * Navigation Toolkit (CARMEN)
 *
 * CARMEN Copyright (c) 2002 Michael Montemerlo, Nicholas
 * Roy, and Sebastian Thrun
 *
 * CARMEN is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public 
 * License as published by the Free Software Foundation; 
 * either version 2 of the License, or (at your option)
 * any later version.
 *
 * CARMEN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General 
 * Public License along with CARMEN; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, 
 * Suite 330, Boston, MA  02111-1307 USA
 *
 ********************************************************/

#ifndef GLOBAL_GRAPHICS_H
#define GLOBAL_GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>
#include <gdk_imlib.h>
#include <carmen/carmen.h>

#define CARMEN_GRAPHICS_INVERT          1
#define CARMEN_GRAPHICS_RESCALE         2
#define CARMEN_GRAPHICS_ROTATE          4
#define CARMEN_GRAPHICS_BLACK_AND_WHITE 8

  typedef struct {
    int width;
    int height;
    double zoom;
  } carmen_graphics_screen_config_t, *carmen_graphics_screen_config_p;

  typedef struct {
    int x;
    int y;
    carmen_graphics_screen_config_p config;
  } carmen_graphics_screen_point_t, *carmen_graphics_screen_point_p;

  typedef struct {
    int fd;
    int callback_id;
    int ok;
  } carmen_graphics_callback;

  extern GdkColor carmen_red, carmen_blue, carmen_white, carmen_yellow, 
    carmen_green, carmen_light_blue, carmen_black, carmen_orange, 
    carmen_grey, carmen_light_grey, carmen_purple;

  int carmen_graphics_trajectory_to_screen(carmen_traj_point_p traj_point, 
					   carmen_graphics_screen_point_p screen_point, 
					   carmen_graphics_screen_config_p screen);

  int carmen_graphics_screen_to_trajectory(carmen_graphics_screen_point_p screen_point, 
					   carmen_traj_point_p traj_point);

  int carmen_graphics_map_to_screen(carmen_map_point_p map_point, 
				    carmen_graphics_screen_point_p screen_point, 
				    carmen_graphics_screen_config_p screen);

  int carmen_graphics_screen_to_map(carmen_graphics_screen_point_p screen_point, 
				    carmen_map_point_p map_point, carmen_map_p map);

  int carmen_graphics_screen_to_map_new(carmen_graphics_screen_point_p screen_point, 
					carmen_map_point_p map_point, carmen_map_p map);

  int carmen_graphics_world_to_screen(carmen_world_point_p world_point, 
				      carmen_graphics_screen_point_p screen_point, 
				      carmen_graphics_screen_config_p screen);

  int carmen_graphics_screen_to_world(carmen_graphics_screen_point_p screen_point, 
				      carmen_world_point_p map_point, carmen_map_p map);

  void carmen_graphics_verify_list_length(carmen_graphics_callback **list, 
					  int *list_length, 
					  int *list_capacity);
  void carmen_graphics_update_ipc_callbacks(GdkInputFunction callback_Func);
  unsigned char *carmen_graphics_convert_to_image(carmen_map_p map, int flags);
  GdkPixmap * carmen_graphics_generate_pixmap(GtkWidget* drawing_area, 
					      unsigned char* image_data,
					      carmen_map_config_p config, 
					      double zoom);
  void carmen_graphics_initialize_screenshot(void);
  void carmen_graphics_save_pixmap_screenshot(char *basefilename, 
					      GdkPixmap *pixmap, 
					      int x, int y, int w, int h);

  GdkColor carmen_graphics_add_color(char *name);
  void carmen_graphics_setup_colors(void);
  GdkColor carmen_graphics_add_color_rgb(int r, int g, int b);

#ifndef NO_JPEG
  void carmen_graphics_write_map_as_jpeg(char *filename, carmen_map_p map, 
					 int flags);
#endif

  void carmen_graphics_write_pixmap_as_png(GdkPixmap *pixmap, 
					   char *user_filename,
					   int x, int y, int w, int h);
  void carmen_graphics_write_data_as_png(unsigned char *data, 
					 char *user_filename, int w, int h);

  void carmen_graphics_draw_ellipse(GdkPixmap *pixmap, GdkGC *GC, double x, 
				    double y, double a, double b, double c, 
				    double k);

  #include <carmen/map_graphics.h>


#ifdef __cplusplus
}
#endif

#endif
