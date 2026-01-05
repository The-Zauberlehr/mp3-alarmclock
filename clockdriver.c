#include <stdio.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "EPD_2in9_V2.h"
#include  "EPD_Test.h"
#include "time.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "pico/time_adapter.h"
#include "pico/util/datetime.h"
#include "pico/aon_timer.h"
#include "math.h"
#include "dfplayer.h"
#include "hardware/powman.h"
#include "pico/sync.h"
#include "ds3231.h"
#include "clockdriver.h"
UBYTE *BlackImage;
char ringtones[34][35]= {"1.django Unchained Main Theme","2.Time of my Life","3.Hakuna Matata","4.Dream of Arrakis","5.Comfort Chain","6.Can you here the music","7.American Prometheus","8.Quantum Mechanics","9.Tick Tock","10.Pedro","11.Waterfalls","12.Bayside","13.12 to 12","14.Close to Me","15.Kiss","16.Red Bellied Black Snack","17.Flying High","18.Woman","19.Tainted Love","20.Weisser Winterwald","21.Driving Home for Christmas","22.Weihnachtsbaum","23.Gesegnet","24.Fernweh","25.Wie du liebst","26. 2","27.Diffus","28.Nur wegen dir","29. Girl Youll be a Woman soon","30.Stuck in the Middle with you","31.Sultans of Swing","32. Hotel California","33.iphone wecker","34.klingel"};
char weekdays[7][3]= {"So.","Mo.","Di.","Mi.","Do.","Fr.","Sa."};
char alarmmodes[3][5]={"aus","Mo-Fr","Mo-So"};
char timezones[4][8]={"UTC","CET","CEST,EET","EEST"};

dfplayer_t dfplayer;
struct tm t;
ds3231_data_t ds3231_data;
ds3231_t ds3231;
bool snooze=-1;
int position;
int startingvalue;

/**
 * @defgroup functions Functions
 * @{
 */
void setstartingvalue(int startvalue){
    startingvalue=startvalue;
}
int returnstaringvalue(){
    return startingvalue;
}
/**
 * @brief init all modules
 *
 * @param Blackimage  
 * @return -1 if not succesful
 */
 int initall(){
    stdio_init_all();
    gpio_init(topbutton);
    gpio_init(menu);
    gpio_init(plus);
    gpio_init(minus);
    gpio_set_dir(topbutton,false);
    gpio_set_dir(menu,false);
    gpio_set_dir(plus,false);
    gpio_set_dir(minus,false);

    DEV_Delay_ms(900);
       if(DEV_Module_Init()!=0){
        return -1;
    }
    uint8_t sda_pin=4;
    uint8_t scl_pin=5;

    ds3231_t ds3231;
   
   
    ds3231_init(&ds3231, i2c_default, DS3231_DEVICE_ADRESS, AT24C32_EEPROM_ADRESS_0);
    sleep_ms(200);
    dfplayer_init(&dfplayer, uart0, 16, 17);

    sleep_ms(200);
    
    dfplayer_set_checksum_tx(false);
    gpio_init(sda_pin);
    gpio_init(scl_pin);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT * 2;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    i2c_init(ds3231.i2c, 400 * 1000);
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    Paint_SetScale(5);
    Paint_Clear(WHITE);
    if(ds3231_read_current_time(&ds3231, &ds3231_data)) {
            printf("No data is received\n");
        } else {
           
                
                t.tm_hour=ds3231_data.hours;
                t.tm_min=ds3231_data.minutes;
                t.tm_sec=ds3231_data.seconds;
                t.tm_year=ds3231_data.year+100;
                t.tm_mon=ds3231_data.month-1;
                
                t.tm_mday=ds3231_data.date;
                t.tm_wday=ds3231_data.day;

        }
    
    aon_timer_start_calendar(&t);
    
    printf("e-Paper Init and Clear...\r\n");
	EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();
    sleep_ms(200);
    Paint_DrawString_EN(10,10,"hello",&Font16,BLACK,WHITE);
    EPD_2IN9_V2_Display_Partial(BlackImage);
}
/**
 * @brief paint the alarm or curently playing song.
 *
 * @param mode the current status of the clock.
 * @param ringtone the id of the selected song 
 * @return true if succesful
 */

