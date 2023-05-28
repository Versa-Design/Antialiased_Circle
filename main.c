//********************************************************************************
// File: main.c
// Description: Versa Design Antialiased Circle Algorithm
// Author: Juan Ramon Vadillo Pastor (Versa Design S.L.)
// Date: 2019-12-01
// Version: 1.0
// License: MIT
//********************************************************************************

#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <realtimeapiset.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

//--------------------------------------------------------------------------------
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 40

// Global variables
uint8_t frameBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];

//********************************************************************************
void ClearFrameBuffer(void)
//********************************************************************************
// Description: Clears the frame buffer
// Parameters: none
// Returns: nothing
//********************************************************************************
{
  uint32_t x, y;
  for(y=0; y<SCREEN_HEIGHT; y++)
  {
    for(x=0; x<SCREEN_WIDTH; x++)
    {
      frameBuffer[y*SCREEN_WIDTH+x] = 0;
    }
  }
}

//********************************************************************************
void PutPixel(uint32_t x, uint32_t y, uint8_t color)
//********************************************************************************
// Description: Sets a pixel in the frame buffer
// Parameters: x,y - pixel coordinates
//             color - color to set the pixel
// Returns: nothing
//********************************************************************************
{
  if(x<SCREEN_WIDTH && y<SCREEN_HEIGHT)
  {
    frameBuffer[y*SCREEN_WIDTH+x] = color;
  }
}



//********************************************************************************
void PrintFrameBuffer(void)
//********************************************************************************
// Description: Prints the frame buffer to the screen
// Parameters: none
// Returns: nothing
//********************************************************************************
{
  uint32_t x, y;
  printf("*");
  for (x=0; x<SCREEN_WIDTH; x++) printf("-");
  printf("*\n");

  for(y=0; y<SCREEN_HEIGHT; y++)
  {
    printf("|");
    for(x=0; x<SCREEN_WIDTH; x++)
    {
      if (frameBuffer[y*SCREEN_WIDTH+x] == 0)
        printf(" ");
      else
        printf("%1X", frameBuffer[y*SCREEN_WIDTH+x]>>4);
    }
    printf("|\n");
  }
  printf("*");
  for (x=0; x<SCREEN_WIDTH; x++) printf("-");
  printf("*\n");
}

//********************************************************************************
void GrDrawFilledCircleAntialiased(uint16_t x0, uint16_t y0, uint16_t r)
//********************************************************************************
// Description: Draws a filled circle using the Versa Design Antialiased Circle
// Algorithm
// Parameters: x0,y0 - center of the circle
//             r - radius of the circle
// Returns: nothing
//********************************************************************************
{
  uint32_t x, y, rmin, rmax, sqy, sqd, c;
  rmin = r * r - r;
  rmax = r * r + r;
  for (y = y0 - r; y <= y0 + r; y++)
  {
    sqy = (y - y0) * (y - y0);
    for (x = x0 - r; x <= x0 + r; x++)
    {
      sqd = (x - x0) * (x - x0) + sqy;
      if (sqd <= rmin)
      {
        PutPixel(x, y, 255);
      }
      else if (sqd < rmax)
      {
        c = rmax - sqd;         // Calculate linear interpolation
        c *= 256;
        c += r;                 // Round up
        c /= 2 * r;
        if (c > 255) c = 255;
        PutPixel(x, y, c);
      }
    }
  }
}


//******************************************************************************
void GrDrawFilledCircleAntialiasedOptimized(uint16_t x0,uint16_t y0,uint16_t r)
//******************************************************************************
// Description: Draw a filled circle with antialiasing
// Parameter:   x0,y0: Center coordinates
//              r:    Radius
// Returns: nothing
//******************************************************************************
{
  uint32_t x,y,rmin,rmax,sqy,sqd,c,rd;
  rd   = 2*r;
  rmin = r * r - r;
  rmax = r * r + r;

  for (y = 0; y<=r; y++)
  {
    sqy = y*y;
    for (x = 0; x<=r; x++)
    {
      sqd = (x*x)+sqy;
      if (sqd <= rmin)
      {
        PutPixel(x0+x,y0-y,255);  // Draw 4 pixels at once
        PutPixel(x0-x,y0-y,255);  // for the 4 quadrants
        PutPixel(x0+x,y0+y,255);
        PutPixel(x0-x,y0+y,255);
      }
      else if (sqd < rmax)
      {
        c  = rmax - sqd;        // Calculate linear interpolation
        c *= 255;
        c += r;    // Round up
        c /= rd;
        if (c > 255) c = 255;
        PutPixel(x0+x,y0-y,c);  // Draw 4 pixels at once
        PutPixel(x0-x,y0-y,c);  // for the 4 quadrants
        PutPixel(x0+x,y0+y,c);
        PutPixel(x0-x,y0+y,c);
      }
      else break;
    }
  }
}


//********************************************************************************
uint64_t GetCycleCount(void)
//********************************************************************************
// Description: Returns the current process cycle count
// Parameters: none
// Returns: current process cycle count
//********************************************************************************
{
  uint64_t cycles;
  if (QueryProcessCycleTime(GetCurrentProcess(), &cycles) == 0)
  {
    printf("Error: QueryProcessCycleTime\n");
    return 0;
  }
  return cycles;
}

#define ITERATIONS_FOR_PROFILING 1000000


//********************************************************************************
int main(int argc, char *argv[])
//********************************************************************************
// Description: Main entry point
// Parameters: argc - number of command line arguments
//             argv - array of command line arguments
// Returns: 0
//********************************************************************************
{
  uint32_t x,y,r;
  uint64_t startTime;
  uint64_t endTime;
  uint32_t i;
  printf("Versa Design Antialiased Circle Algorithm\n");
  printf("Usage: %s x y r\n", argv[0]);
  ClearFrameBuffer();

  x = SCREEN_WIDTH/2;
  y = SCREEN_HEIGHT/2;
  r = 16;
  if (argc > 1) x = atoi(argv[1]);
  if (argc > 2) y = atoi(argv[2]);
  if (argc > 3) r = atoi(argv[3]);
  printf("Drawing circle at (%d,%d) with radius %d\n", x, y, r);
  GrDrawFilledCircleAntialiasedOptimized(x, y, r);
  PrintFrameBuffer();

  printf("\n\n");
  printf("Profiling...\n");
  printf("Trying standard algorithm %d times\n",ITERATIONS_FOR_PROFILING);
  startTime = GetCycleCount();
  for (i=0; i<ITERATIONS_FOR_PROFILING; i++)
    GrDrawFilledCircleAntialiased(x, y, r);
  endTime = GetCycleCount();
  printf("Time for %d iterations: %u cycles\n", ITERATIONS_FOR_PROFILING, (uint32_t)(endTime-startTime));

  printf("\n\n");
  printf("Trying optimized algorithm %d times\n",ITERATIONS_FOR_PROFILING);
  startTime = GetCycleCount();
  for (i=0; i<ITERATIONS_FOR_PROFILING; i++)
    GrDrawFilledCircleAntialiasedOptimized(x, y, r);
  endTime = GetCycleCount();
  printf("Time for %d iterations: %u cycles\n", ITERATIONS_FOR_PROFILING, (uint32_t)(endTime-startTime));

  return 0;
}
