#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define NORMALS


//https://www.youtube.com/shorts/UaOqoL7sSfs
//It doesn't pass the checker at all. My values for the first 10 triangles are the same as the text file, excluding the ViewDirection. If i get the ViewDirection to match the text file, chunks of the image are missing. This is the best I can do, and I can't figure out why it won't pass the checker. 
typedef struct
{
    double          A[4][4];     // A[i][j] means row i, column j
				 //
} Matrix;


void
PrintMatrix(Matrix m)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        printf("(%.7f %.7f %.7f %.7f)\n", m.A[i][0], m.A[i][1], m.A[i][2], m.A[i][3]);
    }
}

Matrix
ComposeMatrices(Matrix M1, Matrix M2)
{
    Matrix m_out;
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
        {
            m_out.A[i][j] = 0;
            for (int k = 0 ; k < 4 ; k++)
                m_out.A[i][j] += M1.A[i][k]*M2.A[k][j];
        }
    return m_out;
}

void 
TransformPoint(Matrix m, const double *ptIn, double *ptOut)
{ //multiplies an array by a matrix and outputs.  
    ptOut[0] = ptIn[0]*m.A[0][0]
             + ptIn[1]*m.A[1][0]
             + ptIn[2]*m.A[2][0]
             + ptIn[3]*m.A[3][0];
    ptOut[1] = ptIn[0]*m.A[0][1]
             + ptIn[1]*m.A[1][1]
             + ptIn[2]*m.A[2][1]
             + ptIn[3]*m.A[3][1];
    ptOut[2] = ptIn[0]*m.A[0][2]
             + ptIn[1]*m.A[1][2]
             + ptIn[2]*m.A[2][2]
             + ptIn[3]*m.A[3][2];
    ptOut[3] = ptIn[0]*m.A[0][3]
             + ptIn[1]*m.A[1][3]
             + ptIn[2]*m.A[2][3]
             + ptIn[3]*m.A[3][3];
}

typedef struct
{
    double          near, far;
    double          angle;
    double          position[3];
    double          focus[3];
    double          up[3];
} Camera;

typedef struct 
{
    double lightDir[3]; // The direction of the light source
    double Ka;           // The coefficient for ambient lighting.
    double Kd;           // The coefficient for diffuse lighting.
    double Ks;           // The coefficient for specular lighting.
    double alpha;        // The exponent term for specular lighting.
} LightingParameters;

LightingParameters 
GetLighting(Camera c)
{
    LightingParameters lp;
    lp.Ka = 0.3;
    lp.Kd = 0.7;
    lp.Ks = 2.8;
    lp.alpha = 50.5;   //lightpos - frag-pos
		       //what is view pos... c.focus
    lp.lightDir[0] = c.position[0]-c.focus[0];
    lp.lightDir[1] = c.position[1]-c.focus[1];
    lp.lightDir[2] = c.position[2]-c.focus[2];
    double mag = sqrt(lp.lightDir[0]*lp.lightDir[0]
                    + lp.lightDir[1]*lp.lightDir[1]
                    + lp.lightDir[2]*lp.lightDir[2]);
    if (mag > 0)
    {
        lp.lightDir[0] /= mag;
        lp.lightDir[1] /= mag;
        lp.lightDir[2] /= mag;
    }

    return lp;
}

double SineParameterize(int curFrame, int nFrames, int ramp)
{  
    int nNonRamp = nFrames-2*ramp;
    double height = 1./(nNonRamp + 4*ramp/M_PI);
    if (curFrame < ramp)
    {
        double factor = 2*height*ramp/M_PI;
        double eval = cos(M_PI/2*((double)curFrame)/ramp);
        return (1.-eval)*factor;
    }
    else if (curFrame > nFrames-ramp)
    {        
        int amount_left = nFrames-curFrame;
        double factor = 2*height*ramp/M_PI;
        double eval =cos(M_PI/2*((double)amount_left/ramp));
        return 1. - (1-eval)*factor;
    }        
    double amount_in_quad = ((double)curFrame-ramp);
    double quad_part = amount_in_quad*height;
    double curve_part = height*(2*ramp)/M_PI;
    return quad_part+curve_part;
} 

