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
void SetPassword(const char* pcPassword)
{
	
} /* SetPassword */


/*******************************************************
 * Function: CheckPassword
 *
 * Args:     pcPassword -- Password String
 *
 * Returns:  0 -- Password does matches saved password
 *           else -- Password doesn't match saved password
 *
 * Compares the given password with the saved password
 *
 *
 *******************************************************/
int  CheckPassword(const char* pcPassword)
{
	return strcmp(pcPassword, "password");
} /* ComparePassword */

