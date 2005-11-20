#ifndef CARMEN_LASER_IPC_H
#define CARMEN_LASER_IPC_H

#include "sick.h"

void ipc_initialize_messages(void);

void publish_laser_alive(int front_stalled, int rear_stalled,
			 int laser3_stalled, int laser4_stalled);

void publish_laser_message(sick_laser_p laser);

#endif
