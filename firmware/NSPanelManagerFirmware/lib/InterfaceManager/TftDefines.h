#define SWITCH_ROOM_BUTTON_ID 10           // ID of button to switch room
#define HOME_LIGHT_COLOR_SLIDER_ID 1       // ID of "kelvin" slider on home page
#define HOME_LIGHT_LEVEL_SLIDER_ID 2       // ID of light level slider on home page
#define CEILING_LIGHTS_MASTER_BUTTON_ID 22 // ID of ceiling lights button on home page
#define TABLE_LIGHTS_MASTER_BUTTON_ID 21   // ID of table lights button on home page
#define ROOM_BUTTON_ID 5                   // ID of button to go into "room"-view
#define SCENES_BUTTON_ID 17                // ID of button to go into Scenes page

#define NSPANELMANAGER_PAGE_NAME "bootscreen"
#define NSPANELMANAGER_TEXT_NAME "bootscreen.t_loading"

//SCREENSAVER PAGE
#define SCREENSAVE_PAGE_NAME "screensaver"
#define SCREENSAVE_PAGE_TEXT_NAME ".curTime"  //this row should be removed and curTime below should be used instead

#define SCREENSAVER_FIRSTVIEW_VARIABLE_NAME "screensaver.firstview" //Variable used to check if Screensaver page is loaded for the first time. Set to 1 when Nextion Screen starts. Variable is used to run code in Nextion screen to hide/show background on screensaver depending on user choice in NSPanel Manager (screensaver.ssBackground). After sceenpage is loaded for the first time this variable is set to 0 so the code just have to be run once. 
#define SCREENSAVER_BACKGROUND_CHOICE_VARIABLE_NAME "screensaver.ssBackground" //Variable represents the user choice Background/No Background for screensaver page that is done in NSPanelManager. 1=Backgroud, 0=No background. Background picture change for all components is done when screensaver page loads for the first time.

//SCREENSAVER TEXT COMPONENT NAMES
#define SCREENSAVER_CURRENT_WEATHER_ICON_TEXT_NAME "curIcon"
#define SCREENSAVER_CURRENT_TEMP_TEXT_NAME "curTemp"
#define SCREENSAVER_CURRENT_MAXMIN_TEXT_NAME "curMaxmin"
#define SCREENSAVER_CURRENT_RAIN_TEXT_NAME "curRain"
#define SCREENSAVER_CURRENT_WIND_TEXT_NAME "curWind"

#define SCREENSAVER_CURRENT_ROOMTEMP_TEXT_NAME "curRoomtemp"
#define SCREENSAVER_CURRENT_ROOMTEMP_ICON_TEXT_NAME "t7"
#define SCREENSAVER_CURRENT_DAY_TEXT_NAME "curDay"
#define SCREENSAVER_CURRENT_TIME_TEXT_NAME "curTime"
#define SCREENSAVER_CURRENT_AMPM_TEXT_NAME "curAMPM"
#define SCREENSAVER_CURRENT_SUNRISE_TEXT_NAME "curSunrise"
#define SCREENSAVER_CURRENT_SUNSET_TEXT_NAME "curSunset"

#define SCREENSAVER_FORECAST_DAY1_TEXT_NAME "forDay1"
#define SCREENSAVER_FORECAST_DAY2_TEXT_NAME "forDay2"
#define SCREENSAVER_FORECAST_DAY3_TEXT_NAME "forDay3"
#define SCREENSAVER_FORECAST_DAY4_TEXT_NAME "forDay4"
#define SCREENSAVER_FORECAST_DAY5_TEXT_NAME "forDay5"

#define SCREENSAVER_FORECAST_ICON1_TEXT_NAME "forIcon1"
#define SCREENSAVER_FORECAST_ICON2_TEXT_NAME "forIcon2"
#define SCREENSAVER_FORECAST_ICON3_TEXT_NAME "forIcon3"
#define SCREENSAVER_FORECAST_ICON4_TEXT_NAME "forIcon4"
#define SCREENSAVER_FORECAST_ICON5_TEXT_NAME "forIcon5"

#define SCREENSAVER_FORECAST_MAXMIN1_TEXT_NAME "forMaxmin1"
#define SCREENSAVER_FORECAST_MAXMIN2_TEXT_NAME "forMaxmin2"
#define SCREENSAVER_FORECAST_MAXMIN3_TEXT_NAME "forMaxmin3"
#define SCREENSAVER_FORECAST_MAXMIN4_TEXT_NAME "forMaxmin4"
#define SCREENSAVER_FORECAST_MAXMIN5_TEXT_NAME "forMaxmin5"