Camera       
GetCamera(int frame, int nframes)
{            
    double t = SineParameterize(frame, nframes, nframes/10);
    Camera c;
    c.near = 5;
    c.far = 200;
    c.angle = M_PI/6;
    c.position[0] = 40*sin(2*M_PI*t);
    c.position[1] = 40*cos(2*M_PI*t);
    c.position[2] = 40;
    c.focus[0] = 0; 
    c.focus[1] = 0; 
    c.focus[2] = 0;
    c.up[0] = 0;    
    c.up[1] = 1;    
    c.up[2] = 0;    
    return c;       
}

double* CrossProduct(double *A, double *B)
{
	double *C = (double*)malloc(sizeof(double) * 3);
	C[0] = A[1] * B[2] - A[2] * B[1];
	C[1] = A[2] * B[0] - A[0] * B[2];
	C[2] = A[0] * B[1] - A[1] * B[0];
	return C;
}

Matrix GetViewTransform(Camera c)
{
    Matrix m;
    double arr[4][4] = {  {1/(tan(c.angle/2)), 0, 0, 0},
	     {0, 1/(tan(c.angle/2)), 0, 0},
	     {0, 0, (c.far + c.near)/(c.far-c.near), -1}, 
	     {0, 0, (2*c.far*c.near)/(c.far-c.near), 0}  };

    memcpy(m.A, arr, sizeof(double)*4*4);
    //PrintMatrix(m); 
    return m;
}
double* Normalize(double *A)
{
	double absolute; 
	absolute = sqrt((A[0]*A[0]) + (A[1]*A[1]) + (A[2]*A[2]));
	A[0] = A[0] / absolute;
	A[1] = A[1] / absolute;
	A[2] = A[2] / absolute;
	return A;
}

Matrix GetCameraTransform(Camera c)
{   
    Matrix rv;
    double *O = malloc(sizeof(double) *3);
    
    double *v = malloc(sizeof(double) * 3);
    double *u = malloc(sizeof(double) * 3);
    double *w = malloc(sizeof(double) * 3);
    double *t = malloc(sizeof(double) * 3);
    
    for( int i = 0; i < 3; i++)
    {
         O[i] = c.position[i];
	 w[i] = c.position[i] - c.focus[i];
	 v[i] = c.up[i]; //tempt
    	 t[i] = 0 - c.position[i];

    }
	u = CrossProduct(v, w);
	v = CrossProduct(w, u);
	
	//normalize
	u = Normalize(u);
	v = Normalize(v);
	w = Normalize(w);

   /* printf("focus is %lf, %lf, %lf\n", c.focus[0], c.focus[1], c.focus[2]);
	   printf("Frame: U = %lf, %lf, %lf\n", u[0], u[1], u[2]);
	   printf("Frame: V = %lf, %lf, %lf\n", v[0], v[1], v[2]);
	   printf("Frame: W = %lf, %lf, %lf\n", w[0], w[1], w[2]);
	   printf("Frame: O = %lf, %lf, %lf\n", O[0], O[1], O[2]);
*/
    double ut=0, vt=0, wt = 0; //
    for (int i = 0; i < 3; i++) //dot product
    {
	ut += u[i] * t[i];
	vt += v[i] * t[i];
	wt += w[i] * t[i];
    }
//	printf("ut: %lf, vt: %lf, wt: %lf\n", ut, vt, wt);
    double arr[4][4] = {{u[0], v[0], w[0], 0}, {u[1], v[1], w[1], 0},{u[2], v[2], w[2], 0},{ut, vt, wt, 1}};
    
    memcpy(rv.A, arr, sizeof(double)*4*4);
    
    return rv;
}

Matrix
GetDeviceTransform(Camera c)
{   
     Matrix rv;
     int n = 1000;
     int m = 1000;
     double arr[4][4] = {{n/2, 0, 0, 0},{0, m/2, 0, 0}, {0, 0, 1, 0}, {n/2, m/2, 0, 1}};
    
    memcpy(rv.A, arr, sizeof(double)*4*4);
    
    return rv;
}

typedef struct
{
   double         X[3];
   double         Y[3];
   double         Z[3];
   double         color[3][3]; // color[2][0] is for V2, red channel
#ifdef NORMALS
   double         normals[3][3]; // normals[2][0] is for V2, x-component
   double 	  shading[3]; //stores shading per vector
#endif
} Triangle;

