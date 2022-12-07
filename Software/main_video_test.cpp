/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"

#include "xadc_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"
#include <cstdlib>

#include <math.h>
#include <algorithm>






GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN)); //try with this
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));

FrameCore frame(FRAME_BASE);


//GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));


GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
SpriteCore ghost2(get_sprite_addr(BRIDGE_BASE, V4_USER4), 1024);
SpriteCore ghost3(get_sprite_addr(BRIDGE_BASE, V5_USER5), 1024);

SpriteCore explosion(get_sprite_addr(BRIDGE_BASE, V7_BAR), 1024);


OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));


Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));


	bool enable1 = false;
	bool enable2 = false;
	bool enable3 = false;




//////////////////////////////////
char* list1[]  = {  "absolute","daylight", "goodwill", "junction", "magnetic", "princess", "sampling",	//all 5 letter words
		   "victoria", "autonomy", "deferred", //10 Words Done
		   "genomics","judgment", "majority",
		   "        ", "        ", "        ", "        ", "        "
		   "macaques", "nabobery", "oatmeals", "pachinko",
		   "quadplex", "rabbitry", "sabotage",  "ukeleles", "vacation", "wackiest", "xanthine",
		   "yakitori", "zamindar","babbling" };

char* list2[]  = {  "bathroom","eighteen", "highland","keyboard", "notebook", "question",
					"terminal", "withdraw","bacteria", "electric", //10 Words Done
					"handling", "kaiaking",
					"northern",  "        ", "        ", "        ", "        ", "        ","aardvark", "fabulous",  "tabbinet", "earliest", "vacantly", "saboteur",
					"naggiest", "kabbalas", "dabbling", "ubiquity","habiting", "rabbling","jackboot", "cabarets"};

char* list3[]  = {"capacity", "fabliaux", "incident","limiting", "observer", "received",
				"umbrella", "yourself", "calendar", "facility", //10 Words Done
				"identify", "laughter",
				"official",
				"        ", "        ", "        ", "        ", "        "
				"zabajone", "iceboats", "gabbroic", "oarlocks", "rabidity","labeling", "facebook",
				"yammered", "jacinths", "quackish", "caballed", "xanthous", "backings", "waddings" };

char* colors[] = {"Blue  |", "Yellow|", "Red   |", "WINNER! "};
char Word[8];


//blue = 0x00f
//green = 0x0f0
//red = 0xf00
/////////////////////////////////////////////////////////////////////////////////////////

void clear_out() {

		for(int i = 0; i < 8; i++) {
			Word[i] = Word[i +8];
		}
}

void osd_testing(OsdCore *osd_p, int &current, int &row1, int &row2, int &row3) {
	osd_p->set_color(0x0f0, 0x000);

	for(int i = 0; i < 8; i++){
	//		osd_p->wr_char(63 + i, 2, colors[2][i]);
		osd_p->wr_char(72 + i, 2, list3[row3][i]);


		if((list3[row3][i] == Word[i]) && (current == 3)) {
			osd_p->wr_char(72 + i, 2, list3[row3][i],1);

		} else if((list3[row3][i] != Word[i]) && (current == 3)){
			osd_p->wr_char(72 + i, 2, list3[row3][i]);
		}
	}//word3



	for(int i = 0; i < 8; i++){
//		osd_p->wr_char(63 + i, 1, colors[1][i]);

		osd_p->wr_char(72 + i, 1, list1[row1][i]);

		if((list1[row1][i] == Word[i]) && (current == 1)) {
			osd_p->wr_char(72 + i,1 , list1[row1][i],1);
		} else{
			osd_p->wr_char(72 + i, 1, list1[row1][i]);
		}
	}//word1



	for(int i = 0; i < 8; i++){
//		osd_p->wr_char(63 + i, 0, colors[0][i]);

		osd_p->wr_char(72 + i,0, list2[row2][i]);

		if((list2[row2][i] == Word[i]) && (current == 2)) {
			osd_p->wr_char(72 + i,0 , list2[row2][i],1);
		} else{
			osd_p->wr_char(72 + i, 0, list2[row2][i]);
		}
	}//word1

	if(list1[row1][0] == ' ')
	{
		enable2 = 0;
		ghost2.bypass(1);
	}
	else if(list2[row2][0] == ' ')
		{
			enable1 = 0;
			ghost.bypass(1);
		}
	else if(list3[row3][0] == ' ')
		{
			enable3 = 0;
			ghost3.bypass(1);
			if(list2[row2][0] == ' ')
			{
				enable1 = 0;
				ghost.bypass(1);
			}
		}
	if((list1[row1][0] == ' ') && (list2[row2][0] == ' ') && (list3[row3][0] == ' '))
	{
		for(int i = 0; i < 8; i++){
			osd_p->wr_char(36 + i, 0, colors[3][i]);
		}
	}

}



