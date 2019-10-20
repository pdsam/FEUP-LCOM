#pragma once


int vbe_get_mode_info_v2(uint16_t mode, vbe_mode_info_t *vmi_p);
uint32_t select_bits(uint32_t value, uint8_t pos, uint8_t size);
void parse_color(uint8_t* red, uint8_t* green, uint8_t* blue, uint32_t color);
int vbe_draw_pixel(uint16_t x, uint16_t y, uint32_t colour);
uint8_t col_index(uint16_t row, uint16_t col, uint8_t no_rectangles, uint8_t step, uint32_t first);
uint32_t col_rgb(uint16_t row, uint16_t col, uint8_t step, uint32_t first);
uint16_t get_h_res();
uint16_t get_v_res();
uint16_t get_bits_in_pixel();
uint8_t *get_video_mem();
void vg_draw_xpm(const char *xpm[], uint16_t xi, uint16_t yi);
int vbe_get_control_parse_info();

void vg_draw_xpm_direct(xpm_string_t xpm[], uint16_t xi, uint16_t yi);


typedef struct __attribute__((packed)){
    char VbeSignature [4];
    BCD VbeVersion[2];
    uint32_t OemStringPtr;
    uint32_t Capabilities;
    uint32_t VideoModePtr;
    uint16_t TotalMemory;
    BCD OemSoftwareRev [2];
    uint32_t OemVendorNamePtr;
    uint32_t OemProductNamePtr;
    uint32_t OemProductRevPtr;
    uint8_t reserved [222];
    uint8_t OemData [256];
} vbe_auxiliar_struct;
