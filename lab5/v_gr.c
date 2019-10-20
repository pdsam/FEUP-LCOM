#include <lcom/lcf.h>

#include <machine/int86.h>
#include <math.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vMacros.h"
#include "v_gr.h"

static uint8_t *video_mem;    /* Process (virtual) address to which VRAM is mapped */
static uint16_t h_res;        /* Horizontal screen resolution in pixels */
static uint16_t v_res;        /* Vertical screen resolution in pixels */
static uint8_t bits_in_pixel; /* Number of VRAM bits per pixel */
static uint8_t bytes_in_pixel;
static uint8_t green_size, red_size, blue_size;
static uint8_t green_pos, red_pos, blue_pos;

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

  return (void *) video_mem;
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

void(parse_color)(uint8_t *red, uint8_t *green, uint8_t *blue, uint32_t color) {
  *red = select_bits(color, red_pos, red_size);
  *green = select_bits(color, green_pos, green_size);
  *blue = select_bits(color, blue_pos, blue_size);
}

int(vbe_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  static bool flag = false;
  if (bits_in_pixel == 8) {
    video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel)] = (uint8_t) color;
  }
  else {
    uint8_t red, green, blue;
    parse_color(&red, &green, &blue, color);
    uint32_t new_mem = red << red_pos | green << green_pos | blue << blue_pos;

    /*if (!flag) {
      printf("NEW_MEM = %X\n", new_mem);
      printf("RED = %X\n", red);
      printf("GREEN = %X\n", green);
      printf("BLUE = %X\n", blue);
      printf("RED SIZE= %d\n", red_size);
      printf("BLUE SIZE= %d\n", blue_size);
      printf("GREEN SIZE= %d\n", green_size);
      printf("RED POS= %d\n", red_pos);
      printf("BLUE POS= %d\n", blue_pos);
      printf("GREEN POS= %d\n", green_pos);
    }*/

    video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel)] = (uint8_t) select_bits(new_mem, 0, 8);
    video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel) + 1] = (uint8_t) select_bits(new_mem, 8, 8);
    video_mem[(y * h_res * bytes_in_pixel) + (x * bytes_in_pixel) + 2] = (uint8_t) select_bits(new_mem, 16, 8);
  }
  flag = true;
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

uint8_t(col_index)(uint16_t row, uint16_t col, uint8_t no_rectangles, uint8_t step, uint32_t first) {
  return (first + (row * no_rectangles + col) * step) % (1 << bits_in_pixel);
}

uint32_t(col_rgb)(uint16_t row, uint16_t col, uint8_t step, uint32_t first) {
  uint8_t red, green, blue;
  uint32_t color = 0;
  parse_color(&red, &green, &blue, first);
  red = (red + col * step) % (1 << red_size);
  green = (green + row * step) % (1 << green_pos);
  blue = (blue + (col + row) * step) % (1 << blue_size);
  color = (red << red_pos) | (green << green_pos) | (blue << blue_pos);
  return color;
}

uint16_t get_h_res() { return h_res; }
uint16_t get_v_res() { return v_res; }
uint16_t get_bits_in_pixel() { return bits_in_pixel; }
uint8_t *get_video_mem() { return video_mem; }

void vg_draw_xpm(const char *xpm[], uint16_t xi, uint16_t yi) {
  int width, height;
  char *map;
  uint32_t color;
  map = read_xpm(xpm, &width, &height);

  int i = 0, j = 0, counter = 0;
  while (i < height) {

    j = 0;
    while (j < width) {
      color = map[counter];
      counter++;
      vbe_draw_pixel(xi + j, yi + i, color);
      j++;
    }
    i++;
  }

  free(map);
}

void vg_draw_xpm_direct(xpm_string_t xpm[], uint16_t xi, uint16_t yi) {
  xpm_image_t img;
  uint8_t *map;
  uint32_t color;
  map = xpm_load(xpm, XPM_8_8_8, &img);

  int i = 0, j = 0, counter = 0;
  while (i < img.height) {

    j = 0;
    while (j < img.width) {
      color = map[counter];
      counter++;
      vbe_draw_pixel(xi + j, yi + i, color);
      j++;
    }
    i++;
  }

  free(map);
}

int vbe_get_control_parse_info() {

  mmap_t map;
  struct reg86u r;
  vg_vbe_contr_info_t control_info;
  vbe_auxiliar_struct aux;
  char string[4] = "VBE2";

  uint32_t *mem_base = lm_init(true);

  lm_alloc(sizeof(vbe_auxiliar_struct), &map);

  memcpy(map.virt, string, sizeof(string));

  memset(&r, 0, sizeof(r)); //zeroing the struct

  r.u.w.ax = VBE_RETURN_CONTROLER_INFO; //VBE call, function 00 : get control info
  r.u.w.es = PB2BASE(map.phys);         /*set a segment base*/
  r.u.w.di = PB2OFF(map.phys);          /* set the offset accordingly */
  r.u.b.intno = BIOS_VIDEO_SERVICE;     //video card interrupt vector

  if (sys_int86(&r) != OK) {
    printf("vbe_get_control_info: sys_int86 failed\n");
    lm_free(&map);
    return -1;
  }
  /*
    //checking AL register acknowledgment
    if(r.u.b.al != FUNCTION_SUCESS) {
        printf("vbe_get_ctrl_info :: VBE function call not supported\n");
        return -1;
    }

    //checking AH register acknowledgment
    if(r.u.b.ah != FUNCTION_SUCESS_AH) {
        if(r.u.b.ah == FUNCTION_CALL_FAILED) {
            printf("vbe_get_ctrl_info :: VBE function call failed\n");
            return -1;
        }

        if(r.u.b.ah == FUNCTION_NOT_SUPPORTED) {
            printf("vbe_get_ctrl_info :: VBE function call not supported in current HW configuration\n");
            return -1;
        }

        if(r.u.b.ah == FUNCTION_INVALID) {
            printf("vbe_get_ctrl_info :: VBE function call invalid in current video mode\n");
            return -1;
        }
    }
    */

  //Passing the information from the map to the struct
  memcpy(&aux, map.virt, map.size);

  //Freeing the map memory
  lm_free(&map);

  //Copying the required components
  strcpy(control_info.VBESignature, aux.VbeSignature);
  control_info.VBEVersion[0] = aux.VbeVersion[0];
  control_info.VBEVersion[1] = aux.VbeVersion[1];
  control_info.OEMString = (void *) ((uint32_t) mem_base + ((SEGMENT(aux.OemStringPtr)) << 4) + (OFFSET(aux.OemStringPtr)));

  //Turning 64kb blocks into kb
  control_info.TotalMemory = aux.TotalMemory * 64;
  control_info.VideoModeList = (void *) ((uint32_t) mem_base + ((SEGMENT(aux.VideoModePtr)) << 4) + (OFFSET(aux.VideoModePtr)));
  control_info.OEMVendorNamePtr = (void *) ((uint32_t) mem_base + ((SEGMENT(aux.OemVendorNamePtr)) << 4) + (OFFSET(aux.OemVendorNamePtr)));
  control_info.OEMProductNamePtr = (void *) ((uint32_t) mem_base + ((SEGMENT(aux.OemProductNamePtr)) << 4) + (OFFSET(aux.OemProductNamePtr)));
  control_info.OEMProductRevPtr = (void *) ((uint32_t) mem_base + ((SEGMENT(aux.OemProductRevPtr)) << 4) + (OFFSET(aux.OemProductRevPtr)));

  //Priting the struct
  vg_display_vbe_contr_info(&control_info);
  return 0;
}
