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
void event(char *gear_shift_key);
unsigned char event_count = 0;


/*Menu options*/

unsigned char menu_key;
void menu_key_operation(char* menu_key);
Status password_check(void);
char menu_log[5][17] = {"    View Log    ","    Clear Log   ","    Set Time    "," Change Password","  Download Log  "};

/*password*/
char cursor = '#';
char password[] = "0000";
char entered_passwd[3];
int success = 1;
unsigned int retry_delay = 300;
static unsigned int blink_cursor = 0;

/*event storing*/

void store_event(char time[],char current_event[],char speed[]);
char storage[10][17];
char storage_index = 0;

/*display event*/
void display_event(void);

/*view log*/

void view_log(void);

/*change password*/

void change_psswd(void);

/*downloag log*/
void download_log(void);

static void init_config(void)
{
    init_clcd();
    init_uart();
    init_adc();
    init_matrix_keypad();
    init_i2c();
    init_ds1307();


    //testing 
}

void main(void)
{
    /*configuering the peripherals*/
    init_config();

    unsigned char key_pressed = 0;
    unsigned char menu_key_flag = 0;



    while (1)
    {


	clcd_print("  TIME   EVNT SP",LINE1(0));
	while (1)
	{

	    clcd_print("  TIME   EVNT SP",LINE1(0));
	    /*displaying real time*/
	    get_time();
	    display_time();

	    /*displaying the event*/
	    event(&key_pressed);
	    clcd_print(gear_state[event_count],LINE2(10));

	    /*diplaying the speed*/
	    current_speed();
	    display_event();

	    /*Menu options*/
	    if((key_pressed = read_switches(STATE_CHANGE)) == MK_SW11)
		menu_key_operation(&key_pressed);



	}
    }

}

void display_event(void)
{
}

void download_log()
{
    for(int i = 0;i < 10;i++ )
    {
	puts(storage[i]);
	putch('\n');
	putch('\r');
    }
}

void change_psswd(void)
{
    char key;
    char pass_count = 0;
    while(1)
    {
	key = read_switches(STATE_CHANGE);

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

	    if(key == MK_SW11)
	    {
		write_internal_eeprom(pass_count, '1');
		pass_count++;

	    }
	    else if (key == MK_SW12)
	    {
		write_internal_eeprom(pass_count, '0');
		pass_count++;

	    }




	}
	else
	    return;

    }


}



void store_event(char time[],char current_event[],char speed[])
{
    int count = 0;



    for(int i = 0;time[i] != '\0';i++)
	storage[storage_index][count++] = time[i];
    storage[storage_index][count++] = ' ';

    for(int i = 0;current_event[i] != '\0';i++)
	storage[storage_index][count++] = current_event[i];
    storage[storage_index][count++] = ' ';

    for(int i = 0;speed[i] != '\0';i++)
	storage[storage_index][count++] = speed[i];
    storage[storage_index][count++] = '\0';


    if(storage_index < 10)
	storage_index++;

}

void event(char *gear_shift_key)
{
    if(storage_index == 0)
	store_event(time,gear_state[event_count],speed);


    switch (*gear_shift_key)
    {
	/*incrementing the gears for MK_SW2*/
	case MK_SW2:
	    {
		if(event_count < 5)
		{
		    event_count = event_count + 1;
	store_event(time,gear_state[event_count],speed);
		}
		break;
	    }
	    /*decrementing the gears for MK_SW3*/
	case MK_SW3:
	    {
		if(event_count > 0)
		{
		    event_count = event_count - 1;
	store_event(time,gear_state[event_count],speed);
		}
		break;
	    }
    }
}


