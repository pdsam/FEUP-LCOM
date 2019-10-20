#pragma once
#include "sprite.h"

/** @defgroup aSprite aSprite
 * @{
 *
 * Animated Sprite 
 */


/**
 * @brief Animated Sprite Strucet
 * 
 */
typedef struct {
	int aspeed;		///< animation speed 
	int cur_aspeed; ///< current animation speed 
	int num_fig; 	///< number of pixmaps 
	int cur_fig; 	///< current pixmap 
  int max_num_fig;///< max number of pixmaps
	Sprite* maps[10];     ///< pointer to array of each AnimSprite pixmaps
} AnimSprite;


/**
 * @brief Create a asprite object with only the base pixmap
 * 
 * @param base - Base sprite
 * @param num_xpms - Number of xpms used in the animation
 * @param speed - Number of frames between changes
 * @return AnimSprite* 
 */
AnimSprite * create_asprite(Sprite *base, uint16_t num_xpms, int speed);
/**
 * @brief Adds an adicional pix map to the AnimSprite object
 * 
 * @param name - xpm_string_t to load to the AnimSprite
 * @param asp - AnimeSprite to load pix maps to
 * @return int - Returns 0 on sucess and 1 if not.
 */
int load_map_to_AnimSprite(xpm_string_t *name, AnimSprite * asp);
/**
 * @brief Draw the next map of the  asprite object when the speed frame has been reached
 * 
 * @param asp - Animated sprite to draw
 */
void draw_AnimSprite(AnimSprite * asp);
/**
 * @brief Draw the current map of the  asprite object 
 * 
 * @param asp - Animated sprite to draw
 */
void draw_current_Sprite(AnimSprite * asp);
/**
 * @brief Free the memorie used for the AnimSprtie
 * 
 * @param asp - AnimSprite to delete
 */
void delete_AnimSprite(AnimSprite * asp);

/** @} end of aSprite */
