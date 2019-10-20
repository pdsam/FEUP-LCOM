#include <machine/int86.h>
#include <math.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vMacros.h"
#include "v_gr.h"

static uint8_t *background;
static uint8_t *video_mem; /* Process (virtual) address to which VRAM is mapped */
static uint8_t *buffer_mem;
static uint16_t h_res;        /* Horizontal screen resolution in pixels */
static uint16_t v_res;        /* Vertical screen resolution in pixels */
static uint8_t bits_in_pixel; /* Number of VRAM bits per pixel */
static uint8_t bytes_in_pixel;
static uint8_t green_size, red_size, blue_size;
static uint8_t green_pos, red_pos, blue_pos;

uint16_t get_h_res() { return h_res; }
uint16_t get_v_res() { return v_res; }
uint16_t get_bits_in_pixel() { return bits_in_pixel; }
uint8_t *get_video_mem() { return video_mem; }

int(vbe_get_mode_info_v2)(uint16_t mode, vbe_mode_info_t *vmi_p) {
  mmap_t map;
  struct reg86u r;

  memset(&r, 0, sizeof(r)); // reseting construction

  if (lm_init(true) == NULL) {
    printf("failed init\n");
    return 1;
  }

  if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) {
    printf("failed alloc\n");
    return 1;
  }

  phys_bytes buf = map.phys;

  r.u.w.ax = 0x4F01;
  r.u.w.es = PB2BASE(buf);
  r.u.w.di = PB2OFF(buf);
  r.u.w.cx = mode;
  r.u.b.intno = BIOS_VIDEO_SERVICE;
  if (sys_int86(&r) != OK) {
    panic("get_vbe_mode_info_v2: sys_int86() failed \n");
  }
  *vmi_p = *(vbe_mode_info_t *) map.virt;

  lm_free(&map);
  return 0;
}

void *(vg_init)(uint16_t mode) {

  struct reg86u reg86;
  vbe_mode_info_t mode_info;
  vbe_get_mode_info_v2(mode, &mode_info);

  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  bits_in_pixel = mode_info.BitsPerPixel;
  green_size = mode_info.GreenMaskSize;
  red_size = mode_info.RedMaskSize;
  blue_size = mode_info.BlueMaskSize;
  green_pos = mode_info.GreenFieldPosition;
  red_pos = mode_info.RedFieldPosition;
  blue_pos = mode_info.BlueFieldPosition;

  uint32_t no_pixels = h_res * v_res;
  bytes_in_pixel = ceil(bits_in_pixel / 8.0);

  int r;
  struct minix_mem_range mr;                       /*physical memory range */
  uint32_t vram_base = mode_info.PhysBasePtr;      /*VRAM’s physical addresss*/
  uint32_t vram_size = no_pixels * bytes_in_pixel; /*VRAM’s size,but you can use the frame -buffer size,instead */

  /*Allow memory mapping*/
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }

  /*Map memory*/
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) {
    panic("couldn’t map video memory");
  }

  memset(&reg86, 0, sizeof(reg86)); /* zero the structure */

  reg86.u.b.intno = BIOS_VIDEO_SERVICE;
  reg86.u.w.bx = mode | VBE_SET_LINEAR_BUFFER;
  reg86.u.w.ax = MIX;

  if (sys_int86(&reg86) != OK) {
    printf("\tvg_exit(): sys_int86() failed \n");
    return NULL;
  }

  if (reg86.u.b.ah != 0x00) {
    if (reg86.u.b.ah == 0x01) {
      printf("vg_init::VBE call failed\n");
      return NULL;
    }
    else if (reg86.u.b.ah == 0x02) {
      printf("vg_init::VBE call not supported in current HW configuration\n");
      return NULL;
    }
    else if (reg86.u.b.ah == 0x03) {
      printf("vg_init::VBE call invalid in current video mode\n");
      return NULL;
    }
  }

  buffer_mem = (uint8_t *) malloc(vram_size);
  background = (uint8_t *) malloc(vram_size);

  if (buffer_mem == NULL) {
    return NULL;
  }

  return (void *) video_mem;
}

uint8_t *get_buffer_pointer() {
  return buffer_mem;
}

uint8_t *get_background_pointer() {
  return background;
}

void draw_background() {
  memcpy(buffer_mem, background, h_res * v_res * bytes_in_pixel);
}

uint32_t(select_bits)(uint32_t value, uint8_t pos, uint8_t size) {
  uint32_t mask = 0;

  for (uint8_t i = 0; i < size; i++) {
    mask = mask | BIT(i);
  }

  mask = mask << pos;
  value = value & mask;
  value = value >> pos;

  return value;
}

uint32_t(get_pixel)(uint16_t x, uint16_t y) {
  uint32_t pixel = 0;

  pixel = pixel | video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel)];
  pixel = pixel | (video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel) + 1] << 8);
  pixel = pixel | (video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel) + 2] << 16);

  return pixel;
}

void(parse_color)(uint8_t *red, uint8_t *green, uint8_t *blue, uint32_t color) {
  *red = select_bits(color, red_pos, red_size);
  *green = select_bits(color, green_pos, green_size);
  *blue = select_bits(color, blue_pos, blue_size);
}

int(vbe_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (x > h_res - 1 || y > v_res - 1) {
    return 1;
  }

  if (color == CHROMA_KEY_GREEN_888) {
    return 0;
  }

  if (bits_in_pixel == 8) {
    buffer_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel)] = (uint8_t) color;
  }
  else {
    uint8_t red, green, blue;
    parse_color(&red, &green, &blue, color);
    uint32_t new_mem = red << red_pos | green << green_pos | blue << blue_pos;

    buffer_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel)] = (uint8_t) select_bits(new_mem, 0, 8);
    buffer_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel) + 1] = (uint8_t) select_bits(new_mem, 8, 8);
    buffer_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel) + 2] = (uint8_t) select_bits(new_mem, 16, 8);
  }
  return 0;
}

int(vg_clear_display)() {
  for (unsigned int i = 0; i < h_res; i++)
    for (unsigned int j = 0; j < v_res; j++)
      vbe_draw_pixel(i, j, 0);

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t j = 0; j < len; j++) {
    vbe_draw_pixel(x + j, y, color);
  }
  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  for (uint16_t i = 0; i < height; i++) {
    vg_draw_hline(x, y + i, width, color);
  }

  return 0;
}

void pageFlip() {
  memcpy(video_mem, buffer_mem, h_res * v_res * bytes_in_pixel);
}
