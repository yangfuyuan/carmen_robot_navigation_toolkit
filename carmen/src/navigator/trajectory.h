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

#ifndef CARMEN_TRAJECTORY_H
#define CARMEN_TRAJECTORY_H

#ifdef __cplusplus
extern "C" {
#endif
  
  int carmen_planner_util_add_path_point(carmen_traj_point_t point, 
					 carmen_planner_path_p path);
  
  carmen_traj_point_p carmen_planner_util_get_path_point(int index, 
							 carmen_planner_path_p path);
  void carmen_planner_util_set_path_point(int index, 
					  carmen_traj_point_p path_point,
					  carmen_planner_path_p path);
  void carmen_planner_util_insert_blank(int index, carmen_planner_path_p path);
  void carmen_planner_util_insert_path_point(int index, 
					     carmen_traj_point_t *current_point, 
					     carmen_planner_path_p path);
  void carmen_planner_util_set_path_velocities(int index, double t_vel, 
					       double r_vel, 
					       carmen_planner_path_p path);
  void carmen_planner_util_clear_path(carmen_planner_path_p path);
  void carmen_planner_util_clip_path(int length, carmen_planner_path_p path);
  void carmen_planner_util_delete_path_point(int index, carmen_planner_path_p path);
  void carmen_planner_util_test_trajectory(carmen_planner_path_p path);
  
#ifdef __cplusplus
}
#endif

#endif