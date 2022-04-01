#ifndef MAIN_H
#define MAIN_H
#include"pic_Specific.h"
#include "clcd.h"
#include "uart.h"
#include "adc.h"
#include "eeprom.h"
#include "matrix_keypad.h"
#include "ds1307.h"
#include "i2c.h"


/*POT(speed) related variables*/
void current_speed(void);

/*Event(gear) related variables*/
void event(char *gear_shift_key);

/*Menu options*/
void menu_key_operation(char* menu_key);
Status password_check(void);

/*event storing*/

void store_event(char time[],char current_event[],char speed[]);

/*display event*/
void display_event(void);

/*view log*/

void view_log(void);

/*change password*/
void change_psswd(void);

/*downloag log*/
void download_log(void);


/*claer log*/
void clear_log(void);


/*set time*/
void set_time();


typedef enum
{
    FALSE,
    TRUE
}Status;

#endif