#define SCREENSAVER_FORECAST_RAIN1_TEXT_NAME "forRain1"
#define SCREENSAVER_FORECAST_RAIN2_TEXT_NAME "forRain2"
#define SCREENSAVER_FORECAST_RAIN3_TEXT_NAME "forRain3"
#define SCREENSAVER_FORECAST_RAIN4_TEXT_NAME "forRain4"
#define SCREENSAVER_FORECAST_RAIN5_TEXT_NAME "forRain5"

#define SCREENSAVER_FORECAST_WIND1_TEXT_NAME "forWind1"
#define SCREENSAVER_FORECAST_WIND2_TEXT_NAME "forWind2"
#define SCREENSAVER_FORECAST_WIND3_TEXT_NAME "forWind3"
#define SCREENSAVER_FORECAST_WIND4_TEXT_NAME "forWind4"
#define SCREENSAVER_FORECAST_WIND5_TEXT_NAME "forWind5"

//SCREENSAVER TEXT COMPONENT ID's
#define SCREENSAVER_CURRENT_WEATHER_ICON_TEXT_ID 28
#define SCREENSAVER_CURRENT_TEMP_TEXT_ID 29
#define SCREENSAVER_CURRENT_MAXMIN_TEXT_ID 30
#define SCREENSAVER_CURRENT_RAIN_TEXT_ID 32
#define SCREENSAVER_CURRENT_WIND_TEXT_ID 34

#define SCREENSAVER_CURRENT_ROOMTEMP_TEXT_ID 46
#define SCREENSAVER_CURRENT_ROOMTEMP_ICON_TEXT_ID 6
#define SCREENSAVER_CURRENT_DAY_TEXT_ID 44
#define SCREENSAVER_CURRENT_TIME_TEXT_ID 35
#define SCREENSAVER_CURRENT_AMPM_TEXT_ID 36
#define SCREENSAVER_CURRENT_SUNRISE_TEXT_ID 38
#define SCREENSAVER_CURRENT_SUNSET_TEXT_ID 40

#define SCREENSAVER_FORECAST_DAY1_TEXT_ID 1
#define SCREENSAVER_FORECAST_DAY2_TEXT_ID 2
#define SCREENSAVER_FORECAST_DAY3_TEXT_ID 3
#define SCREENSAVER_FORECAST_DAY4_TEXT_ID 4
#define SCREENSAVER_FORECAST_DAY5_TEXT_ID 5

#define SCREENSAVER_FORECAST_ICON1_TEXT_ID 6
#define SCREENSAVER_FORECAST_ICON2_TEXT_ID 7
#define SCREENSAVER_FORECAST_ICON3_TEXT_ID 8
#define SCREENSAVER_FORECAST_ICON4_TEXT_ID 9
#define SCREENSAVER_FORECAST_ICON5_TEXT_ID 10

#define SCREENSAVER_FORECAST_MAXMIN1_TEXT_ID 11
#define SCREENSAVER_FORECAST_MAXMIN2_TEXT_ID 12
#define SCREENSAVER_FORECAST_MAXMIN3_TEXT_ID 13
#define SCREENSAVER_FORECAST_MAXMIN4_TEXT_ID 14
#define SCREENSAVER_FORECAST_MAXMIN5_TEXT_ID 15

#define SCREENSAVER_FORECAST_RAIN1_TEXT_ID 16
#define SCREENSAVER_FORECAST_RAIN2_TEXT_ID 17
#define SCREENSAVER_FORECAST_RAIN3_TEXT_ID 18
#define SCREENSAVER_FORECAST_RAIN4_TEXT_ID 19
#define SCREENSAVER_FORECAST_RAIN5_TEXT_ID 20

#define SCREENSAVER_FORECAST_WIND1_TEXT_ID 21
#define SCREENSAVER_FORECAST_WIND2_TEXT_ID 22
#define SCREENSAVER_FORECAST_WIND3_TEXT_ID 23
#define SCREENSAVER_FORECAST_WIND4_TEXT_ID 24
#define SCREENSAVER_FORECAST_WIND5_TEXT_ID 25


