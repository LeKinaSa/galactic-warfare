#include <machine/int86.h>
#include <lcom/vbe.h>
#include "video.h"
#include "vbe_constants.h"

extern void* frame_buffer;

static uint16_t x_res, y_res;
static uint8_t bits_per_pixel;


void* (vg_init)(uint16_t mode) {
  static void* video_mem;

  vbe_mode_info_t info;
  memset(&info, 0, sizeof(info)); // Zero the structure

  if (vbe_get_mode_info(mode, &info)) {   // TODO: Implement our own version of vbe_get_mode_info
    return MAP_FAILED;
  }

  x_res = info.XResolution;
  y_res = info.YResolution;
  bits_per_pixel = info.BitsPerPixel;

  unsigned int vram_size = x_res * y_res * bits_per_pixel / 8;
  
  struct minix_mem_range mr;
  mr.mr_base = info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
    printf("Error when calling sys_privctl.\n");
    return MAP_FAILED;
  }

  video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);

  if (video_set_mode(mode)) {
    printf("Error when calling video_set_mode.\n");
    return MAP_FAILED;
  }

  return video_mem;
}


int video_set_mode(uint16_t mode) {
  /* Set register values */

  struct reg86 reg;
  memset(&reg, 0, sizeof(reg)); // Zero the structure

  reg.intno = SERVICE_VIDEO_CARD;
  reg.ax = VBE_FUNC_AX(FUNC_SET_VBE_MODE);
  reg.bx = VBE_MODE_BX_LINEAR(mode);

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


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (frame_buffer == NULL) {
    printf("Error occurred: frame buffer not set.\n");
    return 1;
  }

  uint8_t bytes_per_pixel = bits_per_pixel / BITS_PER_BYTE;
  uint8_t* start_addr = (uint8_t*)(frame_buffer) + bytes_per_pixel * (x + y * x_res);

  uint8_t bit_shift;

  for (uint16_t i = 0; i < len; i++) {
    for (uint8_t j = 0; j < bytes_per_pixel; j++) {
      bit_shift = j * BITS_PER_BYTE;
      *(start_addr + i * bytes_per_pixel + j) = (uint8_t)(color >> bit_shift);
    }
  }

  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (frame_buffer == NULL) {
    printf("Error occurred: frame buffer not set.\n");
    return 1;
  }

  for (size_t i = 0; i < height; i++) {
    vg_draw_hline(x, y + i, width, color);
  }

  return 0;
}


int vg_draw_pattern(uint8_t no_rectangles, uint32_t first_color, uint8_t step) {
  if (frame_buffer == NULL) {
    printf("Error occurred: frame buffer not set.\n");
    return 1;
  }

  /* Width and height of each rectangle */
  uint16_t rect_width = x_res / no_rectangles;
  uint16_t rect_height = y_res / no_rectangles;

  /* Current position and color */
  uint16_t x_pos = 0, y_pos = 0;
  uint32_t current_color;
  
  for (uint8_t i = 0; i < no_rectangles; i++) {
    for (uint8_t j = 0; j < no_rectangles; j++) {
      /* Calculate rectangle position and color */
      x_pos = rect_width * j;
      y_pos = rect_height * i;

      /* Indexed mode */
      current_color = (first_color + (i * no_rectangles + j) * step) % (1 << bits_per_pixel);

      // TODO: Direct mode

      vg_draw_rectangle(x_pos, y_pos, rect_width, rect_height, current_color);
    }
  }

  return 0;
}