typedef struct
{
   int numTriangles;
   Triangle *triangles;
} TriangleList;

char *
Read3Numbers(char *tmp, double *v1, double *v2, double *v3)
{
    *v1 = atof(tmp);
    while (*tmp != ' ')
       tmp++;
    tmp++; /* space */
    *v2 = atof(tmp);
    while (*tmp != ' ')
       tmp++;
    tmp++; /* space */
    *v3 = atof(tmp);
    while (*tmp != ' ' && *tmp != '\n')
       tmp++;
    return tmp;
}

TriangleList *
Get3DTriangles()
{
   FILE *f = fopen("ws_tris.txt", "r");
   if (f == NULL)
   {
       fprintf(stderr, "You must place the ws_tris.txt file in the current directory.\n");
       exit(EXIT_FAILURE);
   }
   fseek(f, 0, SEEK_END);
   int numBytes = ftell(f);
   fseek(f, 0, SEEK_SET);
   if (numBytes != 3892295)
   {
       fprintf(stderr, "Your ws_tris.txt file is corrupted.  It should be 3892295 bytes, but you have %d.\n", numBytes);
       exit(EXIT_FAILURE);
   }

   char *buffer = (char *) malloc(numBytes);
   if (buffer == NULL)
   {
       fprintf(stderr, "Unable to allocate enough memory to load file.\n");
       exit(EXIT_FAILURE);
   }
   
   fread(buffer, sizeof(char), numBytes, f);

   char *tmp = buffer;
   int numTriangles = atoi(tmp);
   while (*tmp != '\n')
       tmp++;
   tmp++;
 
   if (numTriangles != 14702)
   {
       fprintf(stderr, "Issue with reading file -- can't establish number of triangles.\n");
       exit(EXIT_FAILURE);
   }

   TriangleList *tl = (TriangleList *) malloc(sizeof(TriangleList));
   tl->numTriangles = numTriangles;
   tl->triangles = (Triangle *) malloc(sizeof(Triangle)*tl->numTriangles);

   for (int i = 0 ; i < tl->numTriangles ; i++)
   {
       for (int j = 0 ; j < 3 ; j++)
       {
           double x, y, z;
           double r, g, b;
           double normals[3];
/*
 * Weird: sscanf has a terrible implementation for large strings.
 * When I did the code below, it did not finish after 45 minutes.
 * Reading up on the topic, it sounds like it is a known issue that
 * sscanf fails here.  Stunningly, fscanf would have been faster.
 *     sscanf(tmp, "(%lf, %lf), (%lf, %lf), (%lf, %lf) = (%d, %d, %d)\n%n",
 *              &x1, &y1, &x2, &y2, &x3, &y3, &r, &g, &b, &numRead);
 *
 *  So, instead, do it all with atof/atoi and advancing through the buffer manually...
 */
           tmp = Read3Numbers(tmp, &x, &y, &z);
           tmp += 3; /* space+slash+space */
           tmp = Read3Numbers(tmp, &r, &g, &b);
           tmp += 3; /* space+slash+space */
           tmp = Read3Numbers(tmp, normals+0, normals+1, normals+2);
           tmp++;    /* newline */

           tl->triangles[i].X[j] = x;
           tl->triangles[i].Y[j] = y;
           tl->triangles[i].Z[j] = z;
           tl->triangles[i].color[j][0] = r;
           tl->triangles[i].color[j][1] = g;
           tl->triangles[i].color[j][2] = b;
#ifdef NORMALS
           tl->triangles[i].normals[j][0] = normals[0];
           tl->triangles[i].normals[j][1] = normals[1];
           tl->triangles[i].normals[j][2] = normals[2];
#endif
       }
   }

   free(buffer);
   return tl;
}

typedef struct image
{
    int rows;
    int cols;
    unsigned char* data;
    float* depthBuffer; //just the current z at a current pixel

}Image;


double C441(double f)
{
    return ceil(f-0.00001);
}

double F441(double f)
{
    return floor(f+0.00001);
}


