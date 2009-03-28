/*******************************************************
 * password.h
 *
 * Author(s): David Brenner, Cory Merkel
 *
 * Handles password hashing, comparisons and saving
 *
 *
 *******************************************************/
#ifndef __HASH_H__
#define __HASH_H__

/* Global functions */
void SetPassword(const char* pcPassword);
int  CheckPassword(const char* pcPassword);

#endif
