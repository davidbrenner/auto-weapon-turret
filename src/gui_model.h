/*******************************************************
 * gui_model.h
 *
 * Author(s): David Brenner, Cory Merkel
 *
 * Contains the definition of the GUI model
 *
 *
 *******************************************************/
 #ifndef __GUI_MODEL_H__
 #define __GUI_MODEL_H__
 
 /* Status definitions */
 #define    MODE		    0x02
	#define USER            0x00
	#define AUTO            0x02
 #define    LOCK            0x10
    #define UNLOCKED        0x00
    #define LOCKED          0x10
  #define    CALIBRATE       0x20
    #define DONE            0x00
 
 /* GUI Model struct */   
 struct model
 {
	unsigned char cStatus;   /* 0x00 Status byte */
 }; 
 
 typedef struct model  GUIMODEL;
 typedef struct model* PGUIMODEL;
 
 PGUIMODEL NewGUIModel(void);
 void DeleteGUIModel(PGUIMODEL pModel);
 
 #endif
 
