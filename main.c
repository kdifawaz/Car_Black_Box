#include "main.h"
/*
Auther       :FAWAZ
Date         :17-03-2021
Description  :Black Blox
*/



static void init_config(void)
{
    init_clcd();
    init_uart();
    init_adc();
    init_matrix_keypad();
    init_i2c();
    init_ds1307();


    //testing 
    /*
    TRISB0 = 0;
    RB0 = 1;
*/
}

void main(void)
{
    /*configuering the peripherals*/
    init_config();

    
    while (1)
    {






    }
}

