/***********************************************************************
History
15 Jul 2008 -- Zhang Dong @1
- Create.
************************************************************************************/
#pragma once

typedef enum {SYS_ONLINE, SYS_OFFLINE};
typedef enum {MOTOR_X, MOTOR_Y, MOTOR_Z};
typedef enum {DUMMY_PGM_VERTICAL, DUMMY_PGM_HORIZONTAL, DUMMY_PGM_CIRCLE, DUMMY_PGM_SQUARE};
typedef enum {PARA_TYPE_LOOP_GROUP, PARA_TYPE_BASE_TIME};
#define round(x) (int)(x + 0.5)

#define PI 3.1415926536