#include <machine/int86.h>
#include <lcom/vbe.h>
#include "video.h"
#include "vbe_constants.h"


void *(vg_init)(uint16_t mode) {
  static void *video_mem;

  if (video_set_mode(mode)) {
    printf("Error when calling video_set_mode.\n");
    return MAP_FAILED;
  }

  vbe_mode_info_t info;
  if (vbe_get_mode_info(mode, &info)) {
    return MAP_FAILED;
  }

  unsigned int vram_size = info.XResolution * info.YResolution * info.BitsPerPixel / 8;
  video_mem = vm_map_phys(SELF, (void *) info.PhysBasePtr, vram_size);

  return video_mem;
}

int video_set_mode(uint16_t mode) {
  /* Allow mapping the first 1 MiB of physical memory */

  struct minix_mem_range mr;
  mr.mr_base = (phys_bytes) 0;
  mr.mr_limit = mr.mr_base + MIBIBYTE;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
    printf("Error when calling sys_privctl (ADD_MEM).\n");
    return 1;
  }

  /* Set register values */

  struct reg86 reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = SERVICE_VIDEO_CARD;
  reg.ax = (VBE_FUNCTION_AH << 8) | FUNC_SET_VBE_MODE;
  reg.bx = LINEAR_FRAME_BUFFER_MODEL | mode;

  /* BIOS call */

  if (sys_int86(&reg) != OK) {
    printf("Error when calling sys_int86.\n");
    return 1;
  }

  /* Check return value */

  if (reg.al != VBE_RETURN_FUNC_SUPPORTED) {
    printf("Error occurred: VBE function not supported.\n");
    return 1;
  }

  if (reg.ah != VBE_RETURN_SUCCESS) {
    printf("Error occurred: VBE function not successful.\n");
    return 1;
  }

  return 0;
}
