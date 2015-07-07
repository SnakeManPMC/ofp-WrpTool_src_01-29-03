/* 
   Name: BMP Managing class .cpp file
   Author: Tomas Garcia-Pozuelo Barrios
   Description: Main methods to work with 8 and 24 bpp bitmaps
   Date: Started 03/12/2002
   Copyright: 
*/

/*
   KNOWN ISSUES
   ============
*/

/*
   VERSION HISTORY
   ===============

*/


#include "bmpmanclass.h"

//Constructor
bmpman :: bmpman( void )
{
	Width = 0;
	Height = 0;
    BPP = 0;
  	pbmi = NULL;
 	Palette = NULL;
	Raster = NULL;
}

bmpman :: bmpman( int h, int w, int bitsperpixel )
{
    int i, k;
    RGBQUAD rgb_aux;
  
	Width = h;
	Height = w;
    BPP = bitsperpixel;

	BytesPerRow = Width * BPP / 8;
	BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

	// If BPP aren't 24, reserve memory for the Palette:
	if (BPP==24) pbmi=(BITMAPINFO*)new char [sizeof(BITMAPINFO)];
	else
	{
		pbmi=(BITMAPINFO*) new char[sizeof(BITMAPINFOHEADER)+(1<<BPP)*sizeof(RGBQUAD)];
		Palette=(RGBQUAD*)((char*)pbmi+sizeof(BITMAPINFOHEADER));
	}

    //BITMAPINFO ASIGNATION OF DATA
	pbmi->bmiHeader.biSize=sizeof(BITMAPINFO);
	pbmi->bmiHeader.biHeight=Height;
	pbmi->bmiHeader.biWidth=Width;
	pbmi->bmiHeader.biPlanes=1;
	pbmi->bmiHeader.biBitCount=BPP;
 	pbmi->bmiHeader.biCompression=0;//Multi Char Constant Warning

    int sizeofimage;
    if ( BPP == 24 )
        sizeofimage = ( sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (sizeof(RGBQUAD)*Width*Height));
    else
        sizeofimage = ( sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (sizeof(char)*Width*Height) + sizeof(RGBQUAD)*256);

	pbmi->bmiHeader.biSizeImage = sizeofimage;
	pbmi->bmiHeader.biXPelsPerMeter = 3718;
	pbmi->bmiHeader.biYPelsPerMeter = 3718;
    if ( BPP == 8 )
 	     pbmi->bmiHeader.biClrUsed = 256;
    else{
 	     pbmi->bmiHeader.biClrUsed = 0;
    }
	pbmi->bmiHeader.biClrImportant = 0;

	Raster= new char[BytesPerRow*Height];

//           INICIATION AT WHITE
//********************************************
    char c_aux = char(255);

	for( i = 0; i < Height; i++ )
        for( k = 0; k < BytesPerRow; k++ )
            Raster[i*BytesPerRow+k] = c_aux;
//********************************************

}

//Destructor
bmpman :: ~bmpman()
{
     int i;

     delete [] Raster;
     Raster = NULL;

     if( BPP == 8 ){
         delete [] Palette;
         Palette = NULL;
     }

     delete pbmi;
     pbmi = NULL;
}



