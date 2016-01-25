#ifndef DJI_SAMPLE_H_
#define DJI_SAMPLE_H_
#include "pthread.h"
#include "DJI_LIB/DJI_Pro_App.h"
#include "unistd.h"

static int atti_ctrl_sample_flag = -1;

int DJI_Sample_Atti_Ctrl(void);

#endif