bool paintringtone(clock_mode clock,int ringtone){
    if(clock==clockmode){
        Paint_ClearWindows(0,EPD_2IN9_V2_WIDTH-10-Font12.Height,10+Font12.Width*30,EPD_2IN9_V2_WIDTH-10,WHITE);
        Paint_DrawString_EN(0,EPD_2IN9_V2_WIDTH-10-Font12.Height,ringtones[ringtone-1],&Font12,WHITE,BLACK);
        return true;
    }
    else if (clock==musicmode_music_off||clock==musicmode_music_on){
        int a = dfplayer_get_track_id(&dfplayer);
        if (a==0){
            a=1;
        }
        
        Paint_ClearWindows(0,EPD_2IN9_V2_WIDTH-10-Font12.Height,10+Font12.Width*30,EPD_2IN9_V2_WIDTH-10,WHITE);
        Paint_DrawString_EN(0,EPD_2IN9_V2_WIDTH-10-Font12.Height,ringtones[a-1],&Font12,WHITE,BLACK);
        
    }
    else{
        return false;
    }

}
/**
 * @brief paint the weekday
 *
 * @param mode the current status of the clock.
 * @return true if succesful
 */
bool paintweekday(clock_mode mode){
    if(mode==clockmode){
        Paint_ClearWindows(10, 10, 10 + Font24.Width * 13,  10+Font20.Height, WHITE);
        Paint_DrawString_EN(10,10,weekdays[t.tm_wday],&Font16,WHITE,BLACK);
        if(t.tm_mday<10){
            Paint_DrawString_EN(15+Font16.Width*3,10,"0",&Font16,WHITE,BLACK);
            Paint_DrawNum(15+Font16.Width*4,10,t.tm_mday,&Font16,BLACK,WHITE);
            if(t.tm_mday==0){
                Paint_DrawString_EN(15+Font16.Width*4,10,"0",&Font16,WHITE,BLACK);
            }
        }     
        else{
            Paint_DrawNum(15+Font16.Width*3,10,t.tm_mday,&Font16,BLACK,WHITE);
        }    
        Paint_DrawString_EN(15+Font16.Width*5,10,".",&Font16,WHITE,BLACK);
        
        if((t.tm_mon+1)<10){
            Paint_DrawString_EN(15+Font16.Width*6,10,"0",&Font16,WHITE,BLACK);
            Paint_DrawNum(15+Font16.Width*7,10,t.tm_mon+1,&Font16,BLACK,WHITE);
            if((t.tm_mon+1)==0){
                Paint_DrawString_EN(15+Font16.Width*7,10,"0",&Font16,WHITE,BLACK);
            }
        }     
        else{
            Paint_DrawNum(15+Font16.Width*6,10,t.tm_mon+1,&Font16,BLACK,WHITE);
        } 
        Paint_DrawString_EN(15+Font16.Width*8,10,".",&Font16,WHITE,BLACK);
        if(t.tm_year<10){
            Paint_DrawString_EN(15+Font16.Width*9,10,"0",&Font16,WHITE,BLACK);
            Paint_DrawNum(15+Font16.Width*10,10,t.tm_year+1900,&Font16,BLACK,WHITE);
            if(t.tm_year==0){
                Paint_DrawString_EN(15+Font16.Width*4,10,"0",&Font16,WHITE,BLACK);
            }
        }     
        else{
            Paint_DrawNum(15+Font16.Width*9,10,t.tm_year+1900,&Font16,BLACK,WHITE);
    
        
        }    
        return true;
    }
    else {
        return false;
    }
    
}
/**
 * @brief paint the alarm time
 *
 * @param mode the current status of the clock.
 * @param alarm array [0]...hour,[1]...min when alarm will be triggered
 * @return true if succesful
 */
bool paintalarm(clock_mode mode,int alarm[]){
    if (mode==clockmode){
        Paint_ClearWindows(EPD_2IN9_V2_HEIGHT-10-Font16.Width*5,EPD_2IN9_V2_WIDTH-10-Font12.Height, EPD_2IN9_V2_HEIGHT-10,Font16.Width+EPD_2IN9_V2_WIDTH-10-Font16.Height, WHITE);
            if(alarm[0]<10){
                Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT-10-Font16.Width*5,EPD_2IN9_V2_WIDTH-10-Font16.Height,"0",&Font16,WHITE,BLACK);
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT-10-Font16.Width*4,EPD_2IN9_V2_WIDTH-10-Font16.Height,alarm[0],&Font16,BLACK,WHITE);
                if(alarm[0]==0){
                    Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT-10-Font16.Width*4,EPD_2IN9_V2_WIDTH-10-Font16.Height,"0",&Font16,WHITE,BLACK);
                }
            }
            else{
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT-10-Font16.Width*5,EPD_2IN9_V2_WIDTH-10-Font16.Height,alarm[0],&Font16,BLACK,WHITE);
            }
            Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT-10-Font16.Width*3,EPD_2IN9_V2_WIDTH-10-Font16.Height,":",&Font16,WHITE,BLACK);
            if(alarm[1]<10){
                 Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT-10-Font16.Width*2,EPD_2IN9_V2_WIDTH-10-Font16.Height,"0",&Font16,WHITE,BLACK);
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT-10-Font16.Width*1,EPD_2IN9_V2_WIDTH-10-Font16.Height,alarm[1],&Font16,BLACK,WHITE);
                if(alarm[1]==0){
                    Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT-10-Font16.Width,EPD_2IN9_V2_WIDTH-10-Font16.Height,"0",&Font16,WHITE,BLACK);
                }
            }
            else{
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT-10-Font16.Width*2,EPD_2IN9_V2_WIDTH-10-Font16.Height,alarm[1],&Font16,BLACK,WHITE);
            }
        return true;
    }
    else{
        return false;
    }
}
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param timezone the offse
 * @return true if succesful
 */
