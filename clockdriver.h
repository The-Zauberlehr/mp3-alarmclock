#include <pico/stdlib.h>

#define plus 26
#define minus 27
#define menu 21
#define wakeuppin 0

  

#define topbutton 1

typedef enum{
    clockmode=1,
    musicmode_music_off=2,
    musicmode_music_on=3,
    settingsclock=4,
    settingsmusic=5,
    selectedmusic=6,
    selectedclock=7


}clock_mode;

typedef enum{
    playone = 1,
    loop = 2
}music;
/**
 * @defgroup functions Functions
 * @{
 */

/**
 * @brief init all modules
 *
 * @param Blackimage  
 * @return -1 if not succesful
 */
int initall();

/**
 * @brief paint the alarm or curently playing song.
 *
 * @param mode the current status of the clock.
 * @param ringtone the set ringtone
 * @return true if succesful
 */
bool paintringtone(clock_mode mode,int ringtone );
/**
 * @brief paint the weekday
 *
 * @param mode the current status of the clock.
 * @return true if succesful
 */
bool paintweekday(clock_mode mode);
/**
 * @brief paint the curent alarm
 *
 * @param mode the current status of the clock.
 * @param alarm array 0...hour, 1...minute when the alarm will be triggered
 * @return true if succesful
 */
bool paintalarm(clock_mode mode,int alarm[]);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param timezone the offset to mez
 * @return true if succesful
 */
bool painttime (clock_mode mode, int timezone);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 
 * @param titel titel to play
 @param volume 0-6 
 * @return true if succesful
 */
bool playmusic (clock_mode mode, int titel, int volume);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param alarmmod 1...alarm off, 2... alarm Mo-Fr, 3...alarm Mo-Su
 * @param timezone offset to cet
 * @param alarm array 0...hour, 1...minute
 * @param titel selected ringtone
 * @param volume selected volume 0-6
 * @param position position of pointer
 * 
 * @return true if succesful
 */
bool paintsettings(clock_mode mode,int alarmmod,int timezone, int alarm[],int titel,int volume);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param volume selected volume 0-6
 * @return true if succesful
 */
bool paintmusicmode(clock_mode mode, int volume);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param alarmmod 1...alarm off, 2... alarm Mo-Fr, 3...alarm Mo-Su
 * @param timezone offset to cet
 * @param alarm array 0...hour, 1...minute
 * @param titel selected ringtone
 * @param volume selected volume 0-6
 * @param image BlackImage
 * @return true if succesful
 */
void startmode(clock_mode,int alarmmod,int timezone, int alarm[],int titel,int volume);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param position position of pointer
 * @param startingvalue starting value of the position
 * @param image Blackimage
 * @return returns new int 
 */

int positionselected(clock_mode mode);
/**
 * @brief activate button functions
 *
 * @param mode the current status of the clock.
 
 * @return returns -1 if nothing happend, 0 if track was skipped forward or backward, 3 if music was paused,2 if music was resumed
 */
int drivemusicmode(clock_mode mode);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param ringtone selected ringtone
 * @param volume selected volume 0-6
  */
void triggeralarm(clock_mode mode, int ringtone, int volume);
/**
 * @brief paint the current time
 *
 * @param mode the current status of the clock.
 * @param timezone offset to cet
 * @param alarm array 0...hour, 1...minute
 * @param ringtone selected ringtone
 * @param alarmmod 0...off, 1...Mo-Fr, 2...Mo-Sa
 * @param volume selected volume 0-6
 * @param image BlackImage
 * @return true if succesful
 */
bool timeupdate(clock_mode mode,int timezone,int alarm[],int alarmon,int ringtone, int volume);
int drivesettings(clock_mode mode);
int returnposition();
void setstartingvalue(int startvalue);
int returnstaringvalue();
