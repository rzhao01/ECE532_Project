/*
 * graphics.h
 *
 *  Created on: Feb 25, 2014
 *      Author: DavidDev997516512
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_


/***************************** Include Files *********************************/

#include "xbasic_types.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "tft.h"


/************************** Constant Definitions *****************************/

#define BLACK 0x0000
#define RED 0xF800
#define GREEN 0x07D0
#define BLUE 0x001F
#define YELLOW 0xFF80
#define WHITE 0xFFFF
#define ORANGE 0xFCC0
#define DARK_PURPLE 0xA81F
#define PINK 0xFADF
#define LIGHT_BLUE 0x077F
#define MAGENTA 0xF81F
#define CYAN 0x07FF
#define BROWN 0x82E5

//sin array will have 90/SIN_DIV elements to capture first quarater of sin
#define SIN_DIV 15



/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/



//sin and cos lookup for rendering circle
double sin_lookup(double i);
double cos_lookup(double i);
void render_line(double x1, double y1, double x2, double y2, u16 colour, TFT *TftPtr);
void draw_line_x(double x1, double y1, double x2, double y2, double m, u16 colour, TFT *TftPtr);
void draw_line_y(double x1, double y1, double x2, double y2, double m, u16 colour, TFT *TftPtr);

//Render a hollow circle
void Graphics_RenderCircle(u16 X, u16 Y, u16 Radius, u16 colour, TFT *TftPtr);

void Graphics_FillRect(u16 X, u16 Y, u16 DimX, u16 DimY, TFT *TftPtr);

/************************** Variable Definitions *****************************/


/*****************************************************************************/

#endif /* GRAPHICS_H_ */
