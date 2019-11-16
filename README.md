# C_Life
An implementation of Life and Immigration Life (Color War)

/*************************************************************
*  This program contains:                                    *
*  1. life (regular rules - using adv toroidal wrapping)     *
*      a.) option to fill the board randomly                 *
*      b.) option to start from a known configuration        *
*          such as the glider gun                            *
*  2. Advanced life features are uploaded as life_extra.c    *
*     and implement Immigration Life and Color Cycle Life    *
**************************************************************
*  NOTE please compile using -w for nanosleep                *
*************************************************************/

/*************************************************************
*  This program contains 2 advanced versions of LIFE         *
*  2. Immigration life (color war)                           *
*      This version of life begins with live cells split     *
*      between two colors randomly. The color of the next    *
*      cell is determined by the majority color of           *
*      its parents. Over time one color may dominate         *
*      -to run this version, input 0 at the start.           *
*      -this version is achieved using a 2D array of structs *
*      -functions for the advanced version appear            *
*       at the end of the file and all begin with "im_"      *
*  3. Advanced Color life (life cycle)                       *
*      This version shows the lifespan of the cell through   *
*      color.                                                *
*                 Birth = cyan (light blue)                  *
*                 Child = green                              *
*                 Adult = yellow                             *
*      -to run this version, input 1 at the start            *
*  NOTE please compile using -w to for nanosleep             *
*************************************************************/