void explosion_check(SpriteCore *explosion_p,int rocket_select, int y_position_1,int y_position_2,int y_position_3, bool enable, int bases_state) {
   int x, y;
   if(rocket_select == 0)
   {
	   y = y_position_1;
	   if(bases_state == 100||bases_state ==101||bases_state ==110||bases_state ==111)
	      {
	   	   x = 185;
	      }
	      else if(bases_state == 010||bases_state ==011)
	      {
	   	   x = 320;
	      }
	      else
	      {
	   	   x = 455;
	      }
   }
   else if(rocket_select == 1)
   {
	   y = y_position_2;

	   	  if(bases_state == 010||bases_state ==011||bases_state ==110||bases_state ==111)
	   	   {
	   		   x = 320;							//Position 2
	   	   }
	   	   else if(bases_state == 001||bases_state ==101)
	   	   {
	   		   x = 455;					//Position 3
	   	   }
	   	   else
	   	   {
	   		   x = 185;					//Position 1
	   	   }
   }
   else
   {
	   y = y_position_3;
		if(bases_state == 001||bases_state ==011||bases_state ==101||bases_state ==111)			//Position 3
			   {
				   x = 455;
			   }
			   else if(bases_state == 100||bases_state ==110)		//Position 1
			   {
				   x = 185;
			   }
			   else										//Position 2
			   {
				   x = 320;
			   }
   }
   if(enable == true)
   {
   // slowly move mouse pointer
   explosion_p->bypass(0);
   explosion_p->wr_ctrl(0x04);  //animation; red
  // x = 0;
  // y = 400;
   for (int i = 0; i < 150; i++) {
      explosion_p->move_xy(x, y);
      sleep_ms(1);
      //x = x + 4;
   }
explosion_p -> bypass(1);
   }
   else // else for enable
   {
	   explosion_p -> bypass(1);
   }
   //sleep_ms(3000);
   //explosion_p->move_xy(185, 460);
}



bool good_collision;
bool enable_boom_blue = false;
bool enable_boom_yellow = false;
bool enable_boom_red = false;

bool enable_Boom = false;
int rocket_select = 0;



