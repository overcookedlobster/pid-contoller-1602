#ifndef FSM_H
#define FSM_H

#include <stdio.h>
#include <string.h>

#define STATE_P 0
#define STATE_D 1
#define STATE_I 2

#define STALL 0
#define COUNT 1
#define SHIFT 2
#define WAIT 3
#define RAPID 4

#define P_STRING "P Value:"
#define I_STRING "I Value:"
#define D_STRING "D Value:"
void fsm_debouncing_sel(int button_sel, int *state, int *flag, int *state_pid){
	switch(*state){
		case STALL:
			if(button_sel) *state = COUNT;
			break;	
		case COUNT:
			if(!button_sel) *state = STALL;
			else if(button_sel && (*flag)) *state = SHIFT;
			break;
		case SHIFT:
			*state_pid = (*state_pid+ 1)%3;
			*state = WAIT;
			*flag = 0;
			break;
		case WAIT:
			if(!button_sel) *state = STALL; 
			break;
	}	

}

void fsm_debouncing_op(int button_op, int *state, int *flag, int *timer_flag, int *rapid_flag, float* value){
	switch(*state){
		case STALL:
			if(button_op) *state = COUNT;
			break;	
		case COUNT:
			if(!button_op) *state = STALL;
			else if(button_op && (*flag)) *state = SHIFT;
			break;
		case SHIFT:
			*state = WAIT;
			*flag = 0;
			if(*value > 0) *value += (button_op * 0.1);
			else if (!(*value) && (button_op > 0)) *value += (button_op * 0.1); 
			break;
		case WAIT:
			if(!button_op) *state = STALL; 
			else if(*timer_flag) {
				*state = RAPID;
				*timer_flag = 0;
			}
			break;
		case RAPID:
			if(!button_op) *state = STALL;
			else if(*rapid_flag){
				if (*value > 0) *value += (button_op * 0.1);
				else if (!(*value) && (button_op > 0)) *value += (button_op * 0.1); 
				*rapid_flag = 0;
				*state = WAIT;
				}
			break;

	}	

}


void fsm_set(int *state_set, float *p_value, float *i_value, float *d_value, char *out_string, float* out_val){
	switch(*state_set){
		case STATE_P:
			sprintf(out_string, "%s, %.2f", P_STRING, *p_value);
		break;

		case STATE_D:
			sprintf(out_string, "%s, %.2f", D_STRING, *d_value);
		break;
		case STATE_I:
			sprintf(out_string, "%s, %.2f", I_STRING, *i_value);
		break;
}
}


#endif
