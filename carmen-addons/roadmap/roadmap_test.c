#include <carmen/carmen_graphics.h>
#include <assert.h>
#include <values.h>
#include <carmen/dot.h>
#include <carmen/dot_messages.h>
#include <carmen/dot_interface.h>

#include "roadmap.h"
#include "dynamics.h"

static carmen_roadmap_t *roadmap;
static carmen_world_point_t start;
static carmen_world_point_t person;
static carmen_world_point_t goal;

static void delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  widget = widget; event = event; data = data;
 
  gtk_main_quit();
  exit(0);
}

static int key_event(GtkWidget *widget __attribute__ ((unused)),
		     GdkEventKey *event, guint data __attribute__ ((unused)))
{
  if (toupper(event->keyval) == 'Q' && event->state & GDK_CONTROL_MASK) {
    gtk_main_quit();
    exit(0);
  }
  return 1;
}

static void draw_graph(GtkMapViewer *the_map_view) 
{
  int i, j;
  carmen_map_point_t m1, m2;
  carmen_world_point_t w1, w2;
  carmen_roadmap_vertex_t *n1;
  carmen_roadmap_vertex_t *n2;
  carmen_roadmap_edge_t edge;
  int num_vertices;
  carmen_map_point_t map_pt;
  carmen_world_point_t world_pt, next;
  GdkColor color;
  carmen_roadmap_vertex_t *node_list;
  double max_util, min_util, scale;

  node_list = (carmen_roadmap_vertex_t *)roadmap->nodes->list;
  max_util = -MAXFLOAT;
  min_util = MAXFLOAT;
  for (i = 0; i < roadmap->nodes->length; i++) {
    if (node_list[i].utility < MAXFLOAT/2) {
      if (node_list[i].utility < min_util)
	min_util = node_list[i].utility;
      if (node_list[i].utility > max_util)
	max_util = node_list[i].utility;
    } 
  }

  num_vertices = roadmap->nodes->length;
  for (i = 0; i < num_vertices; i++) {
    n1 = node_list+i;
    m1.x = n1->x;
    m1.y = n1->y;
    m1.map = the_map_view->internal_map;
    carmen_map_to_world(&m1, &w1);
    for (j = 0; 0 && j < n1->edges->length; j++) {
      edge = *((carmen_roadmap_edge_t *)carmen_list_get(n1->edges, j));
      assert (edge.cost != 0);
      n2 = node_list+edge.id;
      m2.x = n2->x;
      m2.y = n2->y;
      m2.map = m1.map;
      carmen_map_to_world(&m2, &w2);
      carmen_map_graphics_draw_line(the_map_view, &carmen_red, &w1, &w2);
    }
    w2 = w1;
    w1.pose.x -= .1;
    w1.pose.y += .1;
    w2.pose.x += .1;
    w2.pose.y -= .1;

    if (n1->utility < MAXFLOAT/2) {
      scale = 1-(n1->utility-min_util)/(max_util-min_util);
      color = carmen_graphics_add_color_rgb(255*scale, 0, 0);
    } else
      color = carmen_black;
    carmen_map_graphics_draw_line(the_map_view, &color, &w1, &w2);
    w1.pose.y -= .2;
    w2.pose.y += .2;
    carmen_map_graphics_draw_line(the_map_view, &color, &w1, &w2);

  }

  if (start.map == NULL) 
    return;

  n1 = carmen_roadmap_nearest_node(&start, roadmap);
  if (n1 == NULL)
    return;

  map_pt.x = n1->x;
  map_pt.y = n1->y;
  map_pt.map = start.map;
  carmen_map_to_world(&map_pt, &world_pt);
  carmen_map_graphics_draw_circle(the_map_view, &carmen_green, 0,
				  &world_pt, 1);
  
  if (person.map != NULL) {
    carmen_map_graphics_draw_circle(the_map_view, &carmen_blue, 1,
				    &person, 1);
    carmen_map_graphics_draw_circle(the_map_view, &carmen_black, 0,
				    &person, 1);
  }

  if (carmen_distance_world(&start, &world_pt) >= 2)
    return;

  //  carmen_warn("%f %f\n", start.pose.x, start.pose.y);

  //  carmen_warn("%d %d %f\n", n1->x, n1->y, n1->utility);

  if (carmen_dynamics_test_node(n1, 1)) 
    return;

  n2 = (carmen_roadmap_vertex_t *)
    carmen_list_get(roadmap->nodes, roadmap->goal_id);
  if (carmen_dynamics_test_node(n2, 1)) 
    return;

  do {
    n2 = carmen_roadmap_next_node(n1, roadmap);
    
    if (n2 == NULL)
      break;

    //    carmen_warn("%d %d %f\n", n2->x, n2->y, n2->utility);

    map_pt.x = n2->x;
    map_pt.y = n2->y;
    carmen_map_to_world(&map_pt, &next);
    carmen_map_graphics_draw_line(the_map_view, &carmen_green, &world_pt,
				  &next);	
    world_pt = next;
    n1 = n2;
  } while (n2->utility > 0);

}

