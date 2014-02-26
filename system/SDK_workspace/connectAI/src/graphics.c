/*
 * graphics.c
 *
 *  Created on: Feb 25, 2014
 *      Author: DavidDev997516512
 */


/***************************** Include Files *********************************/

#include "graphics.h"
#include "xil_assert.h"
/************************** Function Definitions******************************/

double sin_a[360/SIN_DIV] = {0, 0.258819045, 0.5, 0.707106781, 0.866025404, 0.965925826, 1};

/*****************************************************************************/
/**
*
* Look up sin value from a table
*
* @param 	i, angle in degrees
*
******************************************************************************/
double sin_lookup(double i){
	int sign = 1;
	int index = 0;
	if(i>180){
		sign = -1;
		i = i - 180;
	}
	if (i>90)
		i = 180 - i;
	index = i/SIN_DIV;
	double result = sign*sin_a[index];
	return result;
}

/*****************************************************************************/
/**
*
* Look up cos value from a table
*
* @param 	i, angle in degrees
*
******************************************************************************/
double cos_lookup(double i){
	if(i>270)
		i = i - 270;
	else
		i = i + 90;
	return sin_lookup(i);
}

/*****************************************************************************/
/**
*
* Render a straight line
*
* @param 	x1,y1, starting point coordinates
* @param 	x2,y2, end point coordinates
* @param 	colour, colour of the line
* @param	TftPtr, pointer to the tft instance on which button is rendered
*
******************************************************************************/
void render_line(double x1, double y1, double x2, double y2, u16 colour, TFT *TftPtr){
	double m = (y2-y1)/(x2-x1);
	if (x1>x2){
		draw_line(x2,y2,x1,y1,m,colour, TftPtr);
		return;
	}
	draw_line(x1,y1,x2,y2,m,colour, TftPtr);
}

void draw_line(double x1, double y1, double x2, double y2, double m, u16 colour, TFT *TftPtr){
	double i = x1;
	double j = y1;
	while(i<=x2 ){
		TFT_WriteToPixel((int)i, (int)j, colour, TFT_GetImageAddress(TftPtr));
		i++;
		j = j+m;
	}
}

/*****************************************************************************/
/**
*
* Render a wireframe circle on TFT screen
*
* @param 	CentreX, X position of the circle centre
* @param    CentreY, Y position of the circle centre
* @param    Radius, the radius of the rendered circle
* @param 	colour, colour of the button
* @param	TftPtr, pointer to the tft instance on which button is rendered
*
*
******************************************************************************/

void Graphics_RenderCircle(u16 CentreX, u16 CentreY, u16 Radius, u16 colour, TFT *TftPtr){
	double x, y, old_x, old_y;
	int i = 0;
	x = (Radius*(cos_lookup(i))) + CentreX;
	y = (Radius*(sin_lookup(i))) + CentreY;
	for(i = SIN_DIV; i<=360; i = i+SIN_DIV){
		old_x = x;
		old_y = y;
		x = (Radius*(cos_lookup(i))) + CentreX;
		y = (Radius*(sin_lookup(i))) + CentreY;
		render_line(old_x, old_y, x, y, colour, TftPtr);
	}
}


/************************** Variable Definitions *****************************/


/*****************************************************************************/
