#ifndef __PASSWORD_H
#define __PASSWORD_H

#include "main.h"

typedef struct
{
  uint8_t len;
  uint8_t num[16];
}password_structure_t;

int Password_Input(password_structure_t *password, uint8_t num);
int Password_Back(password_structure_t *password);
int Password_Clear(password_structure_t *password);
int Password_Compare(password_structure_t *destPassword, password_structure_t *password);

#define PASSWORD_MATCH 0
#define PASSWORD_TOO_LONG -1
#define PASSWORD_TOO_SHORT -2
#define PASSWORD_NUM_ERROR -3
#define PASSWORD_NO_MATCH -4

#endif