bool painttime(clock_mode mode,int timezone){
    if (mode==clockmode||mode==musicmode_music_off||mode==musicmode_music_on){
        aon_timer_get_time_calendar(&t);
        sPaint_time.Min = t.tm_min;
        sPaint_time.Hour= t.tm_hour;
        Paint_ClearWindows(EPD_2IN9_V2_HEIGHT/2-Font40.Width*3,EPD_2IN9_V2_WIDTH/2-Font40.Height/2, EPD_2IN9_V2_HEIGHT/2+Font40.Width*3,EPD_2IN9_V2_WIDTH/2+Font40.Height/2, WHITE);
            if(t.tm_hour<10){
                Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT/2-Font40.Width*2,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,"0",&Font40,WHITE,BLACK);
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT/2-Font40.Width,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,t.tm_hour,&Font40,BLACK,WHITE);
                if(t.tm_hour==0){
                    Paint_DrawChar(EPD_2IN9_V2_HEIGHT/2-Font40.Width,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,'0',&Font40,WHITE,BLACK);
                }
            }
            else{
                Paint_DrawNum((EPD_2IN9_V2_HEIGHT/2-Font40.Width*2),EPD_2IN9_V2_WIDTH/2-Font40.Height/2,t.tm_hour,&Font40,BLACK,WHITE);
            }
            Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT/2,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,":",&Font40,WHITE,BLACK);
            if(t.tm_min<10){
                Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT/2+Font40.Width,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,"0",&Font40,WHITE,BLACK);
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT/2+Font40.Width*2,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,t.tm_min,&Font40,BLACK,WHITE);
                if(t.tm_min==0){
                    Paint_DrawString_EN(EPD_2IN9_V2_HEIGHT/2+Font40.Width*2,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,"0",&Font40,WHITE,BLACK);
                }
            }
            else{
                Paint_DrawNum(EPD_2IN9_V2_HEIGHT/2+Font40.Width,EPD_2IN9_V2_WIDTH/2-Font40.Height/2,t.tm_min,&Font40,BLACK,WHITE);
            }
    }
    else{
        return false;
    }
   
}

/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 
 * @return true if succesful
 */
bool playmusic (clock_mode mode,int titel,int volume){
    if(mode==clockmode){
        dfplayer_set_eq(&dfplayer,EQ_POP);
        sleep_ms(200);
                
        dfplayer_set_playback_mode(&dfplayer,MODE_FOLDER_REPEAT);
        //hardcoded comand 0x11 in the libary
        sleep_ms(200);
            
        dfplayer_set_volume(&dfplayer, volume*5);

        sleep_ms(200);
        return true;
    }
    else if (mode==musicmode_music_off){
        dfplayer_set_eq(&dfplayer,EQ_POP);
        sleep_ms(200);
                
        dfplayer_play(&dfplayer,titel);
        sleep_ms(200);
            
        dfplayer_set_volume(&dfplayer, volume*5);

        sleep_ms(200);
        return true;
    }
    else{
        return false;
    }
}


