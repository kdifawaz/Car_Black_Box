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
char gear_state[11][3] = {"ON","GN","G1","G2","G3","G4","CL","CP","DL","ST","CO"};
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


/*claer log*/
void clear_log(void);


/*set time*/
void set_time();






static void init_config(void)
{
    init_clcd();
    init_uart();
    init_adc();
    init_matrix_keypad();
    init_i2c();
    init_ds1307();


    write_internal_eeprom(0, '0');
    write_internal_eeprom(1, '0');
    write_internal_eeprom(2, '0');
    write_internal_eeprom(3, '0');
    //testing 
}

void main(void)
{
    /*configuering the peripherals*/
    init_config();

    unsigned char key_pressed = 0;
    unsigned char menu_key_flag = 0;

    RB0 = 0;

    while (1)
    {


	clcd_print("  TIME   EVNT SP",LINE1(0));
	while (1)
	{


	    clcd_print("  TIME   EVNT SP",LINE1(0));
	    /*displaying real time*/
	    get_time();
	    display_time();


	    /*diplaying the speed*/
	    current_speed();

	    /*displaying the event*/
	    event(&key_pressed);
	    clcd_print(gear_state[event_count],LINE2(10));


	    /*Menu options*/
	    if((key_pressed = read_switches(STATE_CHANGE)) == MK_SW11)
		menu_key_operation(&key_pressed);



	}
    }

}

void set_time()
{

    unsigned char dummy;
    union
    {
	unsigned char dummy;
	struct
	{
	    char lower 	: 4;
	    char higher 	: 4;

	}nibble;
    }nibbles;

    clcd_print("                          ",LINE1(0));
    clcd_print("  CHANGE TIME  ",LINE1(0));

    clcd_print("                          ",LINE2(0));
    clcd_print(time,LINE2(0));

    /* Setting the CH bit of the RTC to Stop the Clock */
    dummy = read_ds1307(SEC_ADDR);
    write_ds1307(SEC_ADDR, dummy | 0x80);


    /////////////////////////////////////////////////////////////
    nibbles.dummy = read_ds1307(HOUR_ADDR);


    unsigned char increment_key;
    unsigned char swap_key;
    unsigned char swap_flag = 0;

    unsigned char min = 0;
    unsigned char hour = 0;
    while(1)
    {
	increment_key = read_switches(STATE_CHANGE);
	swap_key = read_switches(STATE_CHANGE);

	if(swap_key == MK_SW6)
	    break;
	if(swap_key == MK_SW5)
	{
	    swap_flag = !swap_flag;

	    /*selecting the time field*/
	    if(swap_flag == 0)
		nibbles.dummy = read_ds1307(HOUR_ADDR);
	    else if(swap_flag == 1)
		nibbles.dummy = read_ds1307(MIN_ADDR);

	}

	if(swap_flag == 1)
	{
	    switch(increment_key)
	    {
		case MK_SW4:
		    {
			if(nibbles.nibble.lower < 9)
			    nibbles.nibble.lower++;
			else if(nibbles.nibble.lower == 9)
			{
			    if(nibbles.nibble.higher >= 0 && nibbles.nibble.higher < 5)
			    {
				nibbles.nibble.higher++;
				nibbles.nibble.lower = 0;
			    }

			}
			if(nibbles.nibble.lower == 9 && nibbles.nibble.higher == 5)
			{
			    nibbles.dummy = 0;
			}
			break;

		    }
	    }


	    if(blink_cursor++ < 1000 )
	    {
		clcd_putch('0' + nibbles.nibble.higher,LINE2(3));
		clcd_putch('0' + nibbles.nibble.lower,LINE2(4));
	    }
	    else if (blink_cursor++ < 2000)
	    {
		clcd_putch(' ',LINE2(3));
		clcd_putch(' ',LINE2(4));
	    }
	    else
		blink_cursor = 0;


	    clcd_putch('0' + nibbles.nibble.higher,LINE2(3));
	    clcd_putch('0' + nibbles.nibble.lower,LINE2(4));
	    min = 0;
	    min = min | nibbles.nibble.higher;
	    min = (min << 4) | nibbles.nibble.lower;
	    write_ds1307(MIN_ADDR,min);
	}
	else if(swap_flag == 0)
	{
	    if ( increment_key == MK_SW4 )
	    {
		if(nibbles.nibble.higher == 2)
		{
		    if(nibbles.nibble.lower >= 4)
		    {
			nibbles.nibble.lower++;
			break;

		    }
		}
		if(nibbles.nibble.lower < 9 && nibbles.nibble.higher < 2)
		{
		    nibbles.nibble.lower++;

		}
		else if(nibbles.nibble.lower == 9)
		{
		    if(nibbles.nibble.higher < 2)
		    {
			nibbles.nibble.higher++;
			nibbles.nibble.lower = 0;
		    }

		}
		if(nibbles.nibble.higher == 2 && nibbles.nibble.lower == 3)
		    nibbles.dummy = 0;


	    }
	    if(blink_cursor++ < 1000 )
	    {
		clcd_putch('0' + nibbles.nibble.higher,LINE2(0));
		clcd_putch('0' + nibbles.nibble.lower,LINE2(1));
	    }
	    else if (blink_cursor++ < 2000)
	    {
		clcd_putch(' ',LINE2(0));
		clcd_putch(' ',LINE2(1));
	    }
	    else
		blink_cursor = 0;


	    clcd_putch('0' + nibbles.nibble.higher,LINE2(0));
	    clcd_putch('0' + nibbles.nibble.lower,LINE2(1));
	    hour=0;

	    hour = nibbles.nibble.higher;
	    hour = (hour << 4) | nibbles.nibble.lower;
	    write_ds1307(HOUR_ADDR,hour);

	}

    }


    write_ds1307(MIN_ADDR,min);
    write_ds1307(HOUR_ADDR,hour);

    store_event(time,gear_state[9],speed);
    /* Clearing the CH bit of the RTC to Start the Clock */
    dummy = read_ds1307(SEC_ADDR);
    write_ds1307(SEC_ADDR, dummy & 0x7F); 

}

