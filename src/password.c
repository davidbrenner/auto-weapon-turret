/*******************************************************
 * password.c
 *
 * Author(s): David Brenner, Cory Merkel
 *
 * Handles password hashing, comparisons and saving
 *
 *
 *******************************************************/
#include "password.h"
#include <string.h>

/*******************************************************
 * Function: SetPassword
 *
 * Args:     pcPassword -- Password String
 *
 * Returns:  void
 *
 * Hashes and saves the given password string
 *
 *
 *******************************************************/
void SetPassword(char* pcPassword)
{
	
} /* SetPassword */


/*******************************************************
 * Function: ComparePassword
 *
 * Args:     pcPassword -- Password String
 *
 * Returns:  0 -- Password does not match saved password
 *           else -- Password matches saved password
 *
 * Compares the given password with the saved password
 *
 *
 *******************************************************/
int  ComparePassword(const char* pcPassword)
{
	return strcmp(pcPassword, "password");
} /* ComparePassword */