void WriteImage(Image *img, const char* filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    	return;
    fprintf(fp, "P6\n");
    fprintf(fp, "1000 1000\n");
    fprintf(fp, "255\n");

    fwrite(img->data, sizeof(unsigned char), 1000 * 1000 * 3, fp);
    fclose(fp);

}    
void ImageColor(Image *img, int index, TriangleList *tl, int r, int c, double z4_r, double z4_g, double z4_b)
{
    int new_r = 1000-r-1;
    if (new_r < 0 || new_r >= 1000 || c < 0 || c >= 1000 )
        return;
    int img_index = 3*(new_r*1000 + c);
    
    unsigned char red = 255* fmin(1, z4_r);
    unsigned char green = 255* fmin(1, z4_g);
    unsigned char blue = 255* fmin(1, z4_b);
    img->data[img_index+0] = red;
    img->data[img_index+1] = green;
    img->data[img_index+2] = blue;
}

int FindVertices(TriangleList *tl, int i, char find)
{
	double y0 = tl->triangles[i].Y[0];
	double y1 = tl->triangles[i].Y[1];
	double y2 = tl->triangles[i].Y[2];
	double x0 = tl->triangles[i].X[0];
	double x1 = tl->triangles[i].X[1];
	double x2 = tl->triangles[i].X[2];
	double slope;
	double intercept;
	int top; //in y direction
	int middle; 
	int bottom;
	if ((y0 <= y1) && (y0 <= y2)){ //0 is bottom
    	    bottom = 0;
	    if (y1 < y2){
		    middle = 1;
		    top = 2;
	    }
	    else{
		middle = 2;
		top = 1;
	    }
	}
    //1 is bottom------------------------------------------		
	else if ((y1 <= y0) && (y1 <= y2)){ //1 is bottom
    	    bottom = 1;
	    if (y0 < y2)
	    {
		    middle = 0;
		    top = 2;
	    }
	    else
	    {
		middle = 2;
		top = 0;
	    }
    }	
	//2 is the bottom-------------------
	else if ((y2 <= y0) && (y2 <= y1)){ //0 is bottom
    	    bottom = 2;
	    if (y0 < y1) {
		    middle = 0;
		    top = 1;
	    }
	    else {
		middle = 1;
		top = 0;
	    }
	}
       if (find == 't')
	    return top;
       else if (find == 'm')
	    return middle;
       else if (find == 'b')
	    return bottom;


}

double CalculateShading(LightingParameters lp, double *viewDirection, double *normal)
{
     
	double ambient = lp.Ka;
	//printf("Normal is %lf, %lf, %lf\n", normal[0], normal[1], normal[2]);
	
	double LdotN = (normal[0] * lp.lightDir[0] + normal[1] * lp.lightDir[1] + normal[2] * lp.lightDir[2]);
	//printf("LdotN is %lf\n", LdotN); 
	
	double diffuse = fmax(0.0,LdotN) * lp.Kd;
	
	double* R = malloc(sizeof(double) * 3);
	for (int i = 0; i < 3; i++)
	{
		R[i] = 2* (LdotN) * normal[i] - lp.lightDir[i];
	}
	//printf("reflection vector R is %lf, %lf, %lf\n", R[0], R[1], R[2]);
	
	double RdotV = (R[0] * viewDirection[0] + R[1] * viewDirection[1] + R[2] * viewDirection[2]);
	//printf("RdotV is %lf\n", RdotV);

	double specular = pow(fmax(0, RdotV), lp.alpha);
	specular = lp.Ks * specular;
	
	double shading = ambient +diffuse + specular;
       // printf("shading = %lf\n", shading);
	
	return shading;

}	


