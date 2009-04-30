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
#include "Md5.h"
#include <string.h>
#include <stdio.h>

/* Password file */
const char* PW_FILE = "awt.pw";


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
	int i;
	unsigned char* digest;
	FILE* fp = fopen(PW_FILE, "w");
	if(fp)
	{
		digest = MDString(pcPassword);
		for (i = 0; i < 16; i++)
		{
		
			fprintf (fp,"%02x", digest[i]);
		}
		fclose(fp);
	}
	else
	{
		/*TODO: Display error */
	}
	
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
	FILE* fp = fopen(PW_FILE, "r");
	unsigned char byte;
	unsigned char* digest;
	int i;
	if(fp)
	{	
		digest = MDString(pcPassword);
		for (i = 0; i < 16; i++)
		{
			fscanf(fp, "%02x", &byte);
			if(digest[i] != byte)
			{
				fclose(fp);
				return 1;
			}
		}
		fclose(fp);
		return 0;
	}
	else
	{
		/*TODO: Display error */
	}

} /* ComparePassword */