// **********
// CRaster::LoadBMPFile (FileName);
//   - loads a BMP file into a CRaster object
//   * supports non-RLE-compressed files of 1, 2, 4, 8 & 24 bits-per-pixel
void bmpman ::LoadBMP ( const char * szFile)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

    //Deletes the image if there is something in it
    if(( this->Height > 0 ) || ( this->Width > 0 ))
         this->Delete();

	// Open file.
	ifstream bmpfile (szFile , ios::in | ios::binary);
	if (! bmpfile.is_open())cout<<"ERROR OPENING BMP FILE\n";		// Error opening file

	// Load bitmap fileheader & infoheader
	bmpfile.read ((char*)&bmfh,sizeof (BITMAPFILEHEADER));
	bmpfile.read ((char*)&bmih,sizeof (BITMAPINFOHEADER));

	// Check filetype signature
	if (bmfh.bfType!='MB')cout<<"FILE IS NOT A BMP FILE\n";		// File is not BMP

	// Assign some short variables:
	BPP=bmih.biBitCount;
	Width=bmih.biWidth;
	Height= (bmih.biHeight>0) ? bmih.biHeight : -bmih.biHeight; // absoulte value
	BytesPerRow = Width * BPP / 8;
	BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

	// If BPP aren't 24, load Palette:
	if (BPP==24) pbmi=(BITMAPINFO*)new char [sizeof(BITMAPINFO)];
	else
	{
		pbmi=(BITMAPINFO*) new char[sizeof(BITMAPINFOHEADER)+(1<<BPP)*sizeof(RGBQUAD)];
		Palette=(RGBQUAD*)((char*)pbmi+sizeof(BITMAPINFOHEADER));
		bmpfile.read ((char*)Palette,sizeof (RGBQUAD) * (1<<BPP));
	}
	pbmi->bmiHeader=bmih;

	// Load Raster
	bmpfile.seekg (bmfh.bfOffBits,ios::beg);

	Raster= new char[BytesPerRow*Height];

	// (if height is positive the bmp is bottom-up, read it reversed)
	if (bmih.biHeight>0)
		for (int n=Height-1;n>=0;n--)
			bmpfile.read (Raster+BytesPerRow*n,BytesPerRow);
	else
		bmpfile.read (Raster,BytesPerRow*Height);
	bmpfile.close();

}




// CRaster::SaveBMPFile (FileName, bmp2save, corrector);
//   - saves a CRaster object into a BMP file
void bmpman :: SaveBMP ( const char * szFile )
{
    BITMAPFILEHEADER bmfh;
    int BytesPerRow;

    bmfh.bfType=WORD('MB');
    bmfh.bfSize = ( sizeof(bmfh) + sizeof(BITMAPINFO) + 1024 + (Width*Height));
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    if( BPP == 8 )
        bmfh.bfOffBits =  1078; //Because the Palette
    else
        bmfh.bfOffBits =  54;


	BITMAPINFOHEADER bmih = pbmi->bmiHeader;

 	BytesPerRow = Width * BPP / 8;
	BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

    // Open file.
	ofstream bmpfile (szFile , ios::out | ios::binary);

	if (! bmpfile.is_open()){cout << "ERROR SAVING FILE" << endl;}

	//Save bitmap fileheader & infoheader
	bmpfile.write ((char*)&bmfh,sizeof (BITMAPFILEHEADER));
	bmpfile.write ((char*)&bmih,sizeof (BITMAPINFOHEADER));

    if ( BPP == 8 ){
        int i;
        for( i = 0; i < (1<<BPP); i++ ){
            bmpfile.write ((char*)&Palette[i], sizeof(char)*4);
        }
    }

    //Save the data
    int n;
  	// (if height is positive the bmp is bottom-up, read it reversed)
	if (bmih.biHeight>0)
		for (int n=Height-1;n>=0;n--)
			bmpfile.write (Raster+BytesPerRow*n,BytesPerRow);
	else
		bmpfile.write (Raster,BytesPerRow*Height);

	bmpfile.close();

}

//Frees the space of one image without deleting it
void bmpman :: Delete()
{

//HAY UN PROBLEMA CON ESTA FUNCION
     int i, j;

  	 delete [] pbmi;
     pbmi = NULL;

     if ( BPP == 8 ){
       delete [] Palette;
       Palette = NULL;
     }

     delete [] Raster;
     Raster = NULL;

   	 Width = 0;
	 Height = 0;
     BPP = 0;
}

//Functions for 8bpp
RGBQUAD bmpman :: GetPalRGB( int index )
{
    RGBQUAD rgb_aux;

    if( BPP == 8 ){
        rgb_aux.rgbRed = Palette[index].rgbRed;
        rgb_aux.rgbGreen = Palette[index].rgbGreen;
        rgb_aux.rgbBlue = Palette[index].rgbBlue;
        rgb_aux.rgbReserved = 0;
        return rgb_aux;
    }
    else
        cout << "ERROR, IMAGE IS NOT 8BPP" << endl;
}

