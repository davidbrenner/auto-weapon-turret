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
 #define    ARMED_UNARMED   0x01
	#define UARMED          0x00
	#define ARMED           0x01
 #define    MODE		    0x02
	#define USER            0x00
	#define AUTO            0x20
 #define    XHAIRS          0x40
    #define OFF             0x00
    #define ON              0x04
 #define    XHAIRS_MOVEABLE 0x08
	#define UNMOVEABLE      0x00
	#define MOVEABLE        0x08
 #define    LOCK            0x10
    #define UNLOCKED        0x00
    #define LOCKED          0x10
 
 /* GUI Model struct */   
 struct model
 {
	char cStatus;   /* 0x00 Status byte */
	int  iTargDist;	/* 0x01 Distance to target in incehs */
 }; 
 
 typedef struct model  GUIMODEL;
 typedef struct model* PGUIMODEL;
 
 PGUIMODEL NewGUIModel(void);
 void DeleteGUIModel(PGUIMODEL pModel);
 
 #endif
 