//SCREENSAVER2 PAGE //Used for users that only wants time on screensaver or a completely black page
#define SCREENSAVE2_PAGE_NAME "screensaver2"

#define SCREENSAVER2_FIRSTVIEW_VARIABLE_NAME "screensaver2.firstview" //Variable used to check if Screensaver page is loaded for the first time. Set to 1 when Nextion Screen starts. Variable is used to run code in Nextion screen to hide/show background on screensaver depending on user choice in NSPanel Manager (screensaver.ssBackground). After sceenpage is loaded for the first time this variable is set to 0 so the code just have to be run once. 
#define SCREENSAVER2_BACKGROUND_CHOICE_VARIABLE_NAME "screensaver2.ssBackground" //Variable represents the user choice Background/No Background for screensaver page that is done in NSPanelManager. 1=Backgroud, 0=No background. Background picture change for all components is done when screensaver page loads for the first time.

//SCREENSAVER2 TEXT COMPONENT NAMES

#define SCREENSAVER2_CURRENT_ROOMTEMP_TEXT_NAME "curRoomtemp"
#define SCREENSAVER2_CURRENT_ROOMTEMP_ICON_TEXT_NAME "t7"
#define SCREENSAVER2_CURRENT_DAY_TEXT_NAME "curDay"
#define SCREENSAVER2_CURRENT_TIME_TEXT_NAME "curTime"
#define SCREENSAVER2_CURRENT_AMPM_TEXT_NAME "curAMPM"

//SCREENSAVER TEXT COMPONENT ID's

#define SCREENSAVER2_CURRENT_ROOMTEMP_TEXT_ID 7
#define SCREENSAVER2_CURRENT_ROOMTEMP_ICON_TEXT_ID 6
#define SCREENSAVER2_CURRENT_DAY_TEXT_ID 5
#define SCREENSAVER2_CURRENT_TIME_TEXT_ID 1
#define SCREENSAVER2_CURRENT_AMPM_TEXT_ID 9


#define HOME_PIC_HIGHLIGHT_CEILING_NAME "p_lockceiling" // Should not have "home." in front as visibility command for Nextion is not as other commands
#define HOME_PIC_HIGHLIGHT_TABLE_NAME "p_locktable"     // Should not have "home." in front as visibility command for Nextion is not as other commands
#define HOME_PAGE_ID 2                                  // ID of "Home"-page
#define HOME_PAGE_NAME "home"
#define HOME_DIMMER_SLIDER_NAME "home.s_brightness"
#define HOME_LIGHT_COLOR_SLIDER_NAME "home.s_kelvin"
#define HOME_LABEL_CEILING_BRIGHTNESS "home.n_ceiling"
#define HOME_LABEL_TABLE_BRIGHTNESS "home.n_table"
#define HOME_BUTTON_CEILING_NAME "home.b_ceiling"
#define HOME_BUTTON_TABLE_NAME "home.b_table"
#define HOME_BUTTON_SCENES_NAME "home.b_scenes"
#define HOME_BUTTON_SCENES_ROOM_MODE_PIC 59  // Pictures to use on button when panel is in Room Lights mode
#define HOME_BUTTON_SCENES_ROOM_MODE_PIC2 60 // Pictures to use on button when panel is in Room Lights mode
#define HOME_BUTTON_SCENES_ALL_MODE_PIC 61   // Pictures to use on button when panel is in All Lights mode
#define HOME_BUTTON_SCENES_ALL_MODE_PIC2 62  // Pictures to use on button when panel is in All Lights mode
#define HOME_PAGE_SCREENSAVER_TIMER_NAME "home.sleep_display"
#define HOME_PAGE_ROOM_LABEL_NAME HOME_PAGE_NAME ".room"
#define HOME_PAGE_ROOM_LABEL_ID 5
#define HOME_PAGE_SWITCH_ROOM_BUTTON_ID 10
#define HOME_PAGE_MODE_LABEL_NAME HOME_PAGE_NAME ".mode"
#define HOME_PAGE_MODE_LABEL_ID 6
#define SWITCH_MODE_BUTTON_ID 6 // ID of button to change light mode (room/home)
#define HOME_PAGE_SLIDER_NORMAL_COLOR 65535
#define HOME_PAGE_SLIDER_LOCK_COLOR 65024
#define HOME_PAGE_TIME_LABEL_NAME "t_time" // Top right corner. Can be pressed to get to settings page
#define HOME_PAGE_TIME_LABEL_ID 7