void TransformAndRenderTriangles(Camera c, TriangleList* tl, Image *img, int i, double* view_bot, double* view_mid, double* view_top)
{

   int top = FindVertices(tl, i, 't'); 
   int middle = FindVertices(tl, i, 'm');
   int bottom = FindVertices(tl, i, 'b');

//printf("top = %d, mid = %d, bot = %d\n", top, middle, bottom);
   double top_x = tl->triangles[i].X[top];
   double top_y = tl->triangles[i].Y[top];
   double top_z = tl->triangles[i].Z[top];
   double bot_x = tl->triangles[i].X[bottom];
   double bot_y = tl->triangles[i].Y[bottom];
   double bot_z = tl->triangles[i].Z[bottom];
   double mid_x = tl->triangles[i].X[middle];
   double mid_y = tl->triangles[i].Y[middle];
   double mid_z = tl->triangles[i].Z[middle];
   
   double* v0 = malloc(sizeof(double)* 4);
    double* new_v0 = malloc(sizeof(double)* 4);
    double* v1 = malloc(sizeof(double)* 4);
    double* new_v1 = malloc(sizeof(double)* 4);
    double* v2 = malloc(sizeof(double)* 4);
    double* new_v2 = malloc(sizeof(double)* 4);
    
    Matrix M, CT, VT, DT, CVT;
	CT = GetCameraTransform(c);
	VT = GetViewTransform(c);
	DT = GetDeviceTransform(c); //for final matrix
	CVT = ComposeMatrices(CT, VT);
	M = ComposeMatrices(CVT, DT);	
	v0[0] = tl->triangles[i].X[0];
	v0[1] = tl->triangles[i].Y[0];
	v0[2] = tl->triangles[i].Z[0];
	v0[3] = 1;
	v1[0] = tl->triangles[i].X[1];
	v1[1] = tl->triangles[i].Y[1];
	v1[2] = tl->triangles[i].Z[1];
	v1[3] = 1;
	v2[0] = tl->triangles[i].X[2];
	v2[1] = tl->triangles[i].Y[2];
	v2[2] = tl->triangles[i].Z[2];
	v2[3] = 1;


   view_bot[0] = c.position[0] - bot_x;
   view_bot[1] = c.position[1] - bot_y;
   view_bot[2] = c.position[2] - bot_z;
     
   view_bot = Normalize(view_bot); 
//printf("1view_bot = %lf, %lf, %lf\n", view_bot[0], view_bot[1], view_bot[2]);
   view_mid[0] = c.position[0] - mid_x;
   view_mid[1] = c.position[1] - mid_y;
   view_mid[2] = c.position[2] - mid_z;
   view_mid = Normalize(view_mid);

//printf("1view_mid = %lf, %lf, %lf\n", view_mid[0], view_mid[1], view_mid[2]);
   view_top[0] = c.position[0] - top_x;
   view_top[1] = c.position[1] - top_y;
   view_top[2] = c.position[2] - top_z;
   view_top = Normalize(view_top);
   
//printf("1view_top = %lf, %lf, %lf\n", view_top[0], view_top[1], view_top[2]);

   
   //shading factor
       //------------- 
	TransformPoint(M, v0, new_v0); //maybe call compose matrices here
	TransformPoint(M, v1, new_v1);			
	TransformPoint(M, v2, new_v2);

	tl->triangles[i].X[0] = new_v0[0]/new_v0[3];
	tl->triangles[i].Y[0] = new_v0[1]/new_v0[3];
	tl->triangles[i].Z[0] = new_v0[2]/new_v0[3];
	tl->triangles[i].X[1] = new_v1[0]/new_v1[3];
	tl->triangles[i].Y[1] = new_v1[1]/new_v1[3];
	tl->triangles[i].Z[1] = new_v1[2]/new_v1[3];
	tl->triangles[i].X[2] = new_v2[0]/new_v2[3];
	tl->triangles[i].Y[2] = new_v2[1]/new_v2[3];
	tl->triangles[i].Z[2] = new_v2[2]/new_v2[3];
        	
}


