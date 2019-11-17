#include <machine/int86.h>
#include <lcom/vbe.h>
#include <stdbool.h>
#include "video.h"
#include "vbe_constants.h"

extern void* frame_buffer;
static vbe_mode_info_t info;


void* (vg_init)(uint16_t mode) {
  static void* video_mem;

  memset(&info, 0, sizeof(info)); // Zero the struct

  if (vbe_get_mode_info(mode, &info)) {   // TODO: Implement our own version of vbe_get_mode_info
    return MAP_FAILED;
  }

  unsigned int vram_size = info.XResolution * info.YResolution * info.BitsPerPixel / 8;
  
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
  memset(&reg, 0, sizeof(reg)); // Zero the struct

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

  uint8_t bytes_per_pixel = info.BitsPerPixel / BITS_PER_BYTE;
  uint8_t* start_addr = (uint8_t*)(frame_buffer) + bytes_per_pixel * (x + y * info.XResolution);

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

  if (info.MemoryModel != VBE_PACKED_PIXEL && info.MemoryModel != VBE_DIRECT_COLOR) {
    printf("Error occurred: unexpected memory model.\n");
    return 1;
  }
  
  bool indexed = (info.MemoryModel == VBE_PACKED_PIXEL);

  /* Width and height of each rectangle */
  uint16_t rect_width = info.XResolution / no_rectangles;
  uint16_t rect_height = info.YResolution / no_rectangles;

  /* Current position and color */
  uint16_t x_pos = 0, y_pos = 0;
  uint32_t current_color;

  /* Color components of first color */
  uint16_t red_first = (BITMASK(info.RedFieldPosition, info.RedMaskSize) & first_color) >> info.RedFieldPosition;
  uint16_t green_first = (BITMASK(info.GreenFieldPosition, info.GreenMaskSize) & first_color) >> info.GreenFieldPosition;
  uint16_t blue_first = (BITMASK(info.BlueFieldPosition, info.BlueMaskSize) & first_color) >> info.BlueFieldPosition;
  
  /* Color components for direct mode */
  uint16_t red, green, blue;

  for (uint8_t row = 0; row < no_rectangles; row++) {
    for (uint8_t col = 0; col < no_rectangles; col++) {
      /* Calculate rectangle position and color */
      x_pos = rect_width * col;
      y_pos = rect_height * row;
      
      if (indexed) { /* Indexed mode */
        current_color = (first_color + (row * no_rectangles + col) * step) % (1 << info.BitsPerPixel);
      }
      else { /* Direct mode */
        red = (red_first + col * step) % (1 << info.RedMaskSize);
        green = (green_first + row * step) % (1 << info.GreenMaskSize);
        blue = (blue_first + (col + row) * step) % (1 << info.BlueMaskSize);

        red &= BITMASK(0, info.RedMaskSize);
        green &= BITMASK(0, info.GreenMaskSize);
        blue &= BITMASK(0, info.BlueMaskSize);

        current_color = (red << info.RedFieldPosition) | (green << info.GreenFieldPosition) | (blue << info.BlueFieldPosition);
      }

      vg_draw_rectangle(x_pos, y_pos, rect_width, rect_height, current_color);
    }
  }

  return 0;
}