#define SCENES_PAGE_NAME "Scenes"
#define SCENES_PAGE_ID 6 // ID for "Scenes"-page
#define SCENES_PAGE_CURRENT_SCENES_LABEL_NAME "scenes_current"
#define SCENES_PAGE_CURRENT_SCENES_LABEL_ID 14
#define SCENES_PAGE_BACK_BUTTON_ID 9
#define SCENES_PAGE_PREVIOUS_SCENES_BUTTON_ID 10
#define SCENES_PAGE_NEXT_SCENES_BUTTON_ID 11
#define SCENES_PAGE_SAVE_SLIDER_NAME "slider_save"

#define SCENES_PAGE_SCENE1_LABEL_NAME "scene1"
#define SCENES_PAGE_SCENE2_LABEL_NAME "scene2"
#define SCENES_PAGE_SCENE3_LABEL_NAME "scene3"
#define SCENES_PAGE_SCENE4_LABEL_NAME "scene4"
#define SCENES_PAGE_SCENE1_LABEL_ID 1
#define SCENES_PAGE_SCENE2_LABEL_ID 2
#define SCENES_PAGE_SCENE3_LABEL_ID 3
#define SCENES_PAGE_SCENE4_LABEL_ID 4
#define SCENES_PAGE_SCENE1_SAVE_BUTTON_NAME "scene1_save"
#define SCENES_PAGE_SCENE2_SAVE_BUTTON_NAME "scene2_save"
#define SCENES_PAGE_SCENE3_SAVE_BUTTON_NAME "scene3_save"
#define SCENES_PAGE_SCENE4_SAVE_BUTTON_NAME "scene4_save"
#define SCENES_PAGE_SCENE1_SAVE_BUTTON_ID 5
#define SCENES_PAGE_SCENE2_SAVE_BUTTON_ID 6
#define SCENES_PAGE_SCENE3_SAVE_BUTTON_ID 7
#define SCENES_PAGE_SCENE4_SAVE_BUTTON_ID 8

#define ROOM_PAGE_NAME "Room"
#define ROOM_PAGE_CURRENT_ROOM_LABEL_NAME "room_current"
#define ROOM_PAGE_ID 4 // ID for "Room"-page
#define ROOM_PAGE_BACK_BUTTON_ID 25
#define ROOM_PAGE_PREVIOUS_ROOM_BUTTON_ID 26
#define ROOM_PAGE_NEXT_ROOM_BUTTON_ID 28

#define ROOM_LIGHT1_LABEL_NAME "device1"
#define ROOM_LIGHT2_LABEL_NAME "device2"
#define ROOM_LIGHT3_LABEL_NAME "device3"
#define ROOM_LIGHT4_LABEL_NAME "device4"
#define ROOM_LIGHT5_LABEL_NAME "device5"
#define ROOM_LIGHT6_LABEL_NAME "device6"
#define ROOM_LIGHT7_LABEL_NAME "device7"
#define ROOM_LIGHT8_LABEL_NAME "device8"
#define ROOM_LIGHT9_LABEL_NAME "device9"
#define ROOM_LIGHT10_LABEL_NAME "device10"
#define ROOM_LIGHT11_LABEL_NAME "device11"
#define ROOM_LIGHT12_LABEL_NAME "device12"
#define ROOM_LIGHT1_LABEL_ID 1
#define ROOM_LIGHT2_LABEL_ID 2
#define ROOM_LIGHT3_LABEL_ID 3
#define ROOM_LIGHT4_LABEL_ID 4
#define ROOM_LIGHT5_LABEL_ID 5
#define ROOM_LIGHT6_LABEL_ID 6
#define ROOM_LIGHT7_LABEL_ID 7
#define ROOM_LIGHT8_LABEL_ID 8
#define ROOM_LIGHT9_LABEL_ID 9
#define ROOM_LIGHT10_LABEL_ID 10
#define ROOM_LIGHT11_LABEL_ID 11
#define ROOM_LIGHT12_LABEL_ID 12

