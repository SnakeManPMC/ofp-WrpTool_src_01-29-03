#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//class for bmp file managing
#include "bmpmanclass.h"

#define MapSize 256

typedef struct {
	short Elevation;
	short TexIndex;
} wrpformat[MapSize][MapSize];

	wrpformat wrp;


//Prototypes (C_GP 26/01/03 15:30 )
void ReadCords( ifstream & f, float & X, float & Y, float & Z );

int main()
{
	FILE *outp;
	FILE *text;
	FILE *vege;
	char sig[33],Rnd3dObjN[255][76];
	char dObjName[76];
	int ver;
	short hoss1,hoss2,hoss3,TexIndex;
	float dDir,dDirX=0,dDirZ=0;
	long dObjIndex;
	int rx=0,rnd3d=0,x=0,z=0;


	outp=fopen("pmcworld.wrp","wb");
	if (!outp) {printf("error in pmcworld.wrp\n");exit(1);}

	text=fopen("wrpdebug.txt","wt");
	if (!text) {printf("error at wrpdebug.txt\n");exit(1);}

	/* vegetation configuration file & strings */
	vege=fopen("vegetation.txt","rt");
	if (!vege) {printf("error at vegetation.txt!\n"); exit(1);}

		fgets(Rnd3dObjN[rx],80,vege);

	while (!feof(vege)) {

		Rnd3dObjN[rx][strlen(Rnd3dObjN[rx])-1]=0;
		rx++;
		fgets(Rnd3dObjN[rx],80,vege);

	}

	fclose(vege);
	rx--;
	fprintf(text, "amount of strings in vegetation.txt: %i\n",rx);


	/* - signature - 4 bytes */
	sprintf(sig,"%s","4WVR");
	fwrite(sig,4,1,outp);
	fprintf(text, "sig: %s\n",sig);

	/* X size 256 */		
	ver=MapSize;
	fwrite(&ver,4,1,outp);
	fprintf(text, "x size: %d\n",ver);

	/* Y size 256 */		
	fwrite(&ver,4,1,outp);
	fprintf(text, "y size: %d\n",ver);

 /* bmp stuff from com_gp */
 bmpman bmp1( MapSize-1, MapSize-1, 8 );
 RGBQUAD rgb_aux, rgb_aux_1;

// for( unsigned int x = 0; x < 256; x++){
  for( int xx = 0; xx < 256; xx++){
    rgb_aux.rgbRed = char(xx);
    rgb_aux.rgbGreen = char(xx);
    rgb_aux.rgbBlue = char(xx);
    bmp1.PutPalRGB( xx, rgb_aux );
 }

 ifstream file;//by Com_GP 26/01/03 15:35
 float x_aux, y_aux, z_aux;//by Com_GP 26/01/03 15:35
 int max_alt, pal;

// cout << "Maximum altitude -> ";
// cin >> max_alt;
   max_alt=2548;
 cout << endl;

 max_alt = max_alt + 100;
 pal = max_alt / 256;

 file.open("terrain.txt");//by Com_GP 26/01/03 15:35

	/* read Wilbur long/lat txt format elevations */
	//translates to OFP format: 
	//X, Z, Y
	for (int c1=0; c1 < MapSize*MapSize; c1++) {

        ReadCords( file, x_aux, y_aux, z_aux );//by Com_GP 26/01/03 15:35
        hoss1 = short( x_aux );//by Com_GP 26/01/03 15:35
        hoss2 = short( y_aux );//by Com_GP 26/01/03 15:36
        hoss3 = short( z_aux );//by Com_GP 26/01/03 15:36

        if (hoss3 == 0) hoss3 = -50;
        if (hoss3 > 0) hoss3 = hoss3 * 6;

        //To enter data in the array
        wrp[hoss1][hoss2].Elevation = hoss3;

	fwrite(&hoss3,2,1,outp);


    int y_temp = 10;
    for( int y = 0; y < max_alt; y = y + 10 ){
     if ((int(z_aux) >= y)&&(int(z_aux) < y_temp)){
        bmp1.EnterINDdata( hoss1, hoss2, int(y/pal) );
     }
     y_temp = y_temp + 10;
    }

}
    file.close();//by Com_GP 26/01/03 15:39
    x_aux=0;
    y_aux=0;
    z_aux=0;

    /* exited and now save the bmp */
    bmp1.LoadBMPHeader();
    bmp1.SaveBMP("name.bmp");

    bmpman bmp2(256,256,8);
    bmp2.LoadBMPHeader();
    bmp2.LoadBMP("land.bmp");

	/* texture indexes in short */
 file.open("terrain.txt");
// desert 255,204,102
// green 0,204,0
// water 0,204,255

	for (int ic=0; ic < MapSize*MapSize; ic++) {
        ReadCords( file, x_aux, y_aux, z_aux );
        hoss1 = short( x_aux );
        hoss2 = short( y_aux );
        hoss3 = short( z_aux );

        rgb_aux_1 = bmp2.GetClr( int (x_aux), int (y_aux) );
    TexIndex=1;
    if (rgb_aux_1.rgbRed==255 && rgb_aux_1.rgbGreen==204 && rgb_aux_1.rgbBlue==102)
    { TexIndex=1; }
    if (rgb_aux_1.rgbRed==0 && rgb_aux_1.rgbGreen==204 && rgb_aux_1.rgbBlue==0)
    { TexIndex=2; }

    //To enter data in the array
    wrp[hoss1][hoss2].TexIndex = TexIndex;


	fwrite(&TexIndex,2,1,outp);
	fprintf(text, "texture index: %d\n",TexIndex);
	}

    file.close();

	/* texture indexes in short */
	/* lets insert base textures */
	// landtext\s3.paa LandText\mo.pac
	sprintf(sig,"LandText\\mo.pac");
	fwrite(sig,32,1,outp);
	sprintf(sig,"landtext\\pi.paa");
	fwrite(sig,32,1,outp);
	sprintf(sig,"landtext\\s3.paa");
	fwrite(sig,32,1,outp);
	sprintf(sig,"landtext\\p4.paa");
	fwrite(sig,32,1,outp);
	sprintf(sig,"landtext\\bah.paa");
	fwrite(sig,32,1,outp);
	sprintf(sig,"landtext\\tt.paa");
	fwrite(sig,32,1,outp);

    /* write the null thing (but then OFP stops reading the WRP hehe)
    sig[0]='0';
    fwrite(sig,32,1,outp);
    */
	/* textures 32 char length and total of 512 */
	for (int ix=0; ix < 506; ix++) {

	fwrite(sig,32,1,outp);
	}

/*
	3d objects!
	9 floats, x coord, elevation, z coord, 3d object index, 3d object filename
	1,0,0,0,1,0,0,0,1 floats
	x/z coords 0 to 12800 in 256 map
	elevation 32768 and its "fit to ground".
*/

	dObjIndex=0;

	for (int cx=0; cx < 50; cx++) {

	dDirX=0;
	dDirZ=0;
	dDirX=1+(float) (MapSize*50*rand()/(RAND_MAX+1.0));
	dDirZ=1+(float) (MapSize*50*rand()/(RAND_MAX+1.0));

x=0,x=int (dDirX);
z=0,z=int (dDirZ);

if (wrp[x/50][z/50].Elevation > 6) {
	dDir=1;
	fwrite(&dDir,4,1,outp);
	dDir=0;
	fwrite(&dDir,4,1,outp);
	fwrite(&dDir,4,1,outp);
	fwrite(&dDir,4,1,outp);
	dDir=1;
	fwrite(&dDir,4,1,outp);
	dDir=0;
	fwrite(&dDir,4,1,outp);
	fwrite(&dDir,4,1,outp);
	fwrite(&dDir,4,1,outp);
	dDir=1;
	fwrite(&dDir,4,1,outp);

	fwrite(&dDirX,4,1,outp);
	dDir=32768;
	fwrite(&dDir,4,1,outp);
	fwrite(&dDirZ,4,1,outp);

	dObjIndex++;
	fwrite(&dObjIndex,4,1,outp);

	/* add the 3d objects randomly */
	rnd3d=1+(int) (rx*rand()/(RAND_MAX+1.0));
	sprintf(dObjName,Rnd3dObjN[rnd3d]);

	fwrite(dObjName,sizeof(dObjName),1,outp);
	fprintf(text,"%i 3dObj: %s in x: %f y: %f\n",rnd3d,dObjName,dDirX,dDirZ);
 }
}

	fclose(outp);
	fclose(text);
	return 0;
}

void ReadCords( ifstream & f, float & X, float & Y, float & Z )
{
    char *x_temp, *y_temp, *z_temp;

    f >> x_temp;
    f >> y_temp;
    f >> z_temp;
	double Xx,Yx,Zx;

    Xx = atof(x_temp);
    Yx = atof(y_temp);
    Zx = atof(z_temp);
}
