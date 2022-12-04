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
void fsm_deb_sel(int button, int *state, int *flag, int *state_pid){
	switch(*state){
		case STALL:
			if(button) *state = COUNT;
			break;	
		case COUNT:
			if(!button) *state = STALL;
			else if(button && (*flag)) *state = SHIFT;
			break;
		case SHIFT:
			*state_pid = (*state_pid+ 1)%3;
			*state = WAIT;
			*flag = 0;
			break;
		case WAIT:
			if(!button) *state = STALL; 
			break;
	}	

}

void fsm_deb_op(int button, int *state, int *flag, int *timer_flag, int *rapid_flag, float* value){
	switch(*state){
		case STALL:
			if(button) *state = COUNT;
			break;	
		case COUNT:
			if(!button) *state = STALL;
			else if(button && (*flag)) *state = SHIFT;
			break;
		case SHIFT:
			*state = WAIT;
			*flag = 0;
			*value += (button * 0.1);
			break;
		case WAIT:
			if(!button) *state = STALL; 
			else if(timer_flag) {
				*state = RAPID;
				*timer_flag = 0;
			}
			break;
		case RAPID:
			if(!button) *state = STALL;
			else if(*rapid_flag){
				*value += (button * 0.1);
				*rapid_flag = 0;
				}
			break;

	}	

}


void fsm_set(int *state_set, char *p_string, char *i_string, char *d_string, char *out_string, float* out_val){
	switch(*state_set){
		case STATE_P:
			sprintf(out_string, "%s, %.2f", p_string, *out_val);
		break;

		case STATE_D:
			sprintf(out_string, "%s, %.2f", d_string, *out_val);
		break;
		case STATE_I:
			sprintf(out_string, "%s, %.2f", i_string, *out_val);
		break;
}
}


#endif