/*function for the viewing the log*/
void view_log(void)
{
    char key;
    char back_key;
    long int back_key_delay = 500;
    char log_count = 0;


    /*clearing the screen and printing the 0 th log*/

    clcd_print("                                      ",LINE1(0));
    clcd_print("LOGs",LINE1(6));
    clcd_print("                                      ",LINE2(0));
    clcd_print(storage[log_count],LINE2(0));
	    clcd_putch('0' + log_count,LINE2(15));


    key = MK_SW11;
    while(1)
    {
	back_key = read_switches(LEVEL_CHANGE);
	key = read_switches(STATE_CHANGE);
	if(back_key_delay-- == 0)
	{
	    back_key_delay = 500;
	    /*back to previous menu if mk 12 is long pressed*/
	    if(back_key == MK_SW12)
		return;

	}




	if(log_count < storage_index - 1 && key == MK_SW11)
	{

	    log_count++;
	    clcd_print(storage[log_count],LINE2(0));
	    clcd_putch('0' + log_count,LINE2(15));
	}
	else if (log_count  > 0 && key == MK_SW12)
	{

	    log_count--;
	    clcd_print(storage[log_count],LINE2(0));
	    clcd_putch('0' + log_count,LINE2(15));
	}




    }
}

void menu_key_operation(char* menu_key)
{
    char menu_index = 0;
    char menu_scroll = 0;
    char menu_key_long_press;
    long int menu_key_long_press_delay = 70000;

    {
	while(1)
	{
	    /*reading the menu option switches*/
	    *menu_key = read_switches(STATE_CHANGE);
	    menu_key_long_press = read_switches(LEVEL_CHANGE);

	    /*Long press logic for going forward and backward in menu*/
	    if(menu_index == 1 && menu_key_long_press == MK_SW11)
	    {

		if(menu_key_long_press_delay-- == 0)
		{
		    menu_key_long_press_delay = 70000;
		    if((menu_key_long_press == MK_SW11))
		    {
			view_log();
			menu_index = 1;
		    }
		    else if(menu_key_long_press == MK_SW12)
		    {
			clcd_print("                              ",LINE2(0));
			menu_index = 0;
			return;
		    }
		}
	    }

	    /*Logic for scroling the menu with edge trigering*/
	    switch(*menu_key)
	    {
		case MK_SW11:
		    {
			if(menu_index == 0)
			{
			    if( password_check() )
				menu_index += 1;
			}
			if(menu_scroll < 4)
			{
			    clcd_print(menu_log[menu_scroll],LINE1(0));
			    clcd_print(menu_log[menu_scroll + 1],LINE2(0));
			}

			if(menu_index ==  1)
			{
			    clcd_putch('*',LINE1(0));

			    if((menu_key_long_press = read_switches(LEVEL_CHANGE))!= MK_SW11)
				menu_index += 1;


			}
			else if(menu_scroll < 4 )
			{
			    clcd_putch('*',LINE2(0));
			    menu_scroll++;
			    if(menu_index < 4 && menu_key_long_press != MK_SW11)
				menu_index += 1;
			}


			for(long int i = 7000;i--;);


			break;
		    }
		case MK_SW12:
		    {
			if(menu_scroll > 0)
			{
			    clcd_print(menu_log[menu_scroll - 1],LINE1(0));
			    clcd_print(menu_log[menu_scroll],LINE2(0));
			}

			if(menu_index ==  4)
			{
			    clcd_putch('*',LINE2(0));
			}
			else if (menu_index != 4 && menu_scroll > 0)
			{
			    clcd_putch('*',LINE1(0));
			    menu_scroll--;
			}


			if(menu_index > 1)
			    menu_index -= 1;
			for(long int i = 7000;i--;);
			break;
		    }

	    }
	}

    }

}

/*Checking the password*/
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

	    /*Reading the password and checking it*/
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

	    /*returning the true status if the entered password is correct*/
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
		for(unsigned long int i = 3000;i--;);
		flag = 1;
		cursor = ' ';


	    }
	    if(flag && (retry_delay-- == 0))
	    {
		flag = 0;
		clcd_print(" ENTER PASSWORD ",LINE1(0));
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
		/////////////////////////////
		while(1);

	    }
	}


    }

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
/*dispalying the time on clcd*/
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
/*Getting the time from Rtc*/
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
