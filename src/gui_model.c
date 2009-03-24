/*******************************************************
 * gui_model.c
 *
 * Author(s): David Brenner, Cory Merkel
 *
 * Contains the definition of the GUI model
 *
 *
 *******************************************************/
 #include "gui_model.h"
 #include <stdlib.h>
 
/*******************************************************
 * Function:  NewGUIModel
 *
 * Args:      None
 *
 * Returns:   Pointer to the new GUIMODEL struct
 *	  
 * Creats and initalizes a new GUIMODEL struct
 *	  
 *******************************************************/ 
 PGUIMODEL NewGUIModel(void)
 {
	PGUIMODEL pModel;
	if( pModel = (PGUIMODEL)malloc(sizeof(GUIMODEL)) )
	{
		pModel->cStatus = 0x00;
		pModel->iTargDist = 0;
		return pModel;
	}
	return NULL;

 } /* NewGUIModel */ 

/*******************************************************
 * Function:  DeleteGUIModel
 *
 * Args:      pModel The GUI model to destroy
 *
 * Returns:   void
 *	  
 * Deallocates memory allocated for pModel
 *	  
 *******************************************************/ 
void DeleteGUIModel(PGUIMODEL pModel)
{
	free( (void*)pModel );
} /* DeleteGUIModel */

