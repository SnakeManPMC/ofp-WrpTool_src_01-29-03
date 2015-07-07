/* 
   Name: BMP Managing class .h file
   Author: Tomas Garcia-Pozuelo Barrios
   Description: Main methods to work with 8 and 24 bpp bitmaps
   Date: Started 22/01/2003
   Copyright: 
*/

#ifndef BMP_H_CAB
#define BMP_H_CAB

#include <iostream.h>
#include <stdlib.h>

#include <windows.h>
#include <fstream.h>
#include <string>

// **********
// class CRaster
//   - Generic class for BMP raster images.
class bmpman {
    private:
		int Width,Height;		// Dimensions
		int BPP;				// Bits Per Pixel.
		char * Raster;			// Bits of the Image.
		RGBQUAD * Palette;		// RGB Palette for the image.
		int BytesPerRow;		// Row Width (in bytes).
		BITMAPINFO * pbmi;		// BITMAPINFO structure
	public:
		// Member functions (defined later):
        bmpman( void );
        bmpman( int h, int w, int bitsperpixel );
        ~bmpman();
		void LoadBMP ( const char * szFile);
        void SaveBMP ( const char * szFile );
        void Delete();

        RGBQUAD GetPalRGB( int index );
        void PutPalRGB( int index, RGBQUAD rgb_aux );
        int GetINDdata( int x, int y );
        void EnterINDdata( int x, int y, int index );

        RGBQUAD GetRGBdata( int x, int y );
        void EnterRGBdata( int x, int y, RGBQUAD rgb );

        int GetWidth();
        int GetHeigth();
        RGBQUAD GetClr( int x, int y );
        void LoadBMPHeader();

};


#endif