bool paintsettings(clock_mode mode,int alarmmode,int timezone,int alarm[],int titel,int volume){
    if(mode==settingsclock||mode==settingsmusic){
        position = 0;
        Paint_DrawString_EN(20,0,"Weckmodus:",&Font16,WHITE,BLACK);
        Paint_DrawString_EN(20,Font16.Height+2,"Weckstunde:",&Font16,WHITE,BLACK);
        Paint_DrawString_EN(20,(Font16.Height+2)*2,"Weckminute:",&Font16,WHITE,BLACK);
        Paint_DrawString_EN(20,(Font16.Height+2)*3,"Titel:",&Font16,WHITE,BLACK);
        Paint_DrawString_EN(20,(Font16.Height+2)*4,"Lautstaerke:",&Font16,WHITE,BLACK);
        Paint_DrawString_EN(20,(Font16.Height+2)*5,"Zeitzone",&Font16,WHITE,BLACK);
        if(mode==settingsclock){
            Paint_DrawString_EN(20,(Font16.Height+2)*6,"Musikmodus",&Font16,WHITE,BLACK);
        }
        else{
             Paint_DrawString_EN(20,(Font16.Height+2)*6,"Weckermodus",&Font16,WHITE,BLACK);
        }
        Paint_DrawString_EN(200,0,alarmmodes[alarmmode],&Font16,WHITE,BLACK);
        Paint_DrawNum(200,Font16.Height+2,alarm[0],&Font16,BLACK,WHITE);
        Paint_DrawNum(200,(Font16.Height+2)*2,alarm[1],&Font16,BLACK,WHITE);
        Paint_DrawNum(200,(Font16.Height+2)*3,titel,&Font16,BLACK,WHITE);
        Paint_DrawNum(200,(Font16.Height+2)*4,volume*5,&Font16,BLACK,WHITE);
        Paint_DrawString_EN(200,(Font16.Height+2)*5,timezones[timezone+1],&Font16,WHITE,BLACK);
        Paint_DrawChar(0,(Font16.Height+2)*position,'>',&Font16,BLACK,WHITE);

    }
            
}

bool paintmusicmode(clock_mode mode,int volume){
    if (mode==musicmode_music_off||mode==musicmode_music_on){
    Paint_DrawString_EN(10,10, "Musikmodus ^Play/Pause^",&Font16,WHITE,BLACK);
    if (volume==0){
        Paint_DrawChar(EPD_2IN9_V2_HEIGHT-10-Font16.Width*4,EPD_2IN9_V2_WIDTH-10-Font16.Height,volume*5,&Font16,BLACK,WHITE);
    }
    else{
        Paint_DrawNum(EPD_2IN9_V2_HEIGHT-10-Font16.Width*4,EPD_2IN9_V2_WIDTH-10-Font16.Height,'0',&Font16,BLACK,WHITE);
    }
    return true;
}
else{
    return false;
}
}
void startmode(clock_mode mode,int alarmmode,int timezone, int alarm[],int titel,int volume){
    Paint_Clear(WHITE);
    paintalarm(mode,alarm);
    
    
    paintringtone(mode,titel);
    
    paintsettings(mode,alarmmode,timezone,alarm,titel,volume);
    
    painttime(mode,timezone);
    
    paintweekday(mode);
    paintmusicmode(mode,volume);
    sleep_ms(200);
    EPD_2IN9_V2_Display_Partial(BlackImage);
    
}
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param position position of pointer
 * @param startingvalue starting value of the position
 * 
 * @return returns new int 
 */
