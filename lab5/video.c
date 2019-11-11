#include <machine/int86.h>
#include "video.h"
#include "vbe.h"

int video_set_mode(uint16_t mode) {
  struct reg86 reg;

  memset(&reg, 0, sizeof(reg));

  reg.intno = SERVICE_VIDEO_CARD;
  reg.ax = (VBE_FUNCTION_AH << 8) | FUNC_SET_VBE_MODE;
  reg.bx = LINEAR_FRAME_BUFFER_MODEL | mode;

  if (sys_int86(&reg) != OK) {
    printf("Error when calling sys_int86.\n");
    return 1;
  }

  return 0;
}
