#include <machine/int86.h>
#include <lcom/vbe.h>
#include <lcom/lcf.h>
#include <stdbool.h>
#include <limits.h>

#include "video.h"
#include "vbe_constants.h"
#include "i8254.h"

extern void *frame_buffer;
static vbe_mode_info_t info;
static uint8_t bytes_per_pixel;

void *(vg_init)(uint16_t mode) {
  static void *video_mem;

  memset(&info, 0, sizeof(info)); // Zero the struct

  if (vbe_return_mode_info(mode, &info)) {
    return MAP_FAILED;
  }

  bytes_per_pixel = info.BitsPerPixel / 8;
  if (info.BitsPerPixel % 8 != 0) {
    // Bits per pixel isn't a multiple of 8: add an extra byte
    ++bytes_per_pixel;
  }

  unsigned int vram_size = info.XResolution * info.YResolution * bytes_per_pixel;
  
  struct minix_mem_range mr;
  mr.mr_base = info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
    printf("Error when calling sys_privctl.\n");
    return MAP_FAILED;
  }

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_set_mode(mode)) {
    printf("Error when calling video_set_mode.\n");
    return MAP_FAILED;
  }

  return video_mem;
}


int vbe_return_mode_info(uint16_t mode, vbe_mode_info_t *info_ptr) {
  if (info_ptr == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  mmap_t map;
  lm_alloc(sizeof(*info_ptr), &map);
  phys_bytes buf = map.phys;

  struct reg86 reg;
  memset(&reg, 0, sizeof(reg)); // Zero the reg86 struct

  /* Set register values */
  reg.intno = SERVICE_VIDEO_CARD;
  reg.ax = VBE_FUNC_AX(FUNC_RETURN_VBE_MODE_INFO);
  reg.cx = VBE_MODE_LINEAR(mode);
  reg.es = PB2BASE(buf);
  reg.di = PB2OFF(buf);

  /* BIOS call */
  if (sys_int86(&reg) != OK) {
    lm_free(&map);
    printf("Error when calling sys_int86.\n");
    return 1;
  }

  /* Check return value  */
  if (reg.ah != VBE_RETURN_SUCCESS) {
    lm_free(&map);
    printf("Error occurred: VBE function not successful (AH = 0x%x).\n", reg.ah);
    return 1;
  }

  if (reg.al != VBE_RETURN_FUNC_SUPPORTED) {
    lm_free(&map);
    printf("Error occurred: VBE function not supported.\n");
    return 1;
  }

  *info_ptr = *((vbe_mode_info_t *)(map.virt));
  lm_free(&map);

  return 0;
}


int video_set_mode(uint16_t mode) {
  struct reg86 reg;
  memset(&reg, 0, sizeof(reg)); // Zero the struct

  /* Set register values */
  reg.intno = SERVICE_VIDEO_CARD;
  reg.ax = VBE_FUNC_AX(FUNC_SET_VBE_MODE);
  reg.bx = VBE_MODE_LINEAR(mode);

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


int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color, void **buffer) {
  if (buffer == NULL) {
    printf("Error occurred: buffer not set.\n");
    return 1;
  }

  color &= BITMASK(0, info.BitsPerPixel);

  uint8_t *addr = (uint8_t *)(*buffer) + bytes_per_pixel * (x + y * info.XResolution);
  memcpy(addr, &color, bytes_per_pixel);

  return 0;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (frame_buffer == NULL) {
    printf("Error occurred: frame buffer not set.\n");
    return 1;
  }

  for (uint16_t i = 0; i < len; i++) {
    vg_draw_pixel(x + i, y, color, &frame_buffer);
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


int vg_draw_xpm(uint8_t *pixmap, xpm_image_t img, uint16_t x, uint16_t y, bool double_buffered) {
  if (pixmap == NULL) {
    printf("Error occurred: null pixmap.\n");
    return 1;
  }

  if (frame_buffer == NULL) {
    printf("Error occurred: frame buffer not set.\n");
    return 1;
  }

  uint32_t transparency_color = xpm_transparency_color(XPM_INDEXED);
  uint32_t current_color;

  /* Auxiliary buffer used for double buffering */
  void *aux_buffer;
  if (double_buffered) {
    /* Allocate required memory */
    aux_buffer = malloc(info.XResolution * info.YResolution * bytes_per_pixel);
  }

  /* Draw pixelmap */
  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      current_color = pixmap[col + row * img.width];
      
      if (current_color != transparency_color) {
        if (double_buffered) {
          vg_draw_pixel(x + col, y + row, pixmap[col + row * img.width], &(aux_buffer));
        }
        else {
          vg_draw_pixel(x + col, y + row, pixmap[col + row * img.width], &frame_buffer);
        }
      }
    }
  }

  if (double_buffered) {
    /* Copy resulting image to frame buffer */
    memcpy(frame_buffer, aux_buffer, info.XResolution * info.YResolution * bytes_per_pixel);
  }

  return 0;
}


int vg_get_movement_info(uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, 
uint8_t fr_rate, movement_info *info) {
  if (fr_rate == 0 || (xi != xf && yi != yf)) {
    return 1;
  }

  /* Calculate interrupt count and step */
  if (speed == 0) {
    info->int_count = UCHAR_MAX;
    info->type = NO_MOVEMENT;
  }
  else if (speed > 0) {
    info->int_count = TIMER0_INTERRUPTS_PER_SECOND / fr_rate;
    info->step = speed;
  }
  else {
    info->int_count = TIMER0_INTERRUPTS_PER_SECOND / fr_rate * (-speed);
    info->step = 1;
  }

  /* Calculate movement type */
  if (xi == xf) {
    if (yi == yf) {
      info->type = NO_MOVEMENT;
    }
    else if (yi < yf) {
      info->type = POSITIVE_Y;
    }
    else {
      info->type = NEGATIVE_Y;
    }
  }
  else {
    if (xi < xf) {
      info->type = POSITIVE_X;
    }
    else {
      info->type = NEGATIVE_X;
    }
  }

  /* Final adjustments to step */
  if (info->type == NO_MOVEMENT) {
    info->step = 0;
  }
  else if (info->type == NEGATIVE_X || info->type == NEGATIVE_Y) {
    info->step = -info->step;
  }

  return 0;
}

int vbe_return_controller_info(vg_vbe_contr_info_t *info_p) {
  if (info_p == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  mmap_t map;
  /* Allocate low memory */
  void* virt = lm_alloc(VBE_INFO_BLOCK_SIZE, &map);
  phys_bytes buf = map.phys;

  struct reg86 reg;
  memset(&reg, 0, sizeof(reg)); // Zero the reg86 struct

  /* Set register values */
  reg.intno = SERVICE_VIDEO_CARD;
  reg.ax = VBE_FUNC_AX(FUNC_RETURN_VBE_CONTROLLER_INFO);
  reg.es = PB2BASE(buf);
  reg.di = PB2OFF(buf);

  vbe_info_block *info_ptr = (vbe_info_block *)(map.virt);
  strcpy(((vg_vbe_contr_info_t *)(virt))->VBESignature, "VBE2");

  /* BIOS call */
  if (sys_int86(&reg) != OK) {
    lm_free(&map);
    printf("Error when calling sys_int86.\n");
    return 1;
  }

  /* Check return value */
  if (reg.ah != VBE_RETURN_SUCCESS) {
    lm_free(&map);
    printf("Error occurred: VBE function not successful (AH = 0x%x).\n", reg.ah);
    return 1;
  }

  if (reg.al != VBE_RETURN_FUNC_SUPPORTED) {
    lm_free(&map);
    printf("Error occurred: VBE function not supported.\n");
    return 1;
  }

  uint8_t msb, lsb;

  if (util_get_MSB((*info_ptr).VBEVersion, &msb)) {
  }

  if (util_get_LSB((*info_ptr).VBEVersion, &lsb)) {
  }

  /* Copying and parsing the selected fields from VbeInfoBlock to vg_vbe_contr_info_t */
  strcpy((*info_p).VBESignature, (*info_ptr).VBESignature);
  (*info_p).VBEVersion[0] = lsb;
  (*info_p).VBEVersion[1] = msb;

  (*info_p).TotalMemory = (*info_ptr).TotalMemory * 64;

  uint32_t base = (uint32_t)virt - map.phys;
  (*info_p).VideoModeList = (uint16_t *)(base + far_ptr_to_linear((*info_ptr).VideoModePtr));
  (*info_p).OEMString = (char *)(base + far_ptr_to_linear((*info_ptr).OEMStringPtr));
  (*info_p).OEMVendorNamePtr = (char *)(base + far_ptr_to_linear((*info_ptr).OEMVendorNamePtr));
  (*info_p).OEMProductNamePtr = (char *)(base + far_ptr_to_linear((*info_ptr).OEMProductNamePtr));
  (*info_p).OEMProductRevPtr = (char *)(base + far_ptr_to_linear((*info_ptr).OEMProductRevPtr));

  lm_free(&map);

  return 0;
}

uint32_t far_ptr_to_linear(uint32_t far_ptr) {
  uint32_t base = (far_ptr >> (BITS_PER_BYTE * 2)) << 4;
  uint32_t offset = far_ptr & BITMASK(0, BITS_PER_BYTE * 2);

  return base + offset;
}

