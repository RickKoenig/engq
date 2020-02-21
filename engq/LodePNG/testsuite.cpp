/*
LodePNG PngSuite-test

Copyright (c) 2005-2007 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

/*
This code can be used to test LodePNG, picoPNG, or others, on the PngSuite, a test
suite made by Willem van Schaik and can be found at http://www.schaik.com/pngsuite/

You need to download the PngSuite images at http://www.schaik.com/pngsuite/ to
be able to use this test.

The structure of this .cpp file is as follows:
-header inclusions:
  *SDL is required
  *lodepng.h is required if you want to test LodePNG, in that case you also need lodepng.cpp
-the load function, in this function the png is loaded using the decoder you want to test
  *In this case, LodePNG and picoPNG can be tested by commenting/uncommenting the correct code
  *you could as well use it to test a different PNG decoder by putting the code to use that decoder in it
-the show function, which calls the load function to load the image and then draws it with SDL
  *it draws a checkered background pattern behind translucent PNGs
  *if you press esc, it returns 0, if you press the close button of the window it returns 1
  *it turns the whole window red instead of showing the image if the decoder returned an error
-the main function
  *the folder variable can be used to specify the folder in which you placed the PngSuite images
  *this calls the show function for most of the PngSuite images
  *if the show function returns that you pressed the close button, it quits, otherwise it opens the next image
  
So, press esc to see the next image, press the close button to stop viewing next images.
*/

//g++ *.cpp -lSDL -Wall -Wextra -pedantic -ansi -O3