int positionselected(clock_mode mode){
    if (mode==selectedclock||mode==selectedmusic){
        if (position==6){
            if (mode==selectedclock){
                return 1;
            }
            else{
            return 2;
        }
        }
        else{
            
            if (gpio_get(plus)==1){
                startingvalue +=1;
            }
            else if (gpio_get(minus)==1)
            {
                startingvalue -=1;
            }
            if(gpio_get(plus)==1||gpio_get(minus)==1){
            Paint_ClearWindows(200,(Font16.Height+2)*position,EPD_2IN9_V2_HEIGHT,(Font16.Height+2)*position+Font16.Height,WHITE);
                switch (position){
            case 0:
            // position 0: 0...off, 1...Mo-Fr, 2...Mo-Su
                if (startingvalue >=3){
                    startingvalue = 0;
                }
                if (startingvalue < 0){
                    startingvalue =2;
                }
                Paint_DrawString_EN(200,0,alarmmodes[startingvalue],&Font16,WHITE,BLACK);
                break;
            case 1:
            //position 1: hour when the alarm will be triggerd
                if(startingvalue >=24){
                    startingvalue = 0;
                }
                if(startingvalue<0){
                    startingvalue = 23;
                }
                if(startingvalue==0){
                    Paint_DrawChar(200,Font16.Height+2,'0',&Font16,BLACK,WHITE);
                }
                else {
                    Paint_DrawNum(200,Font16.Height+2,startingvalue,&Font16,BLACK,WHITE);
                }
                break;
            case 2:
            //position 2:minute when the alarm will be triggered
                if(startingvalue >=59){
                    startingvalue = 0;
                }
                if(startingvalue<0){
                    startingvalue = 59;
                }
                if(startingvalue==0){
                    Paint_DrawChar(200,(Font16.Height+2)*2,'0',&Font16,BLACK,WHITE);
                }
                else {
                    Paint_DrawNum(200,(Font16.Height+2)*2,startingvalue,&Font16,BLACK,WHITE); 
                }
                break;
            case 3:
            //position 3: ringtone
                if(startingvalue >=(sizeof(ringtones)/sizeof(ringtones[0]))){
                    //size of ringtones array
                    startingvalue = 1;
                }
                if(startingvalue<0){
                    startingvalue = (sizeof(ringtones)/sizeof(ringtones[0]));
                }
                Paint_DrawNum(200,(Font16.Height+2)*3,startingvalue,&Font16,BLACK,WHITE);
                break;
            case 4:
            //position 4: volume 0-6
            if(startingvalue >=6){
                startingvalue =6;
                }
            if(startingvalue<0){
                startingvalue = 0;
                }
            if (startingvalue==0){
                Paint_DrawChar(200,(Font16.Height+2)*3,'0',&Font16,BLACK,WHITE); 
            }
            Paint_DrawNum(200,(Font16.Height+2)*3,startingvalue,&Font16,BLACK,WHITE); 
            break;
            
        }
            
            EPD_2IN9_V2_Display_Partial(BlackImage);
            return 0;
            }
            else{
                return 0;
            }
            
            
        }
        
        }
        
        
    
    else {
        return 0;
    }
}
int drivemusicmode(clock_mode mode){
    if (mode==musicmode_music_on) {
        if (gpio_get(topbutton)==0){
            dfplayer_pause(&dfplayer);
            return 3;

        }
        if (gpio_get(plus)==1){
            dfplayer_next(&dfplayer);
            return 0;
    }
    else if (gpio_get(minus)==1){
        dfplayer_previous(&dfplayer);
        return 0;
    }
    
    
}
else if(gpio_get(topbutton)==0&&(mode==musicmode_music_off)){
    dfplayer_resume(&dfplayer,0);
            return 2;
    }
else {return -1;}
    
}
void triggeralarm(clock_mode mode, int ringtone, int volume){
    
    playmusic(mode,ringtone,volume);
    while (true)
    {
        if(gpio_get(menu)==1){
            dfplayer_pause(&dfplayer);
            return;
        }  
        if(gpio_get(topbutton)==0){
            dfplayer_pause(&dfplayer);
            snooze=5;
            return;
        }
        sleep_ms(200);
    }
        
    
    
}

bool timeupdate(clock_mode mode,int timezone,int alarm[],int alarmmod,int ringtone, int volume){
    if (mode==(clockmode||musicmode_music_off||musicmode_music_on)){
       struct tm alarmt;
        aon_timer_get_time_calendar(&alarmt);
        
        if (t.tm_min!=alarmt.tm_min){
            painttime(mode,timezone);
            if (snooze>=0){
            snooze-=1;
            }
            if(alarmt.tm_min%5==0){
                startmode(mode,alarmmod,timezone,alarm,ringtone,volume);
            }
            if ((alarmt.tm_min==alarm[1]&&alarm[0]==alarmt.tm_hour)||snooze==0){
                switch (alarmmod){
                    case 1:
                    if (alarmt.tm_wday>0&&alarmt.tm_wday!=6){
                        triggeralarm(mode,ringtone,volume);
                    }
                    break;
                    case 2:
                    triggeralarm(mode,ringtone,volume);
                }

            }
        EPD_2IN9_V2_Display_Partial(BlackImage);
        return true;
        }
        else{
            return false;
        }
    }
    else{return false;}
}
int drivesettings(clock_mode mode){
    
    if(mode==settingsclock||mode==settingsmusic){
    if (gpio_get(minus)==1){
        position -= 1;
    }
    else if (gpio_get(plus)==1)
    {
        position += 1;
    }
    else{
        return 0;
    }
    if (position>6){
        switch(mode){
            case settingsclock:
            return -1;
            break;
            case settingsmusic:
            return -2;
            break;
        }
        
    }
    if (position<0){
        position=6;
    }
    if (gpio_get(minus)==1||gpio_get(plus)==1){
        Paint_ClearWindows(0,0,20,EPD_2IN9_V2_WIDTH,WHITE);
        Paint_DrawChar(0,(Font16.Height+2)*position,'>',&Font16,BLACK,WHITE);
        EPD_2IN9_V2_Display_Partial(BlackImage);
    }
    return 0;
    
}
else{
    return 0;
}
}
int returnposition(){
    return position;
}

