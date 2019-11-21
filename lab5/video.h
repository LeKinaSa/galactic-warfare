#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>

/* Possible types of movement for video_test_move */
enum movement_type { 
  NO_MOVEMENT, 
  POSITIVE_X, 
  NEGATIVE_X, 
  POSITIVE_Y, 
  NEGATIVE_Y
};

/* Structure with useful information for video_test_move */
typedef struct {
  enum movement_type type;
  int step;
  uint8_t int_count;
} movement_info;

/* Structure to be passed to VBE function 00h */
typedef struct __attribute__((packed)) {
  char VBESignature[4];
  uint16_t VBEVersion;
  uint32_t OEMStringPtr;
  uint8_t Capabilities[4];
  uint32_t VideoModePtr;
  uint16_t TotalMemory;
  uint16_t OEMSoftwareRev;
  uint32_t OEMVendorNamePtr;
  uint32_t OEMProductNamePtr;
  uint32_t OEMProductRevPtr;
  uint8_t Reserved[222];
  uint8_t OEMData[256];
} vbe_info_block;



/* Our implementation of vbe_get_mode_info */
int vbe_return_mode_info(uint16_t mode, vbe_mode_info_t *info_ptr);

/* Sets the operating mode of the video card */
int video_set_mode(uint16_t mode);

/* Draws a pixel of the given color in the specified position to a buffer */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color, void **buffer);

/* Draws a pattern of rectangles according to the video_test_pattern specification */
int vg_draw_pattern(uint8_t no_rectangles, uint32_t first_color, uint8_t step);
int vg_obtain_pixelmap(xpm_map_t xpm, uint8_t *pixelmap, xpm_image_t *img);
void vg_draw_pixelmap(uint8_t *pixelmap, xpm_image_t *img, uint16_t x, uint16_t y);

/* Draws a xpm image to the frame buffer in the specified position. If double_buffered is set to true, uses an auxiliary buffer to prevent flickering */
int vg_draw_xpm(uint8_t *pixmap, xpm_image_t img, uint16_t x, uint16_t y, bool double_buffered);

/* Fills a movement_info struct using video_test_move args. Returns true if args are valid, false otherwise. */
int vg_get_movement_info(uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, 
uint8_t fr_rate, movement_info *info);

/* Gets the controller info */
int vbe_return_controller_info(vg_vbe_contr_info_t *info_p);

uint32_t far_ptr_to_linear(uint32_t far_ptr);

#endif /* __VIDEO_H */