static gint motion_handler (GtkMapViewer *the_map_view, 
			    carmen_world_point_p world_point,
			    GdkEventMotion *event __attribute__ ((unused)))
{
  //  world_point = world_point;
  start = *world_point;
  //  start.pose.x = 38.32;
  //  start.pose.y = 14.67;;
  carmen_map_graphics_redraw(the_map_view);
  
  return 1;
}

static int release_handler(GtkMapViewer *the_map_view, 
			   carmen_world_point_p world_point, 
			   GdkEventButton *event __attribute__ ((unused))) 
{
  carmen_dot_person_t dot_person;

  if (the_map_view->internal_map == NULL)
    return 1;

  if (event->state & GDK_CONTROL_MASK) {
    person = *world_point;
    dot_person.id = 0;
    dot_person.x = person.pose.x;
    dot_person.y = person.pose.y;    
    dot_person.vx = 0.375;
    dot_person.vy = 0.375;
    dot_person.vxy = 0.125;
    carmen_dynamics_update_person(&dot_person);
  } else {
    goal = *world_point;
  }

  carmen_roadmap_plan(roadmap, &goal);

  carmen_map_graphics_redraw(the_map_view);
  
  return 1;
}

static void initialize_graphics(int *argc, char **argv[], carmen_map_p map)
{
  GtkWidget *window;
  GtkWidget *main_box;
  GtkMapViewer *map_view;

  gtk_init (argc, argv);
  gdk_imlib_init();
 
  carmen_graphics_setup_colors();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      "WM destroy");
  gtk_window_set_title (GTK_WINDOW (window), "POMDP Viewer");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  
  main_box = gtk_vbox_new(FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (main_box), 0);
  gtk_container_add (GTK_CONTAINER (window), main_box);
  if (map->config.x_size > 500)
    map_view =
      carmen_map_graphics_new_viewer(map->config.x_size*2/3.0,
                                     map->config.y_size*2/3.0, 100.0);
  else
    map_view =
      carmen_map_graphics_new_viewer(map->config.x_size, map->config.y_size,
                                     40.0);
  gtk_signal_connect (GTK_OBJECT (window), "key_press_event",
                      (GtkSignalFunc) key_event, NULL);

  carmen_map_graphics_add_drawing_func(map_view, (drawing_func)draw_graph);
  carmen_map_graphics_add_motion_event
    (map_view, (GtkSignalFunc)motion_handler);
  carmen_map_graphics_add_button_release_event
    (map_view, (GtkSignalFunc)release_handler);

  gtk_box_pack_start(GTK_BOX (main_box), map_view->map_box, TRUE, TRUE, 0);
  carmen_map_graphics_add_map(map_view, map, 0);
  gtk_widget_show_all (window);
  gtk_widget_grab_focus(window);
}

int main(int argc, char *argv[]) 
{  
  carmen_map_p map;
  unsigned int seed;
  carmen_roadmap_vertex_t *n1, *n2;
  carmen_dot_person_t dot_person;

  carmen_initialize_ipc(argv[0]);
  seed = carmen_randomize(&argc, &argv);
  //  seed = 0xfe3acf4c;
  carmen_set_random_seed(seed);
  carmen_warn("Seed: %u\n", seed);

  map = (carmen_map_p)calloc(1, sizeof(carmen_map_t));
  carmen_test_alloc(map);
  carmen_map_get_gridmap(map);

  roadmap = carmen_roadmap_initialize(map);
  carmen_dynamics_initialize_no_ipc(map);

  if (1) {
    //  goal.pose.x = 11.4;
    //  goal.pose.y = 21.2;
    //    goal.pose.x = 18.6;
    //    goal.pose.y = 19.5;
    goal.pose.x = 25.4;
    goal.pose.y = 9.4;
    goal.map = map;
    
    dot_person.id = 0;
    dot_person.x = 14.5;
    dot_person.y = 22.8;
    dot_person.vx = 0.375;
    dot_person.vy = 0.375;
    dot_person.vxy = 0.125;

    person.pose.x = 14.5;
    person.pose.y = 22.8;
    person.map = map;
    carmen_dynamics_update_person(&dot_person);

    carmen_roadmap_plan(roadmap, &goal);
    
    memset(&start, 0, sizeof(carmen_world_point_t));
    
    if (1) {
      start.pose.x = 25;
      start.pose.y = 17;

      //      start.pose.x = 10.3;
      //      start.pose.y = 26.6;
      //      start.pose.x = 38.32;
      //      start.pose.y = 14.67;;
      
      start.map = map;
      n1 = carmen_roadmap_nearest_node(&start, roadmap);
      do {
	n2 = carmen_roadmap_next_node(n1, roadmap);
	n1 = n2;
      } while (n2 && n2->utility > 0);
    }  
  }

  initialize_graphics(&argc, &argv, map);
  gtk_main();

  return 0;
}