//It puts a RGBQUAD in the palette
void bmpman :: PutPalRGB( int index, RGBQUAD rgb_aux )
{

    if( BPP == 8 ){
        Palette[index].rgbRed = rgb_aux.rgbRed;
        Palette[index].rgbGreen = rgb_aux.rgbGreen;
        Palette[index].rgbBlue = rgb_aux.rgbBlue;
    }
    else
        cout << "ERROR, IMAGE IS NOT 8BPP" << endl;
}

//It gets data for an 8bpp BMP in a pixel
int bmpman :: GetINDdata( int x, int y )
{
    int i_aux, BytesPerRow;

    if( BPP == 8 ){

     	BytesPerRow = Width * BPP / 8;
	    BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

        i_aux = int(char(Raster[y*BytesPerRow + x]));

        if( i_aux < 0 )
            i_aux = 256 + i_aux;

        return i_aux;
    }
    else
        cout << "ERROR, IMAGE IS NOT 8BPP" << endl;
}

void bmpman :: EnterINDdata( int x, int y, int index )
{
    int BytesPerRow;

  	BytesPerRow = Width * BPP / 8;
    BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

    if( BPP == 8 )
        if( (index > 255) || (index < 0))
            cout << "ERROR, INDEX IS BIGGER OR SMALLER THAN ONE BYTE" << endl;
        else
            Raster[y*BytesPerRow + x] = char(index);
    else
        cout << "ERROR, IMAGE IS NOT 8BPP" << endl;
}

//Functions for 24bpp
RGBQUAD bmpman :: GetRGBdata( int x, int y )
{
    RGBQUAD rgb_aux;
    int BytesPerRow;

  	BytesPerRow = Width * BPP / 8;
    BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

    if( BPP == 24 ){

        rgb_aux.rgbRed = Raster[y*BytesPerRow + x*3 + 2];//2 = rgbRed;
        rgb_aux.rgbGreen = Raster[y*BytesPerRow + x*3 + 1];//1 = rgbGreen;
        rgb_aux.rgbBlue = Raster[y*BytesPerRow + x*3];//0 = rgbBlue;

        return rgb_aux;
    }
    else
        cout << "ERROR, IMAGE IS NOT 24BPP" << endl;
}

void bmpman :: EnterRGBdata( int x, int y, RGBQUAD rgb )
{
    int BytesPerRow;

  	BytesPerRow = Width * BPP / 8;
    BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment

    if( BPP == 24 ){
        Raster[x*BytesPerRow + y*3 + 2] = rgb.rgbRed;
        Raster[x*BytesPerRow + y*3 + 1] = rgb.rgbGreen;
        Raster[x*BytesPerRow + y*3] = rgb.rgbBlue;
    }          
    else
        cout << "ERROR, IMAGE IS NOT 24BPP" << endl;
}

int bmpman :: GetWidth()
{
    return Width;
}

int bmpman :: GetHeigth()
{
    return Height;
}

RGBQUAD bmpman :: GetClr( int x, int y )
{
    RGBQUAD rgb_aux;

    if( BPP == 8 ){
        rgb_aux = this->GetPalRGB( this->GetINDdata( x, y ) );
    }
    else{
        rgb_aux = this->GetRGBdata( x, y );
    }
}

void bmpman :: LoadBMPHeader()
{
   ifstream fi;

   string name = "bmpbase.dat";


   fi.open( name.c_str() );
   if( !fi )
       cout << "ERROR OPENING THE bmpbase.dat FILE" << endl;
   else
   {
  	BITMAPFILEHEADER bmfh_aux;
    BITMAPINFOHEADER bmih_aux;

 	fi.read ((char*)&bmfh_aux,sizeof (BITMAPFILEHEADER));
	fi.read ((char*)&bmih_aux,sizeof (BITMAPINFOHEADER));

    this->pbmi->bmiHeader = bmih_aux;

	pbmi->bmiHeader.biHeight=Height;
	pbmi->bmiHeader.biWidth=Width;
	pbmi->bmiHeader.biBitCount=BPP;
	pbmi->bmiHeader.biClrUsed=0;
	pbmi->bmiHeader.biClrImportant=0;
   }
   fi.close();
}