void clear_log(void)
{
    for(int i = 0;i < 10;i++)
    {
	storage[i][0] = '\0';
    }
    clcd_print("   LOG CLEARED  ",LINE1(0));
    clcd_print("                ",LINE2(0));
    storage_index = 0;
    store_event(time,gear_state[6],speed);
    for(int i = 1000;i--;);
}

void download_log()
{
    clcd_print(" LOG DOWNOALED ",LINE1(0));
    for(int i = 0;i < 10;i++ )
    {
	if(storage[i][0] == '\0')
	    puts("empty");
	else
	    puts(storage[i]);
	putch('\n');
	putch('\r');
    }
    for(long int i = 70000;i--;);
    store_event(time,gear_state[8],speed);
}

void change_psswd(void)
{
    char key;
    char pass_count = 0;

    store_event(time,gear_state[7],speed);
    clcd_print("TYPE NEW PASSWORD",LINE1(0));
    clcd_print("                  ",LINE2(0));
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
	case MK_SW1:
	    {
		store_event(time,gear_state[10],speed);
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
	    back_key_delay = 5000;
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

    int flag = 0;

    {
	while(1)
	{
	    /*reading the menu option switches*/
	    *menu_key = read_switches(STATE_CHANGE);
	    menu_key_long_press = read_switches(LEVEL_CHANGE);


	    switch(menu_key_long_press)
	    {
		case MK_SW11:
		    {
			if(menu_key_long_press_delay-- == 0)
			{
			    menu_key_long_press_delay = 70000;
			    if(menu_scroll == 0)
			    {
				view_log();
			    }
			    else if(menu_scroll == 1)
			    {
				clear_log();

			    }
			    else if(menu_scroll == 2)
			    {
				set_time();
				*menu_key = 11;
				/*

				//set_time();
				 */
			    }
			    else if(menu_scroll == 3)
			    {
				change_psswd();
			    }
			    if(menu_scroll == 4)
			    {
				download_log();
				clcd_print(menu_log[menu_scroll - 1],LINE1(0));
			    }
			}
			break;
		    }
		case MK_SW12:
		    {
			if(menu_key_long_press_delay-- == 0)
			{
			    menu_key_long_press_delay = 70000;
			    menu_index = 0;
			    clcd_print("                          ",LINE1(0));
			    clcd_print("                          ",LINE2(0));
			    return;
			}
			break;
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


			//			for(long int i = 1000;i--;);


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
			//			for(long int i = 1000;i--;);
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
    clcd_print("                ",LINE2(0));

    for(int i = 0;i < 4;i++)
    {
	password[i] = read_internal_eeprom(i);	
    }

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
