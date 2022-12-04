#include <string.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_timer.h"
#include "HD44780.c"
#include "FSM.h"

#define LCD_ADDR 0x27
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2

#define GPIO_INPUT_IO_0 34
#define GPIO_INPUT_IO_1 35
#define GPIO_INPUT_IO_2 32
#define GPIO_INPUT_PIN_SEL ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1)| (1ULL<<GPIO_INPUT_IO_2))
esp_timer_create_args_t create_args;
esp_timer_handle_t timer_handle;

esp_timer_create_args_t create_args_1;
esp_timer_handle_t timer_handle_1;

esp_timer_create_args_t create_args_2;
esp_timer_handle_t timer_handle_2;

int button_sel;
int button_op;
int state_sel = 0;
int state_op = 0;
int flag = 0;
int timer_flag = 0;
int rapid_flag = 0;

float p_value = 0;
float i_value = 0;
float d_value = 0;
float out_val = 0; 

char *p_string = "P Value:";
char *i_string = "I Value:";
char *d_string = "D Value:";
char out_string[10];

void LCD_DemoTask(void* param)
{
    char num[20];
    while (true) {
        LCD_home();
	LCD_clearScreen();
//	sprintf(num, "%s", out_string);
        LCD_writeStr(out_string);
        vTaskDelay(100 / portTICK_RATE_MS);
        LCD_clearScreen();
        }
  
}
void timer_expired_1(void *p){
	flag = 1;

}

void timer_expired_2(void *p){
	rapid_flag = 1;

}

void timer_expired(void *p){
	button_sel = gpio_get_level(GPIO_INPUT_IO_0) ? 0:1;
	button_op = ((gpio_get_level(GPIO_INPUT_IO_1))? 0:(-1)) + ((gpio_get_level(GPIO_INPUT_IO_2))? 0:(1));
	fsm_deb_sel(button_sel, &state_sel, &flag, &state_op);
	fsm_deb_op(button_op, &state_op, &flag, &flag, &rapid_flag, &out_val);
	fsm_set(&state_sel, p_string, i_string, d_string, out_string, &out_val);

	if((state_sel == COUNT)||(state_op == COUNT)) esp_timer_start_once(timer_handle_1, 500000);
	else esp_timer_stop(timer_handle_1);

	if(state_sel == WAIT) esp_timer_start_periodic(timer_handle_2, 250000);
	else esp_timer_stop(timer_handle_2);
}


void app_main(void)
{
gpio_config_t io_conf = {};

io_conf.intr_type = GPIO_INTR_DISABLE;
io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
io_conf.mode = GPIO_MODE_INPUT;
io_conf.pull_up_en = 1;
io_conf.pull_down_en = 0;
gpio_config(&io_conf);

create_args.callback = timer_expired;
create_args.dispatch_method = ESP_TIMER_TASK;
create_args.name = "esp_timer";
esp_timer_create(&create_args, &timer_handle);

create_args_1.callback = timer_expired_1;
create_args_1.dispatch_method = ESP_TIMER_TASK;
create_args_1.name = "esp_timer_1";
esp_timer_create(&create_args_1, &timer_handle_1);

create_args_2.callback = timer_expired_2;
create_args_2.dispatch_method = ESP_TIMER_TASK;
create_args_2.name = "esp_timer_2";
esp_timer_create(&create_args_2, &timer_handle_2);

esp_timer_start_periodic(timer_handle , 1000);

    LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);
    xTaskCreate(LCD_DemoTask, "Demo Task", 2048, NULL, 5, NULL);
    
}

