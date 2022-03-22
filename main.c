#include "main.h"
/*
Auther       :FAWAZ
Date         :17-03-2021
Description  :Black Blox
*/

/*RTC related variables*/
unsigned char clock_reg[3];
unsigned char time[9];


/*POT(speed) related variables*/
void current_speed(void);
char speed[3];

/*Event(gear) related variables*/
char gear_state[6][3] = {"ON","GN","G1","G2","G3","G4"};
void event(unsigned char *event_count);


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


	unsigned char event_count = 0;
	clcd_print("  TIME   EVNT SP",LINE1(0));
	while (1)
	{

	    /*displaying real time*/
	    get_time();
	    display_time();

	    /*displaying the event*/
	    event(&event_count);

	    /*diplaying the speed*/
	    current_speed();





	}
    }

}

    void event(unsigned char *event_count)
    {
	unsigned char gear_shift_key = 0xff;
	/*detecting the gear shifts*/
	gear_shift_key = read_switches(STATE_CHANGE);

	switch (gear_shift_key)
	{
	    /*incrementing the gears for MK_SW2*/
	    case MK_SW2:
		{
		    if(*event_count < 5)
			*event_count = *event_count + 1;
		    break;
		}
		/*decrementing the gears for MK_SW3*/
	    case MK_SW3:
		{
		    if(*event_count > 0)
			*event_count = *event_count - 1;
		    break;
		}
	}

	clcd_print(gear_state[*event_count],LINE2(10));

    }


    void current_speed(void)
    {
	unsigned char i; 
	unsigned short pot_val;
	pot_val = read_adc(CHANNEL4);
	i = 1;
	do
	{
	    speed[i] = (pot_val % 10) + '0';
	    pot_val = pot_val / 10;
	} while (i--);
	speed[2] = '\0';
	/*Printing the speed on clcd*/
	clcd_print(speed, LINE2(14));

    }
    void display_time(void)
    {
	clcd_print(time, LINE2(0));

	if (clock_reg[0] & 0x40)
	{
	    if (clock_reg[0] & 0x20)
	    {
		clcd_print("PM", LINE2(12));
	    }
	    else
	    {
		clcd_print("AM", LINE2(12));
	    }
	}
    }

    void get_time(void)
    {
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	if (clock_reg[0] & 0x40)
	{
	    time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
	    time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	else
	{
	    time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
	    time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
    }
