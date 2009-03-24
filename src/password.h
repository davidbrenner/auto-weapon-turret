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
void SetPassword(char* pcPassword);
int  ComparePassword(const char* pcPassword);

#endif
