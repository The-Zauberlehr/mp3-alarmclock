#include <stdio.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "EPD_2in9_V2.h"
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
int MoFr = 0;
int timezone =0;

clock_mode mode;
int ringtone= 33;
int volume = 15; 
int alarm[2]={6,0};
int settingsvalue;

bool musicon=false;
int c = 0;
int positionidentifier(){
    switch(returnposition()){
         case 0: 
        return MoFr;
        break;
        case 1:
        return alarm[0];
        break;
        case 2: 
        return alarm[1];
        break;
        case 3:
        return ringtone;
        break;
        case 4:
        return volume;
        break;
        case 5:
        return timezone;
        break;
    }
}

void decodeposition(int newint){
    
    switch(returnposition()){
        case 0: 
        MoFr=newint;
        break;
        case 1:
        alarm[0]=newint;
        break;
        case 2: 
        alarm[1]=newint;
        break;
        case 3:
        ringtone=newint;
        break;
        case 4:
        volume=newint;
        break;
        case 5:
        timezone=newint;
        break;
        case 6:
        if(newint==-25){
            mode=musicmode_music_off;
            startmode(mode,MoFr,timezone,alarm,ringtone,volume);

        }
        else if (newint== -26){
             mode=clockmode;
            startmode(mode,MoFr,timezone,alarm,ringtone,volume);

        }
        break;
    }
}
int main()
{
    
    int a;
    initall();
    mode = clockmode;
    startmode(mode,MoFr,timezone,alarm,ringtone,volume);
    sleep_ms(200);
    while(true){
        if (gpio_get(menu)==1){
            switch (mode){
                case clockmode:
                    mode=settingsclock;
                    startmode(mode,MoFr,timezone,alarm,ringtone,volume);
                    break;
                case musicmode_music_off:
                    mode=settingsmusic;
                    musicon=false;
                    startmode(mode,MoFr,timezone,alarm,ringtone,volume);
                    break;
                case musicmode_music_on:
                    mode=settingsmusic;
                    musicon=true;
                    startmode(mode,MoFr,timezone,alarm,ringtone,volume);
                    break;
                case settingsclock:
                    mode =selectedclock;
                    setstartingvalue(positionidentifier());
                    
                    
                    break;
                case settingsmusic:
                    mode =selectedmusic;
                    setstartingvalue(positionidentifier());
                    
                    break;
                case selectedmusic:
                    
                    decodeposition(returnstaringvalue());
                    mode = settingsmusic;
                    break;
                case selectedclock:
                    decodeposition(returnstaringvalue());
                    mode = settingsclock;
                    break;

                }
                
            }
        
        switch (drivesettings(mode))
        {
        case -1:
            mode= clockmode;
            startmode(mode,MoFr,timezone,alarm,ringtone,volume);
            break;
        case -2:
            if (musicon){
                mode=musicmode_music_on;
            }
            else{
                mode=musicmode_music_off;
            }
            startmode(mode,MoFr,timezone,alarm,ringtone,volume);
            break;
        
        default:
            break;
        }
        positionselected(mode);
        switch (positionselected(mode))
        {
        case 1:
            decodeposition(-25);
            break;
        case 2:
            decodeposition(-26);
        break;
        default:
            break;
        }

        
        timeupdate(mode,timezone,alarm,MoFr,ringtone,volume);
        drivemusicmode(mode);
        sleep_ms(100);

    }
}