#include <iostream>
#include <SDL/SDL.h>
#include "lodepng.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int load(std::vector<unsigned char>& out, unsigned& w, unsigned& h, std::string filename)
{
  std::vector<unsigned char> buffer;
  LodePNG::loadFile(buffer, filename); //load the image file with given filename
  
  
  //using LodePNG:
  LodePNG::Decoder decoder;
  decoder.decode(out, buffer); //decode the png
  w = decoder.getWidth();
  h = decoder.getHeight();
  return decoder.getError();
  
  //using picoPNG (you need to copypaste the decodePNG function from picoPNG in this .cpp file to use this)
  //return decodePNG(out, w, h, &buffer[0], buffer.size());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int show(const std::string& filename)
{
  std::vector<unsigned char> image;
  unsigned w, h;

  int error = load(image, w, h, filename);

  //stop if there is an error
  if(error)
  {
    std::cout << "error " <<  error << " in file " << filename << " - w: " << w << " h: " << h << std::endl;
  }
  
  //avoid too large window size by downscaling large image
  unsigned jump = 1;
  if(w / 1024 >= jump) jump = w / 1024 + 1;
  if(h / 1024 >= jump) jump = h / 1024 + 1;
  
  //init SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
  unsigned screensizex = w / jump, screensizey = h / jump;
  if(screensizex < 256) screensizex = 256;
  if(screensizey < 32) screensizey = 32;
  SDL_Surface* scr = SDL_SetVideoMode(screensizex, screensizey, 32, SDL_HWSURFACE);
  if(!scr) return -1;
  SDL_WM_SetCaption(filename.c_str(), NULL); //set window caption
  
  //plot the pixels of the PNG file
  if(!error)
  for(unsigned y = 0; y + jump - 1 < h; y += jump)
  for(unsigned x = 0; x + jump - 1 < w; x += jump)
  {
    //get RGBA components
    Uint32 r = image[4 * y * w + 4 * x + 0]; //red
    Uint32 g = image[4 * y * w + 4 * x + 1]; //green
    Uint32 b = image[4 * y * w + 4 * x + 2]; //blue
    Uint32 a = image[4 * y * w + 4 * x + 3]; //alpha
    
    //make translucency visible by placing checkerboard pattern behind image
    int checkerColor = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
    r = (a * r + (255 - a) * checkerColor) / 255;
    g = (a * g + (255 - a) * checkerColor) / 255;
    b = (a * b + (255 - a) * checkerColor) / 255;
    
    //give the color value to the pixel of the screenbuffer
    Uint32* bufp;
    bufp = (Uint32 *)scr->pixels + (y * scr->pitch / 4) / jump + (x / jump);
    *bufp = 65536 * r + 256 * g + b;
  }
  else //draw red screen indicating error
  for(unsigned y = 0; y < screensizey; y++)
  for(unsigned x = 0; x < screensizex; x++)
  {
    Uint32* bufp;
    bufp = (Uint32 *)scr->pixels + (y * scr->pitch / 4) + x;
    *bufp = 65536 * 255;
  }
  
  //pause until you press escape and meanwhile redraw screen
  SDL_Event event;
  int done = 0;
  while(done == 0)
  {
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT) done = 2;
      if(SDL_GetKeyState(NULL)[SDLK_ESCAPE]) done = 1;
    }
    SDL_UpdateRect(scr, 0, 0, 0, 0); //redraw screen
    SDL_Delay(5); //pause 5 ms so it consumes less processing power
  }
  
  SDL_Quit();
  
  if(done == 2) return 1;
  else return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main()
{
  std::string folder = "PngSuite/";
  
  //basic formats
  
  if(show(folder + "basn0g01.png")) return 0;
  if(show(folder + "basn0g02.png")) return 0;
  if(show(folder + "basn0g04.png")) return 0;
  if(show(folder + "basn0g08.png")) return 0;
  if(show(folder + "basn0g16.png")) return 0;
  
  if(show(folder + "basn2c08.png")) return 0;
  if(show(folder + "basn2c16.png")) return 0;
  
  if(show(folder + "basn3p01.png")) return 0;
  if(show(folder + "basn3p02.png")) return 0;
  if(show(folder + "basn3p04.png")) return 0;
  if(show(folder + "basn3p08.png")) return 0;
  
  if(show(folder + "basn4a08.png")) return 0;
  if(show(folder + "basn4a16.png")) return 0;
  
  if(show(folder + "basn6a08.png")) return 0;
  if(show(folder + "basn6a16.png")) return 0;
  
  //interlacing
  
  if(show(folder + "basi0g01.png")) return 0;
  if(show(folder + "basi0g02.png")) return 0;
  if(show(folder + "basi0g04.png")) return 0;
  if(show(folder + "basi0g08.png")) return 0;
  if(show(folder + "basi0g16.png")) return 0;
  
  if(show(folder + "basi2c08.png")) return 0;
  if(show(folder + "basi2c16.png")) return 0;
  
  if(show(folder + "basi3p01.png")) return 0;
  if(show(folder + "basi3p02.png")) return 0;
  if(show(folder + "basi3p04.png")) return 0;
  if(show(folder + "basi3p08.png")) return 0;
  
  if(show(folder + "basi4a08.png")) return 0;
  if(show(folder + "basi4a16.png")) return 0;
  
  if(show(folder + "basi6a08.png")) return 0;
  if(show(folder + "basi6a16.png")) return 0;
  
  //odd sizes
  
  if(show(folder + "s01i3p01.png")) return 0;
  if(show(folder + "s01n3p01.png")) return 0;
  if(show(folder + "s02i3p01.png")) return 0;
  if(show(folder + "s02n3p01.png")) return 0;
  if(show(folder + "s03i3p01.png")) return 0;
  if(show(folder + "s03n3p01.png")) return 0;
  if(show(folder + "s04i3p01.png")) return 0;
  if(show(folder + "s04n3p01.png")) return 0;
  if(show(folder + "s05i3p02.png")) return 0;
  if(show(folder + "s05n3p02.png")) return 0;
  if(show(folder + "s06i3p02.png")) return 0;
  if(show(folder + "s06n3p02.png")) return 0;
  if(show(folder + "s07i3p02.png")) return 0;
  if(show(folder + "s07n3p02.png")) return 0;
  if(show(folder + "s08i3p02.png")) return 0;
  if(show(folder + "s08n3p02.png")) return 0;
  if(show(folder + "s09i3p02.png")) return 0;
  if(show(folder + "s09n3p02.png")) return 0;
  
  if(show(folder + "s32i3p04.png")) return 0;
  if(show(folder + "s32n3p04.png")) return 0;
  if(show(folder + "s33i3p04.png")) return 0;
  if(show(folder + "s33n3p04.png")) return 0;
  if(show(folder + "s34i3p04.png")) return 0;
  if(show(folder + "s34n3p04.png")) return 0;
  if(show(folder + "s35i3p04.png")) return 0;
  if(show(folder + "s35n3p04.png")) return 0;
  if(show(folder + "s36i3p04.png")) return 0;
  if(show(folder + "s36n3p04.png")) return 0;
  if(show(folder + "s37i3p04.png")) return 0;
  if(show(folder + "s37n3p04.png")) return 0;
  if(show(folder + "s38i3p04.png")) return 0;
  if(show(folder + "s38n3p04.png")) return 0;
  if(show(folder + "s39i3p04.png")) return 0;
  if(show(folder + "s39n3p04.png")) return 0;
  if(show(folder + "s40i3p04.png")) return 0;
  if(show(folder + "s40n3p04.png")) return 0;
  
  //filtering
  
  if(show(folder + "f00n0g08.png")) return 0;
  if(show(folder + "f00n2c08.png")) return 0;
  if(show(folder + "f01n0g08.png")) return 0;
  if(show(folder + "f01n2c08.png")) return 0;
  if(show(folder + "f02n0g08.png")) return 0;
  if(show(folder + "f02n2c08.png")) return 0;
  if(show(folder + "f03n0g08.png")) return 0;
  if(show(folder + "f03n2c08.png")) return 0;
  if(show(folder + "f04n0g08.png")) return 0;
  if(show(folder + "f04n2c08.png")) return 0;
  
  //compression level
  
  if(show(folder + "z00n2c08.png")) return 0;
  if(show(folder + "z03n2c08.png")) return 0;
  if(show(folder + "z06n2c08.png")) return 0;
  if(show(folder + "z09n2c08.png")) return 0;
  
  //chunk ordering
  
  if(show(folder + "oi1n0g16.png")) return 0;
  if(show(folder + "oi1n2c16.png")) return 0;
  if(show(folder + "oi2n0g16.png")) return 0;
  if(show(folder + "oi2n2c16.png")) return 0;
  if(show(folder + "oi4n0g16.png")) return 0;
  if(show(folder + "oi4n2c16.png")) return 0;
  if(show(folder + "oi9n0g16.png")) return 0;
  if(show(folder + "oi9n2c16.png")) return 0;
}