void RasterizeTriangle(TriangleList* tl, int index, Image *img, double*view_bot, double* view_mid, double* view_top, LightingParameters lp, Camera c)
{
   int top = FindVertices(tl, index, 't'); //bottom maybe change later
   int middle = FindVertices(tl, index, 'm');
   int bottom = FindVertices(tl, index, 'b');

   double* bot_n = malloc(sizeof(double)*3);
   double* mid_n = malloc(sizeof(double)*3);
   double* top_n = malloc(sizeof(double)*3);

   double top_x = tl->triangles[index].X[top];
   double top_y = tl->triangles[index].Y[top];
   double top_z = tl->triangles[index].Z[top];
   double bot_x = tl->triangles[index].X[bottom];
   double bot_y = tl->triangles[index].Y[bottom];
   double bot_z = tl->triangles[index].Z[bottom];
   double mid_x = tl->triangles[index].X[middle];
   double mid_y = tl->triangles[index].Y[middle];
   double mid_z = tl->triangles[index].Z[middle];
   //printf("bot %d, top %d, mid %d\n", bottom, top, middle);
   for (int i = 0; i < 3; i++)
   {
     bot_n[i] = tl->triangles[index].normals[bottom][i];
     mid_n[i] = tl->triangles[index].normals[middle][i];
     top_n[i] = tl->triangles[index].normals[top][i];
   }
   //shading factor
 // printf("for vertex %d\n", bottom);
 //
   double bot_s = CalculateShading(lp, view_bot, bot_n);
  // printf("vertex %d\n", middle);
   double mid_s = CalculateShading(lp, view_mid, mid_n);
  // printf("vertex %d\n", top);
   double top_s = CalculateShading(lp, view_top, top_n);
   
//transform now

   double leftEnd, rightEnd;

   double bot_r = tl->triangles[index].color[bottom][0];
   double bot_g = tl->triangles[index].color[bottom][1];
   double bot_b = tl->triangles[index].color[bottom][2];
   double mid_r = tl->triangles[index].color[middle][0];
   double mid_g = tl->triangles[index].color[middle][1];
   double mid_b = tl->triangles[index].color[middle][2];
   double top_r = tl->triangles[index].color[top][0];
   double top_g = tl->triangles[index].color[top][1];
   double top_b = tl->triangles[index].color[top][2];

   double slope_t, intercept_t, slope_m_t, intercept_m_t, slope_m, intercept_m;
   float z4, z5,z6;
   double z4_r, z4_g, z4_b, z5_r, z5_g, z5_b, z6_r, z6_g, z6_b;

//LERP THE SHADING
   double z4_s, z5_s, z6_s; //shading
   //normals
   
   if (bot_x != top_x)
   {
      slope_t = (top_y - bot_y)/(top_x - bot_x);
      intercept_t = top_y - (slope_t * top_x);
   } 
   if (mid_x != top_x)
   {
      slope_m_t= (top_y - mid_y) /(top_x - mid_x);
      intercept_m_t = top_y - (slope_m_t * top_x);
   }
   if (mid_x != bot_x)
   {
      slope_m = (mid_y - bot_y)/(mid_x - bot_x);
      intercept_m = mid_y - (slope_m * mid_x); 
   }
   double intercept1;
   double rowMin = C441(bot_y);
   double rowMax = F441(top_y);

   for (int r = rowMin; r <= rowMax; r++)
   {
	if (bot_x == top_x)
		intercept1 = bot_x; //vertical line
	else			    
 		intercept1 = (r - intercept_t)/slope_t;
	double intercept2;
	if (r < mid_y) //scanning bottom
	{
		if(mid_x ==bot_x)
	   		intercept2 = mid_x;
		else if (slope_m == '\0')
			intercept2 = mid_x; 
		else
 	     		intercept2 = (r - intercept_m)/slope_m;
	}
	else { //scanning top
		if (top_x == mid_x)
			intercept2 = mid_x;
		else if (slope_m_t == '\0')
			intercept2 = mid_x;
		else 
			intercept2 = (r - intercept_m_t)/slope_m_t;
	}	
	if(intercept1 < intercept2){
		leftEnd = intercept1;
		rightEnd = intercept2;
	}
	else{
	      rightEnd = intercept1;
	      leftEnd = intercept2;
	}
	
		z5 = bot_z + (r - bot_y)/(top_y - bot_y) * (top_z - bot_z);
	        z5_r = bot_r + (r - bot_y)/(top_y - bot_y) * (top_r - bot_r);
	        z5_g = bot_g + (r - bot_y)/(top_y - bot_y) * (top_g - bot_g);
	        z5_b = bot_b + (r - bot_y)/(top_y - bot_y) * (top_b - bot_b);
		z5_s = bot_s + (r - bot_y)/(top_y - bot_y) * (top_s - bot_s);

	      if (r < mid_y)
	      {
		z6 = bot_z + (r - bot_y) / (mid_y - bot_y) * (mid_z - bot_z);
	        z6_r = bot_r + (r - bot_y) / (mid_y - bot_y) * (mid_r - bot_r);
	        z6_g = bot_g + (r - bot_y) / (mid_y - bot_y) * (mid_g - bot_g);
	      	z6_b = bot_b + (r - bot_y) / (mid_y - bot_y) * (mid_b - bot_b);
	        z6_s = bot_s + (r - bot_y) / (mid_y - bot_y) * (mid_s - bot_s);
	      }
	      else{
		z6 = mid_z + (r - mid_y) / (top_y - mid_y) * (top_z - mid_z);
		z6_r = mid_r + (r - mid_y) / (top_y - mid_y) * (top_r - mid_r);
		z6_g = mid_g + (r - mid_y) / (top_y - mid_y) * (top_g - mid_g);
		z6_b = mid_b + (r - mid_y) / (top_y - mid_y) * (top_b - mid_b);
		z6_s = mid_s + (r - mid_y) / (top_y - mid_y) * (top_s - mid_s);
	      }
	      //-------------
     for(int c = C441(leftEnd); c <= F441(rightEnd); c++)
    {
	     if (intercept1 == leftEnd){ //z5 is left
		  z4 = z5 + (c - leftEnd)/(rightEnd - leftEnd) * (z6 - z5);
		  z4_r = z5_r + (c- leftEnd)/ (rightEnd - leftEnd) * (z6_r - z5_r); 		  
		  z4_g = z5_g + (c- leftEnd)/ (rightEnd - leftEnd) * (z6_g - z5_g); 
		  z4_b = z5_b + (c- leftEnd)/ (rightEnd - leftEnd) * (z6_b - z5_b); 
	     	  z4_s = z5_s + (c - leftEnd)/ (rightEnd - leftEnd) * (z6_s - z5_s);
	     }
	     else if(intercept2 == leftEnd) //z5 is right	   
	     {
	          z4 = z6 + (c - leftEnd)/(rightEnd - leftEnd) * (z5 - z6);
		  z4_r = z6_r + (c- leftEnd)/ (rightEnd - leftEnd) *
			  (z5_r - z6_r); 
		  z4_g = z6_g + (c- leftEnd)/ (rightEnd - leftEnd) *
			  (z5_g - z6_g); 
		  z4_b = z6_b + (c- leftEnd)/ (rightEnd - leftEnd) *
			  (z5_b - z6_b); 	     
		  z4_s = z6_s + (c- leftEnd)/ (rightEnd - leftEnd) * 
			  (z5_s - z6_s);
	     }
	     if (r >=0 && r <1000 && c>= 0 && c < 1000 && z4 > img->depthBuffer[r*1000+c] && z4 < 0)
	     {
		     
                  ImageColor(img, index, tl, r, c, z4_r*z4_s, z4_g*z4_s, z4_b*z4_s);
	 	  img->depthBuffer[r*1000+c] = z4;
	     }
     }
   }
}