void test_start(GpoCore *led_p) {
   int i;

   for (i = 0; i < 20; i++) {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}











void sseg_kb_mouse(SsegCore *sseg_p, int speed)
{

	//turn off led
	for (int i = 0; i < 8; i++)
	{
	  sseg_p->write_1ptn(0xff, i);
	}
	speed = 999- speed;
	//speed values
	int reading1 = speed/100;//%10;
	int reading2 = speed/10%10;//%10;
	int reading3 = speed%10;//%10;

	//writing to sseg
	sseg_p->write_1ptn(sseg_p->h2s(reading1),2);
	sseg_p->write_1ptn(sseg_p->h2s(reading2),1);
	sseg_p->write_1ptn(sseg_p->h2s(reading3),0);
	sseg_p->set_dp(0x00);


}

void pwm_good_collision(PwmCore *pwm_p,bool good_collision,bool bad_collision)
{
	double red = 1;
	double green = 1;
	double blue = 1;

	if(good_collision == true)
	{
	blue = 0;
	red = 0;
	//pwm_p->set_duty(red, 2);
	pwm_p->set_duty(green, 1);
	//pwm_p->set_duty(blue, 0);

	pwm_p->set_duty(red, 5);
	pwm_p->set_duty(green, 4);
	pwm_p->set_duty(blue, 3);
	}
	else if(bad_collision == true)
	{
		green = 0;
			blue = 0;
			red = 1;
			pwm_p->set_duty(red, 2);
			pwm_p->set_duty(red, 5);

	}
	else
	{
		green = 0;
		red = 0;
		blue = 0;
		pwm_p->set_duty(red, 2);
			pwm_p->set_duty(green, 1);
			pwm_p->set_duty(blue, 0);

		pwm_p->set_duty(red, 5);
		pwm_p->set_duty(green, 4);
		pwm_p->set_duty(blue, 3);
	}

}



/**
 * check bar generator core
 * @param bar_p pointer to Gpv instance
 */
void bar_check(GpvCore *bar_p) {
   bar_p->bypass(0);
   sleep_ms(3000);
}

/**
 * check color-to-grayscale core
 * @param gray_p pointer to Gpv instance
 */
void gray_check(GpvCore *gray_p) {
   gray_p->bypass(0);
   sleep_ms(3000);
   gray_p->bypass(1);
}

/**
 * check osd core
 * @param osd_p pointer to osd instance
 *
 */
void osd_check(OsdCore *osd_p) {
   osd_p->set_color(0x0f0, 0x001); // dark gray/green
   osd_p->bypass(0);
   osd_p->clr_screen();
   for (int i = 0; i < 64; i++) {
      osd_p->wr_char(8 + i, 20, i);
      osd_p->wr_char(8 + i, 21, 64 + i, 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
}

/**
 * test frame buffer core
 * @param frame_p pointer to frame buffer instance
 */
void frame_check(FrameCore *frame_p) { ///////
   int x, y, color;

   frame_p->bypass(0);
   for (int i = 0; i < 10; i++) {
      frame_p->clr_screen(0x008);  // dark green
      for (int j = 0; j < 20; j++) {
         x = rand() % 640;
         y = rand() % 480;
         color = rand() % 512;
         frame_p->plot_line(400, 200, x, y, color);
      }
     // sleep_ms(300);
   }
   //sleep_ms(3000);
}

int prev_state = 000;

void frame_check2(FrameCore *frame_p,int state_of_gme) { /////////////////////////////////////////////
   int x, y, color;

   frame_p->bypass(0);

         x =  240;
         y =  320;
         color = 220;

         if(state_of_gme != prev_state)
         {
    	 frame_p->clr_screen(0x000);
         }
         if(state_of_gme == 111)
         {

         //Tower 1
         frame_p->plot_line(45, 640, 85, 640, color);
         frame_p->plot_line(45, 640, 45, 680, color);
         frame_p->plot_line(85, 640, 85, 680, color);
         //Tower 2
         frame_p->plot_line(180, 640, 220, 640, color);
         frame_p->plot_line(180, 640, 180, 680, color);
         frame_p->plot_line(220, 640, 220, 680, color);
         //Tower 2
         frame_p->plot_line(315, 640, 355, 640, color);
         frame_p->plot_line(315, 640, 315, 680, color);
         frame_p->plot_line(355, 640, 355, 680, color);
         //FLOOR
         frame_p->plot_line(-75, 680, 480, 680, color);
         }

         if(state_of_gme == 0)
                  {
                  //FLOOR
//        	 	 frame_p->clr_screen(0x008);
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }
         if(state_of_gme == 1)
                  {

//                  //Tower 3
                  frame_p->plot_line(315, 640, 355, 640, color);
                  frame_p->plot_line(315, 640, 315, 680, color);
                  frame_p->plot_line(355, 640, 355, 680, color);
                  //FLOOR
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }
         if(state_of_gme == 10)
                  {

                  //Tower 1

                  //Tower 2
                  frame_p->plot_line(180, 640, 220, 640, color);
                  frame_p->plot_line(180, 640, 180, 680, color);
                  frame_p->plot_line(220, 640, 220, 680, color);
                  //Tower 2

                  //FLOOR
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }
         if(state_of_gme == 11)
                  {

//                  //Tower 1

                  //Tower 2
                  frame_p->plot_line(180, 640, 220, 640, color);
                  frame_p->plot_line(180, 640, 180, 680, color);
                  frame_p->plot_line(220, 640, 220, 680, color);
                  //Tower 2
                  frame_p->plot_line(315, 640, 355, 640, color);
                  frame_p->plot_line(315, 640, 315, 680, color);
                  frame_p->plot_line(355, 640, 355, 680, color);
                  //FLOOR
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }
         if(state_of_gme == 100)
                  {

                  //Tower 1
                  frame_p->plot_line(45, 640, 85, 640, color);
                  frame_p->plot_line(45, 640, 45, 680, color);
                  frame_p->plot_line(85, 640, 85, 680, color);
                  //Tower 2

                  //FLOOR
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }
         if(state_of_gme == 101)
                  {

                  //Tower 1
                  frame_p->plot_line(45, 640, 85, 640, color);
                  frame_p->plot_line(45, 640, 45, 680, color);
                  frame_p->plot_line(85, 640, 85, 680, color);
                  //Tower 2

                  //Tower 2
                  frame_p->plot_line(315, 640, 355, 640, color);
                  frame_p->plot_line(315, 640, 315, 680, color);
                  frame_p->plot_line(355, 640, 355, 680, color);
                  //FLOOR
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }
         if(state_of_gme == 110)
                  {

                  //Tower 1
                  frame_p->plot_line(45, 640, 85, 640, color);
                  frame_p->plot_line(45, 640, 45, 680, color);
                  frame_p->plot_line(85, 640, 85, 680, color);
                  //Tower 2
                  frame_p->plot_line(180, 640, 220, 640, color);
                  frame_p->plot_line(180, 640, 180, 680, color);
                  frame_p->plot_line(220, 640, 220, 680, color);
                  //Tower 2

                  //FLOOR
                  frame_p->plot_line(-75, 680, 480, 680, color);
                  }

         prev_state = state_of_gme;
}




/**
 * test ghost sprite
 * @param ghost_p pointer to mouse sprite instance
 */

double counter_timer = 0;
bool timer_done = false;
int timer_output = 0;
//int current_time = now_ms();


/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */

int i_ghost1, i_ghost2, i_ghost3, i_explosion, state_of_bases;
int speed_sleep = 50;


int xadc_speed(XadcCore *adc_p, double reading)
{
			//double reading;
			reading = (adc_p->read_adc_in(0)) *(-1000) +999;
//				uart.disp("\n\r");
				speed_sleep =999- reading;
return speed_sleep;
}


//Keep this BLUE
int ghost_check(SpriteCore *ghost_p , int start,  bool enable, int bases_state) {//int speed_sleep
   int x, y;
//   unsigned int time = now_ms();

   if(bases_state == 100||bases_state ==101||bases_state ==110||bases_state ==111)
   {
	   x = 185;
   }
   else if(bases_state == 10||bases_state ==11)
   {
	   x = 320;
   }
   else if(bases_state == 001)
   {
	   x = 455;
   }
   else if(bases_state == 0)
   {
	   ghost_p->bypass(1);
   }

   if(enable == true && bases_state != 0){
   start = i_ghost1;
   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x1c);  //animation; blue ghost
   //x = 185;
   y = start;
   if (i_ghost1 < 460) {
      ghost_p->move_xy(x, y);
//      sleep_ms(speed_sleep);
      //y = y + 4;
      if (i_ghost1 == 60) {
         // change to red ghost half way
         ghost_p->wr_ctrl(0x1c);

      }
   }
   i_ghost1 = i_ghost1 + 4;
   if (i_ghost1 > 460)
   {
	   i_ghost1  =0;
   }
   }
   else // else for enable
   {
	   i_ghost1 = 0;
	   ghost_p->bypass(1);
   }

   return i_ghost1;
}



//Keep this Yellow
int ghost_check_2(SpriteCore *ghost2_p, int start2,  bool enable,int bases_state) {
	   int x, y;


	  	   if(bases_state == 10||bases_state ==11||bases_state ==110||bases_state ==111)
	   {
		   x = 320;							//Position 2
	   }
	   else if(bases_state == 001||bases_state ==101)
	   {
		   x = 455;					//Position 3
	   }
	   else if(bases_state == 100)
	   {
		   x = 185;					//Position 1
	   }
	   else if(bases_state == 0)
	   {
		   ghost2_p->bypass(1);
		   x = 0;
		   y = 0;
	   }
 if(enable == true && bases_state != 0){
	 start2 = i_ghost2;
	   // slowly move mouse pointer
	   ghost2_p->bypass(0);
	   if(i_ghost2 >= 0 && i_ghost2 <240)
	   {
	   ghost2_p->wr_ctrl(1111);  //animation; blue ghost
	   }
	 //  x = 320;
	   y = start2;
	   if (i_ghost2 < 460) {
		   ghost2_p->move_xy(x, y);
	      //sleep_ms(speed_sleep);
	      //y = y + 4;
	      if (i_ghost2 >= 240) {
	         // change to red ghost half way
	         ghost2_p->wr_ctrl(1111);

	      }
	   }
	   i_ghost2 = i_ghost2 + 4;
	   if (i_ghost2 > 460)
	   {
		   i_ghost2  =0;
	   }
	   }
	   else // else for enable
	   {
		   i_ghost2 = 0;
		   ghost2_p->bypass(1);
	   }
	   return i_ghost2;
	}


//keep this RED
int ghost_check_3(SpriteCore *ghost3_p, int start3,  bool enable,int bases_state) {
	int x, y;

	if(bases_state == 001||bases_state ==11||bases_state ==101||bases_state ==111)			//Position 3
		   {
			   x = 455;
		   }
		   else if(bases_state == 100||bases_state ==110)		//Position 1
		   {
			   x = 185;
		   }
		   else if (bases_state == 10)								//Position 2
		   {
			   x = 320;
		   }
		   else
		   {
			   ghost3_p->bypass(1);
			   x = 0;
			   y = 0;
		   }


	if(enable == true && bases_state != 0){
	start3 = i_ghost3;
	// slowly move mouse pointer
	ghost3_p->bypass(0);
	if(i_ghost3 >= 0 && i_ghost3 <240)
	{
	ghost3_p->wr_ctrl(0x04);  //animation; blue ghost
	}
	//x = 455;
	y = start3;
	if (i_ghost3 < 460) {
		ghost3_p->move_xy(x, y);
	   //sleep_ms(speed_sleep);
	   //y = y + 4;
	   if (i_ghost3 >= 240)
	   {
		  // change to red ghost half way
		   ghost3_p->wr_ctrl(0x04);
	   }
	}
	i_ghost3 = i_ghost3 + 4;
	if (i_ghost3 > 460)
	{
		i_ghost3  =0;
	}
	}
	else // else enable
	{
		i_ghost3 = 0;
		ghost3_p->bypass(1);
	}
	return i_ghost3;
	}




void testing(PwmCore *pwm_p, Ps2Core *ps2_p, OsdCore *osd_p, int &count, int &current, int &row1,  int &row2,  int &row3, int &base_altitude, bool &good_collision, bool &bad_collision, int &y_axis_of_word1, int &y_axis_of_word2, int &y_axis_of_word3) {
	char ch;
	good_collision = false;
	rocket_select = 0;	//ADDED
	enable_Boom = false; //CHANGED
	if(list2[row2][0] != ' ')
	{
	 enable1 = true;
	}
	if(list1[row1][0] != ' ')
	{
	 enable2 = true;
	}
	if(list3[row3][0] != ' ')
		{
	 enable3 = true;
		}
	if(y_axis_of_word3 >= base_altitude) {
			good_collision = false;
			clear_out();
			current = 0;
			count = 0;
			row3++;
	} else if(y_axis_of_word1 >= base_altitude) {
		good_collision = false;
		clear_out();
		current = 0;
		count = 0;
		row1++;
	} else if(y_axis_of_word2 >= base_altitude) {
		good_collision = false;
		clear_out();
		current = 0;
		count = 0;
		row2++;
	}


	if(ps2_p->get_kb_ch(&ch)) {
		if(count <= 7) {
//
			Word[count] = ch;
			if(y_axis_of_word3 < base_altitude) {

				if(list3[row3][count] == Word[count]) {
					count++;
					current = 3;
					good_collision = false;
//					bad_collision = false;
//					uart.disp("Good Collision Red");
//					uart.disp("\r\n");
					if(list3[row3][7] == Word[7]) {
						clear_out();
						current = 0;
						count = 0;
						row3++;
						good_collision = true;
//						bad_collision = false;
//						uart.disp("Good Collision Red");
//						uart.disp("\r\n");
						rocket_select = 2;
						enable_Boom = true;
						enable3 = false;
					} // same last letter
				} else if(list1[row1][count] == Word[count]) {
					count++;
					current = 1;
					good_collision = false;
//					bad_collision = false;
//					uart.disp("Good Collision Blue");
//					uart.disp("\r\n");
					if(list1[row1][7] == Word[7]) {
						clear_out();
						current = 0;
						count = 0;
						row1++;
						good_collision = true;
//						bad_collision = false;
//						uart.disp("Good Collision Yellow");
//						uart.disp("\r\n");

						rocket_select = 1;
						enable_Boom = true;
						enable2 = false;
						//end of word
					}
				} else if(list2[row2][count] == Word[count]) {
					count++;
					current = 2;
					good_collision = false;
//					bad_collision = false;
//					uart.disp("Good Collision Yellow");
//					uart.disp("\r\n");
					if(list2[row2][7] == Word[7]) {
						clear_out();
						current = 0;
						count = 0;
						row2++;
						good_collision = true;
//						bad_collision = false;
//						uart.disp("Good Collision Blue");
//						uart.disp("\r\n");

						rocket_select = 0;
						enable_Boom = true;
						enable1 = false;


					}
				} else if(((list1[row1][count] != Word[count]) || (list2[row2][count] != Word[count]) || (list3[row3][count] != Word[count]))) {
					count = 0;
					current = 0;
					clear_out();
					good_collision = false;
//					bad_collision = false;
	//				pwm_good_collision(pwm_p, good_collision, bad_collision);
				}//check 2 arrays //end of word

			} // good collision


		}//count <= 8
			pwm_good_collision(pwm_p, good_collision, bad_collision);
			osd_testing(osd_p, current, row1, row2, row3);
			good_collision = false;
//			pwm_good_collision(pwm_p, 0, bad_collision);

	}//ps2
}//kb




int stateOfBasesConverter(int altitudeBlue, int altitudeYellow, int altitudeRed, int stateOfBases)

{
	if(stateOfBases == 111)
	{
		if(altitudeBlue > 430)
		{
			state_of_bases = 11;
			i_ghost1 = 0;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 101;
			i_ghost2 = 0;
		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 110;
			i_ghost3 = 0;
		}
	}//should work

	else if(stateOfBases == 110)
	{
		if(altitudeBlue > 430)
		{
			state_of_bases = 10;
			i_ghost1 = 0;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 100;
			i_ghost2 = 0;

		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 10;
			i_ghost3 = 0;

		}
	} //Should work

	else if(stateOfBases == 101)
	{
		if(altitudeBlue > 430)
		{
			i_ghost1 = 0;
			state_of_bases = 1;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 100;
			i_ghost2 = 0;

		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 100;
			i_ghost3 = 0;
		}

	}//should work
	else if(stateOfBases == 100)
	{
		if(altitudeBlue > 430)
		{
			state_of_bases = 0;
			i_ghost1 = 0;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 0;
			i_ghost2 = 0;

		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 0;
			i_ghost3 = 0;
		}

	} //should work game over -----------------
	else if(stateOfBases == 11)
	{
		if(altitudeBlue > 430)
		{
			state_of_bases = 1;
			i_ghost1 = 0;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 1;
			i_ghost2 = 0;

		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 10;
			i_ghost3 = 0;
		}

	}//should work
	else if(stateOfBases == 10)
	{
		if(altitudeBlue > 430)
		{
			state_of_bases = 0;
			i_ghost1 = 0;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 0;
			i_ghost2 = 0;
		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 0;
			i_ghost3 = 0;
		}

	} //should work Game over -----------------
	else if(stateOfBases == 1)
	{
		if(altitudeBlue > 430)
		{
			state_of_bases = 0;
			i_ghost1 = 0;
		}
		else if(altitudeYellow > 430)
		{
			state_of_bases = 0;
			i_ghost2 = 0;
		}
		else if(altitudeRed > 430)
		{
			state_of_bases = 0;
			i_ghost3 = 0;
		}

	} // should work Game over -----------------
	else if(stateOfBases == 0)
	{
			state_of_bases = 0;
			ghost.bypass(1);
			ghost2.bypass(1);
			ghost3.bypass(1);
	}


	return state_of_bases;
}



int main() {

	//Jonamhae's stuff
	int y_axis_of_word1 = 0;
	int y_axis_of_word2 = 0;
	int y_axis_of_word3 = 0;
	int base_altitude = 430;
	bool good_collision = false;
	bool bad_collision = false;
	int count = 0;
	int current = 0;
	int row1 = 0, row2= 0, row3= 0;

	for(int i = 0; i < 8; i++){
		osd.wr_char(63 + i, 0, colors[0][i]);
		osd.wr_char(63 + i, 1, colors[1][i]);
		osd.wr_char(63 + i, 2, colors[2][i]);
		osd.wr_char(72 + i, 3, '-');

		osd.wr_char(72 + i, 1, list1[row1][i]);
		osd.wr_char(72 + i, 0, list2[row2][i]);
		osd.wr_char(72 + i, 2, list3[row3][i]);
	}

		row1 = 0;
		row2 = 0;
		row3 = 0;
		//My stuff
	 //speed_sleep = 50;
//	int rocket_select = 0;
	i_ghost1 = 0; i_ghost2 = 0; i_ghost3 = 0; i_explosion = 0;
	int now ;
	bool temp_on = false;

//	bool enable_Boom = false;

	int i_ghost1_temp  = 0;
	int i_ghost2_temp  = 0;
	int i_ghost3_temp  = 0;
	state_of_bases = 111;
	int base1,base2,base3;
	base1 = 1;
	base2 = 1;
	base3 = 1;
	ghost.bypass(1);
	ghost2.bypass(1);
	ghost3.bypass(1);
	explosion.bypass(1);
	osd.bypass(0);
	mouse.bypass(1);
	frame.bypass(1);


   while (1) {
	   	   	   good_collision = false;
				if(state_of_bases == 0)
				{
					ghost.bypass(1);
					ghost2.bypass(1);
					ghost3.bypass(1);
				}
	   frame_check2(&frame,state_of_bases);
	  // state_of_bases = base1*100+base2*10+base3;


	   if(btn.read(0) == 1||btn.read(1) == 1||btn.read(2) == 1||btn.read(3) == 1)
	   {
		    temp_on = true;
	   }
	   else
	   {
		   temp_on = false;
	   }




	   if(i_ghost1 != 0)
	   {
		   i_ghost1_temp = i_ghost1;
	   }
	   else
	   {
		   i_ghost1_temp = i_ghost1_temp;
	   }

	   if(i_ghost2 != 0)
		   {
			   i_ghost2_temp = i_ghost2;
		   }
		   else
		   {
			   i_ghost2_temp = i_ghost2_temp;
		   }
	   if(i_ghost3 != 0)
		   {
			   i_ghost3_temp = i_ghost3;
		   }
		   else
		   {
			   i_ghost3_temp = i_ghost3_temp;
		   }

	    bad_collision = false;
	   if(i_ghost1 >= 425 || i_ghost2 >= 425 || i_ghost3 >= 425 )
	   {
		   bad_collision = true;
	   }

	  sseg_kb_mouse(&sseg, speed_sleep);
	  if(sw.read(0) ==1)
	  {
      ghost_check(&ghost,i_ghost1, enable1,state_of_bases);
      ghost_check_2(&ghost2,i_ghost2, enable2,state_of_bases);
      ghost_check_3(&ghost3,i_ghost3, enable3,state_of_bases);
	  }
      explosion_check(&explosion,rocket_select,i_ghost1_temp,i_ghost2_temp,i_ghost3_temp,enable_Boom,state_of_bases);
      sleep_ms(speed_sleep); // This is the only sleep in the system, its not in every missle because then it would add up and keys would become unresponsive
      stateOfBasesConverter( i_ghost1,  i_ghost2,  i_ghost3, state_of_bases);
      rocket_select = 0;
      enable_Boom = false;
      xadc_speed(&adc,speed_sleep);
	  pwm_good_collision(&pwm, 0, bad_collision);
	  testing(&pwm,&ps2, &osd, count, current, row1, row2, row3, base_altitude, good_collision, bad_collision, i_ghost1, i_ghost2, i_ghost3);

      //} //while
   } // while
} //main
