#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "em_emu.h"
#include "retargetserial.h"

#include "read_char.h"

int32_t readChar()
{
  int32_t c = getchar();

#ifdef SERIAL_ECHO
  if ( c != 0xff && c != 0x00 ) {
    putchar(c);
  }
#endif

  return c;
}