int main()
{
    TriangleList *tl = Get3DTriangles(); //get triangle datai
    int count = 0;
    int top, middle, bottom;
    struct image *img = (struct image*)malloc(sizeof(struct image));
    img->depthBuffer = (float*)malloc(1000*1000 * sizeof(float) + 1);
    img->data = (unsigned char*)malloc(1000*1000 * 3);//*100
   
    
    char* filename = (char*)malloc(sizeof(char) * 30);
   
   // for (int i = 0; i < 1000; i++)
   // {//this is the shift in camera focus
   
//	if (i % 250 != 0)
  //  		continue;
  	 for (int j = 0; j < 1000*1000; j++)
	 {
		img->depthBuffer[j] =-1;
	 }
  	for (int k =0; k < 1000* 1000 * 3; k++)
    	{
		img->data[k] = 0;
    	}

     Camera c = GetCamera(0, 1000);
     double* view_bot = malloc(sizeof(double)*3);
     double* view_mid = malloc(sizeof(double)*3);
     double* view_top = malloc(sizeof(double)*3);
     //for (int l = 0; l <8; l++)
  for (int l = 0; l < tl->numTriangles; l++)
     {
 //       top = FindVertices(tl, l, 't'); //bottom maybe change later
   //	middle = FindVertices(tl,l, 'm');
   //	bottom = FindVertices(tl, l, 'b');
	     //printf("Triangle %d\n", l);
	    LightingParameters lp = GetLighting(c);
	    TransformAndRenderTriangles(c, tl, img, l, view_bot, view_mid, view_top);
	    RasterizeTriangle(tl, l, img, view_bot, view_mid, view_top, lp, c);
     }
   //     sprintf(filename, "proj1F_frame%04d.pnm", i);
        filename = "proj1F_frame0000.pnm";
        WriteImage(img, filename);
	tl = Get3DTriangles();
//   }
}

