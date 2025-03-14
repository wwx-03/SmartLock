#include "password.h"

int Password_Input(password_structure_t *password, uint8_t num)
{
  int result = 0;
  if(password->len > 16)
  {
    result = PASSWORD_TOO_LONG;
  }
  else if(num != 0 && num <= 10)
  {
    password->num[password->len] = num % 10;
    password->len ++;
  }
  else
  {
    result = PASSWORD_NUM_ERROR;
  }
  return result;
}

int Password_Back(password_structure_t *password)
{
  int result = 0;
  if(password->len == 0)
  {
    result = PASSWORD_TOO_SHORT;
  }
  else
  {
    password->len--;
    password->num[password->len] = 0;
  }
  return result;
}

int Password_Clear(password_structure_t *password)
{
  for(uint8_t i = 0; i < 16; i ++)
  {
    password->num[i] = 0;
  }
  password->len = 0;
  return 0;
}

int Password_Compare(password_structure_t *destPassword, password_structure_t *password)
{
  int result = PASSWORD_MATCH;
  if(destPassword->len != password->len)
  {
    result = PASSWORD_NO_MATCH;
  }
  else
  {
    for(uint8_t i = 0; i < destPassword->len; i ++)
    {
      if(destPassword->num[i] != password->num[i])
      {
        result = PASSWORD_NO_MATCH;
        break;
      }
    }
  }
  return result;
}