#define ROOM_LIGHT1_SW_NAME "device1_sw"
#define ROOM_LIGHT2_SW_NAME "device2_sw"
#define ROOM_LIGHT3_SW_NAME "device3_sw"
#define ROOM_LIGHT4_SW_NAME "device4_sw"
#define ROOM_LIGHT5_SW_NAME "device5_sw"
#define ROOM_LIGHT6_SW_NAME "device6_sw"
#define ROOM_LIGHT7_SW_NAME "device7_sw"
#define ROOM_LIGHT8_SW_NAME "device8_sw"
#define ROOM_LIGHT9_SW_NAME "device9_sw"
#define ROOM_LIGHT10_SW_NAME "device10_sw"
#define ROOM_LIGHT11_SW_NAME "device11_sw"
#define ROOM_LIGHT12_SW_NAME "device12_sw"
#define ROOM_LIGHT1_SW_ID 13
#define ROOM_LIGHT2_SW_ID 14
#define ROOM_LIGHT3_SW_ID 15
#define ROOM_LIGHT4_SW_ID 16
#define ROOM_LIGHT5_SW_ID 17
#define ROOM_LIGHT6_SW_ID 18
#define ROOM_LIGHT7_SW_ID 19
#define ROOM_LIGHT8_SW_ID 20
#define ROOM_LIGHT9_SW_ID 21
#define ROOM_LIGHT10_SW_ID 22
#define ROOM_LIGHT11_SW_ID 23
#define ROOM_LIGHT12_SW_ID 24
#define ROOM_LIGHT1_SW_CAP_ID 31
#define ROOM_LIGHT2_SW_CAP_ID 32
#define ROOM_LIGHT3_SW_CAP_ID 33
#define ROOM_LIGHT4_SW_CAP_ID 34
#define ROOM_LIGHT5_SW_CAP_ID 35
#define ROOM_LIGHT6_SW_CAP_ID 36
#define ROOM_LIGHT7_SW_CAP_ID 37
#define ROOM_LIGHT8_SW_CAP_ID 38
#define ROOM_LIGHT9_SW_CAP_ID 39
#define ROOM_LIGHT10_SW_CAP_ID 40
#define ROOM_LIGHT11_SW_CAP_ID 41
#define ROOM_LIGHT12_SW_CAP_ID 42

#define LIGHT_PAGE_ID 5
#define LIGHT_PAGE_NAME "Lights"
#define LIGHT_PAGE_BACK_BUTTON_ID 1
#define LIGHT_PAGE_LIGHT_LABEL_NAME "currentLight"
#define LIGHT_PAGE_SWITCH_MODE_BUTTON_ID 8
#define LIGHT_PAGE_SWITCH_MODE_BUTTON_NAME "colormode"
#define LIGHT_PAGE_BRIGHTNESS_SLIDER_ID 7
#define LIGHT_PAGE_BRIGHTNESS_SLIDER_NAME "brightness"
#define LIGHT_PAGE_KELVIN_SLIDER_ID 5
#define LIGHT_PAGE_KELVIN_SLIDER_NAME "kelvin_sat"
#define LIGHT_PAGE_HUE_SLIDER_ID 6
#define LIGHT_PAGE_HUE_SLIDER_NAME "hue"
#define LIGHT_PAGE_KELVIN_SLIDER_PIC 69   // Pic for "currentmode"
#define LIGHT_PAGE_KELVIN_SLIDER_PIC1 70  // Pic for "currentmode"
#define LIGHT_PAGE_SAT_SLIDER_PIC 73      // Pic for "currentmode"
#define LIGHT_PAGE_SAT_SLIDER_PIC1 74     // Pic for "currentmode"
#define LIGHT_PAGE_COLOR_TEMP_MODE_PIC 77 // Pic top right corner
#define LIGHT_PAGE_COLOR_RGB_MODE_PIC 78  // Pic top right corner

#define SETTINGS_PAGE_NAME "settings"
#define SETTINGS_PAGE_ID 3 // ID for "Settings"-page
#define SETTINGS_PAGE_BACK_BUTTON_ID 4
#define SETTINGS_PAGE_LABEL_NAME "label" // Currently in tft file label.txt is set to "Settings"
#define SETTINGS_PAGE_LABEL_ID 5
#define SETTINGS_PAGE_SAVE_SLIDER_NAME "slider_save"
#define SETTINGS_PAGE_SAVE_SLIDER_ID 3
#define SETTINGS_PAGE_RESTART_BUTTON_NAME "b_restart"
#define SETTINGS_PAGE_RESTART_BUTTON_ID 6
#define SETTINGS_PAGE_FACTORYRESET_BUTTON_NAME "b_factory"
#define SETTINGS_PAGE_FACTORYRESET_BUTTON_ID 7
