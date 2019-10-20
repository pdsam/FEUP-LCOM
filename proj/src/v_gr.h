#pragma once
#include <lcom/lcf.h>

/** @defgroup vgr vgr
 * @{
 *
 * video graphics card related functions
 */

/**
 * @brief Initializes packed vbe_mode__info_t structure passed as an address with the VBE information on the input mode, by calling VBE function 0x01, Return VBE Mode Information, and copying the ModeInfoBlock struct returned by that function.
 * 
 * @param mode - mode whose information should be returned
 * @param vmi_p - address of vbe_mode_info_t structure to be initialized
 * @return int - Returns 0 on sucess and 1 otherwise
 */
int vbe_get_mode_info_v2(uint16_t mode, vbe_mode_info_t *vmi_p);
/**
 * @brief Selects a sub part of a bit mask
 * 
 * @param value - Value to be split
 * @param pos - Position of the first bit
 * @param size - Amount of bits to be split
 * @return uint32_t - Returns the sub part
 */
uint32_t select_bits(uint32_t value, uint8_t pos, uint8_t size);

/**
 * @brief Gets the Horizonal Resoltion
 * 
 * @return uint16_t - Horizontal resolution
 */
uint16_t get_h_res();
/**
 * @brief Gets the Vertical Resoltion
 * 
 * @return uint16_t - Vertical Resolution
 */
uint16_t get_v_res();
/**
 * @brief Get the number of bits in a pixel
 * 
 * @return uint16_t - Number of bits in a pixel
 */
uint16_t get_bits_in_pixel();
/**
 * @brief Get the video memory pointer
 * 
 * @return uint8_t* - pointer to the video memory
 */
uint8_t *get_video_mem();
/**
 * @brief Get the pixel in the x and y coordinates
 * 
 * @param x - the pixel in the x positon of the horizontal axis
 * @param y - the pixel in the y position of the vertical axis
 * @return uint32_t - Pixel content
 */
uint32_t get_pixel(uint16_t x, uint16_t y);
/**
 * @brief Splits a colour value into it's RGB components
 * 
 * @param red - value of red
 * @param green - value of green
 * @param blue - value of blue
 * @param color - colour to be parsed
 */
void parse_color(uint8_t* red, uint8_t* green, uint8_t* blue, uint32_t color);
/**
 * @brief Draw a pixel in the video memorie in the position (x,y), of a colour
 * 
 * @param x - X value of the pixel to be drawn 
 * @param y - Y value of the pixel to be drawn
 * @param colour - Colour to draw in the pixel
 * @return int - Returns 0 if sucessful and non-zero otherwise
 */
int vbe_draw_pixel(uint16_t x, uint16_t y, uint32_t colour);
/**
 * @brief fill the video memorie with blank pixels(black pixels)
 * 
 * @return int - Returns 0 if sucessful and non-zero otherwise
 */
int vg_clear_display();
/**
 * @brief Gets the buffer pointer (for double buffering)
 * 
 * @return uint8_t* - The pointer to the buffer
 */
uint8_t * get_buffer_pointer();
/**
 * @brief Copies the Buffer Memory to be displayed
 * 
 */
void pageFlip();
/**
 * @brief Gets the pointer for the auxiliar buffer for the background
 * 
 * @return uint8_t* - Background pointer
 */
uint8_t * get_background_pointer();
/**
 * @brief copies the background into the buffer
 * 
 */
void draw_background();

/** @} end of vgr */
