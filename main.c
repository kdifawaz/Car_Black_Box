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


/*Menu options*/

unsigned char menu_key;
void menu_key_operation(char* menu_key_flag);
Status password_check(void);
char menu_log[5][17] = {"    View Log    ","    Clear Log   ","    Set Time    "," Change Password","  Download Log  "};

/*password*/
char cursor = '#';
char password[] = "0000";
char entered_passwd[3];
int success = 1;
unsigned int retry_delay = 300;
static unsigned int blink_cursor = 0;
//static unsigned char i = 0;


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
    unsigned char menu_key = 0;
    unsigned char menu_key_flag = 0;
    unsigned char event_count = 0;

    while (1)
    {


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
	    /*Menu options*/
	    menu_key_operation(&menu_key_flag);




	}
    }

}




void menu_key_operation(char *menu_key_flag)
{
    char menu_scroll = 0;
    while(1)
    {
	menu_key = read_switches(STATE_CHANGE);
	switch(menu_key)
	{
	    case MK_SW11:
		{
		    if(*menu_key_flag == 0)
		    {
			if( password_check() )
			    *menu_key_flag += 1;

		    }

		    if(menu_scroll < 4)
		    {
			clcd_print(menu_log[menu_scroll],LINE1(0));
			clcd_print(menu_log[menu_scroll + 1],LINE2(0));
		    }

		    if(*menu_key_flag ==  1)
		    {
			clcd_putch('*',LINE1(0));
		    }
		    else if(*menu_key_flag != 1 && menu_scroll < 4)
		    {
			clcd_putch('*',LINE2(0));
			menu_scroll++;
		    }


		    if(*menu_key_flag < 4)
			*menu_key_flag += 1;
		    for(long int i = 6000;i--;);


		    break;
		}
	    case MK_SW12:
		{
		    if(menu_scroll > 0)
		    {
			clcd_print(menu_log[menu_scroll - 1],LINE1(0));
			clcd_print(menu_log[menu_scroll],LINE2(0));
		    }

		    if(*menu_key_flag ==  4)
		    {
			clcd_putch('*',LINE2(0));
		    }
		    else if (*menu_key_flag != 4 && menu_scroll > 0)
		    {
			clcd_putch('*',LINE1(0));
			menu_scroll--;
		    }


		    if(*menu_key_flag > 1)
			*menu_key_flag -= 1;
		    for(long int i = 6000;i--;);
		    break;
		}
	}
    }


}






Status password_check(void)
{


    int pass_count = 0;


    unsigned char key;
    unsigned char attempt = 4;
    unsigned char flag;
    unsigned int delay =  0;
    clcd_print(" ENTER PASSWORD ",LINE1(0));
    clcd_print("                       ",LINE2(0));

    while(1)
    {
	if(pass_count < 4)
	{
	    if(blink_cursor++ < 600 )
	    {
		clcd_putch(cursor,LINE2(pass_count));
	    }
	    else if (blink_cursor++ < 5000)
		clcd_putch(' ',LINE2(pass_count));
	    else
		blink_cursor = 0;

	    key = read_switches(STATE_CHANGE);
	    if(key != ALL_RELEASED)
	    {


		if(key == MK_SW11)
		{
		    entered_passwd[pass_count] = '1';
		    clcd_putch('*',LINE2(pass_count));
		    if(entered_passwd[pass_count] != password[pass_count])
			success = 0;
		    pass_count++;

		}
		if (key == MK_SW12)
		{
		    entered_passwd[pass_count] = '0';
		    clcd_putch('*',LINE2(pass_count));
		    if(entered_passwd[pass_count] != password[pass_count])
			success = 0;
		    pass_count++;
		}
	    }
	}

	if(pass_count == 4)
	{

	    if(success)
	    {
		clcd_print("    PASSWORD    ",LINE1(0));
		clcd_print("    VERIFIED    ",LINE2(0));

		return TRUE;
	    }
	    else
	    {

		clcd_print("Incorrect Psswrd",LINE1(0));
		clcd_print("      Retry     ",LINE2(0));
		for(unsigned long int i = 5000;i--;);
		flag = 1;
		cursor = ' ';


	    }
	    if(flag && (retry_delay-- == 0))
	    {
		flag = 0;
		clcd_print("ENTER PASSWORD",LINE1(0));
		clcd_print("               ",LINE2(0));
		retry_delay = 300;
		attempt--;
		cursor = '#';
		pass_count = 0;
		success = 1;
	    }
	    if (attempt == 1)
	    {
		clcd_print("Incorrect Psswrd",LINE1(0));
		clcd_print(" Access Denied  ",LINE2(0));
		cursor = ' ';

	    }
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
