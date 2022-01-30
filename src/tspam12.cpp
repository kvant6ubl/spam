
#include "tspam12.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include "auxiliary.h"

#define PNG_DEBUG 3
#include <png.h>

using namespace std;
using namespace boost;

static int d[16] = {0, 1, 0, -1, 1, 0, -1, 0, 1, 1, -1, -1, 1, -1, -1, 1};

TSpam12::TSpam12(std::string fileName, int T1, int T2)
{
	/*first, we load the image and pad it */
	loadImage(fileName);

	this->T1 = T1;
	this->T2 = T2;
	P1 = vector<shared_array<double>>(8);
	F1 = vector<shared_array<double>>(8);
	P2 = vector<shared_array<double>>(8);
	F2 = vector<shared_array<double>>(8);

	/*than, we calculate 2nd order spam features of padded image */
	shared_array<int> diff;
	/*features in horizontal direction */
	diff = subIm(3 * pWidth + 3, 3 * pWidth + 4, height, width - 1); // im[:,0:xSize-1]-im[:,1:xSize]
	cooc(0, 1, width - 2, height, diff, width - 1, 0);
	cooc2nd(0, 1, 2, width - 3, height, diff, width - 1, 0);

	diff = subIm(3 * pWidth + 4, 3 * pWidth + 3, height, width - 1); // im[:,1:xSize]-im[:,0:xSize-1]
	cooc(1, 0, width - 2, height, diff, width - 1, 1);
	cooc2nd(2, 1, 0, width - 3, height, diff, width - 1, 1);

	/*features in vertical direction */
	diff = subIm(3 * pWidth + 3, 4 * pWidth + 3, height - 1, width); // im[0:ySize-1,:]-im[1:ySize,:]
	cooc(0, width, width, height - 2, diff, width, 2);
	cooc2nd(0, width, 2 * width, width, height - 3, diff, width, 2);

	diff = subIm(4 * pWidth + 3, 3 * pWidth + 3, height - 1, width); // im[1:ySize,:]-im[0:ySize-1,:]
	cooc(width, 0, width, height - 2, diff, width, 3);
	cooc2nd(2 * width, width, 0, width, height - 3, diff, width, 3);

	/*features along main diagonal */
	diff = subIm(3 * pWidth + 3, 4 * pWidth + 4, height - 1, width - 1); // im[0:ySize-1,0:xSize-1]-im[1:ySize,1:xSize]
	cooc(0, (width - 1) + 1, width - 2, height - 2, diff, width - 1, 4);
	cooc2nd(0, (width - 1) + 1, 2 * (width - 1) + 2, width - 3, height - 3, diff, width - 1, 4);

	diff = subIm(4 * pWidth + 4, 3 * pWidth + 3, height - 1, width - 1); // im[1:ySize,1:xSize]-im[0:ySize-1,0:xSize-1]
	cooc((width - 1) + 1, 0, width - 2, height - 2, diff, width - 1, 5);
	cooc2nd(2 * (width - 1) + 2, (width - 1) + 1, 0, width - 3, height - 3, diff, width - 1, 5);

	/* features along minor diagonal */
	diff = subIm(3 * pWidth + 4, 4 * pWidth + 3, height - 1, width - 1); // im[0:ySize-1,1:xSize]-im[1:ySize,0:xSize-1]
	cooc(1, width - 1, width - 2, height - 2, diff, width - 1, 6);
	cooc2nd(2, (width - 1) + 1, 2 * (width - 1), width - 3, height - 3, diff, width - 1, 6);

	diff = subIm(4 * pWidth + 3, 3 * pWidth + 4, height - 1, width - 1); // im[1:ySize,0:xSize-1]-im[0:ySize-1,1:xSize]
	cooc(width - 1, 1, width - 2, height - 2, diff, width - 1, 7);
	cooc2nd(2 * (width - 1), (width - 1) + 1, 2, width - 3, height - 3, diff, width - 1, 7);
}

/** Substracts two submatrices of im (padded) (M1 - M2). Submatrices have to
have same dimension rows x cols.
@param stIndex1 starting index of submatrix M1
@param stIndex2 starting index of submatrix M2
@param rows number of rows of submatrix
@param cols number of columns of submatrix
@return matrix rows-cols containing M1-M2*/
shared_array<int> TSpam12::subIm(int stIndex1, int stIndex2, int rows, int cols)
{
	/*first, let's allocate space for the output */
	shared_array<int> subIm(new int[rows * cols]);
	/*than let's do for copy...*/
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			subIm[i * cols + j] = im[stIndex1 + i * pWidth + j] - im[stIndex2 + i * pWidth + j];
		}
	}
	return subIm;
}

/** Calculates number of cooccuureces in the matrix diff on the same offsets starting from x1,x2,and y.
The cooccurences are calculated in the range [-T,...,+T].
Moreover, the number of cooccurences of x1 and x2 are calculated so we will be able to calculate conditional
probabilities. Results are stored in F and P keyed by key.
@param x1 starting offset of the first sub-matrix
@param x2 starting offset of the second sub-matrix
@param y starting offset of the third sub-matrix
@param cols number of columns of submatrices x1,x2 and y
@param rows number of columns of submatrices x1,x2 and y
@param diff matrix within which x1,x2, and x3 are calculated
@param dColls number of collumns of matrix diff
@param key index to P and F, where the cooccurences will be stored */
void TSpam12::cooc2nd(int x1, int x2, int y, int cols, int rows, shared_array<int> diff, int dColls, int key)
{
	int t = 2 * T2 + 1;
	shared_array<double> f(new double[t * t * t]);
	shared_array<double> p(new double[t * t]);
	for (int i = 0; i < t * t * t; i++)
	{
		f[i] = 0.0;
	}

	for (int i = 0; i < t * t; i++)
	{
		p[i] = 0.0;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int dx1 = diff[x1 + i * dColls + j];
			int dx2 = diff[x2 + i * dColls + j];

			if ((abs(dx1) <= T2) && (abs(dx2) <= T2))
			{
				p[(dx1 + T2) * t + (dx2 + T2)] = p[(dx1 + T2) * t + (dx2 + T2)] + 1;
				int dy = diff[y + i * dColls + j];
				if (abs(dy) <= T2)
				{
					f[(dx1 + T2) * t * t + (dx2 + T2) * t + (dy + T2)] = f[(dx1 + T2) * t * t + (dx2 + T2) * t + (dy + T2)] + 1;
				}
			}
		}
	}

	F2[key] = f;
	P2[key] = p;
}

/** Calculates number of cooccuureces in the matrix diff on the same offsets starting from x1,x2,and y.
The cooccurences are calculated in the range [-T,...,+T].
Moreover, the number of cooccurences of x1 and x2 are calculated so we will be able to calculate conditional
probabilities. Results are stored in F and P keyed by key.
@param x1 starting offset of the first sub-matrix
@param y starting offset of the second sub-matrix
@param cols number of columns of submatrices x1,x2 and y
@param rows number of columns of submatrices x1,x2 and y
@param diff matrix within which x1,x2, and x3 are calculated
@param dColls number of collumns of matrix diff
@param key index to P and F, where the cooccurences will be stored */
void TSpam12::cooc(int x1, int y, int cols, int rows, shared_array<int> diff, int dColls, int key)
{
	int t = 2 * T1 + 1;
	shared_array<double> f(new double[t * t]);
	shared_array<double> p(new double[t]);
	for (int i = 0; i < t * t; i++)
	{
		f[i] = 0.0;
	}

	for (int i = 0; i < t; i++)
	{
		p[i] = 0.0;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int dx1 = diff[x1 + i * dColls + j];

			if (abs(dx1) <= T1)
			{
				p[(dx1 + T1)] = p[(dx1 + T1)] + 1;
				int dy = diff[y + i * dColls + j];
				if (abs(dy) <= T1)
				{
					f[(dx1 + T1) * t + (dy + T1)] = f[(dx1 + T1) * t + (dy + T1)] + 1;
				}
			}
		}
	}

	F1[key] = f;
	P1[key] = p;
}

/** Loads image to the array im. It fils the width, height, pWidth, pHeight approariately.
@param fileName path to the image to load */
void TSpam12::loadImage(std::string fileName)
{
	png_byte header[8]; // 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(fileName.c_str(), "rb");
	if (!fp)
		throw TSpamEx("[read_png_file] File " + fileName + " could not be opened for reading");
	if (fread(header, 1, 8, fp) == 0)
		throw TSpamEx("[read_png_file] File " + fileName + " is empty");
	if (png_sig_cmp(header, 0, 8))
		throw TSpamEx("[read_png_file] File " + fileName + " is not recognized as a PNG file");

	/* initialize stuff */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		throw TSpamEx("[read_png_file] png_create_read_struct failed");

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		throw TSpamEx("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		throw TSpamEx("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// this->width = info_ptr->width;
	// this->height = info_ptr->height;
	this->width  = png_get_image_width(png_ptr, info_ptr);
	this->height = png_get_image_height(png_ptr, info_ptr);

	if (png_get_channels(png_ptr, info_ptr) != 1)
	{
		throw TSpamEx("sorry, but program supports only images with one channel.\nWe are academics, not business\nuse convert -type grayscale");
	}
	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_GRAY)
	{
		throw TSpamEx("sorry, but program supports only greyscale images.\nWe are academics, not business\nuse convert -type grayscale");
	}
	if (png_get_bit_depth(png_ptr, info_ptr) != 8)
	{
		throw TSpamEx("sorry, but program supports only images with 8 bits color depth.\nWe are academics, not business\nuse convert -type grayscale");
	}

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		throw TSpamEx("[read_png_file] Error during read_image");

	/* Allocate the memory to read the rows of the image */
	png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++)
		row_pointers[y] = (png_byte *)malloc(sizeof(png_bytep) * (png_get_rowbytes(png_ptr, info_ptr)));

	/* Read the image. */
	png_read_image(png_ptr, row_pointers);

	/*close the file */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);

	/*now, allocate the space for a local "padded" copy of the image and copy it there */
	pWidth = width + 6;
	pHeight = height + 6;
	this->im = shared_array<int>(new int[pWidth * pHeight]);
	for (int i = 0; i < height; i++)
	{
		png_bytep row = row_pointers[i];
		for (int j = 0; j < width; j++)
		{
			im[(i + 3) * pWidth + j + 3] = row[j];
		}
	}

	/*pad the image for the easier calculation afterwards */
	for (int j = 0; j < pWidth; j++)
	{
		im[0 * pWidth + j] = -1536;
		im[(height + 5) * pWidth + j] = -1536;
		im[1 * pWidth + j] = -1024;
		im[(height + 4) * pWidth + j] = -1024;
		im[2 * pWidth + j] = -512;
		im[(height + 3) * pWidth + j] = -512;
	}

	for (int i = 0; i < pHeight; i++)
	{
		im[i * pWidth + 0] = -1536;
		im[i * pWidth + width + 5] = -1536;
	}
	for (int i = 1; i < pHeight - 1; i++)
	{
		im[i * pWidth + 1] = -1024;
		im[i * pWidth + width + 4] = -1024;
	}
	for (int i = 2; i < pHeight - 2; i++)
	{
		im[i * pWidth + 2] = -512;
		im[i * pWidth + width + 3] = -512;
	}

	/*finally, de-allocate the image from the memory*/
	for (int y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);
	// 	cout <<"loaded image "<<fileName<<" "<<width<<"x"<<height<<endl;
}

/** Change the pixel of the image at location x,y to value v.
Matrices for the calculation of the features are appropriately recalculated.
@param x x coordinate
@param y y coordinate
@param v new value of the pixel */
void TSpam12::setPixel(int x, int y, int v)
{
	/*move to the padded  coordinates */
	y = y + 3;
	x = x + 3;

	shared_array<int> oldD(new int[6 * 8]);
	shared_array<int> newD(new int[6 * 8]);
	for (int i = 0; i < 8; i++)
	{
		oldD[6 * i + 0] = im[(y - 3 * d[2 * i + 0]) * pWidth + x - 3 * d[2 * i + 1]] - im[(y - 2 * d[2 * i + 0]) * pWidth + x - 2 * d[2 * i + 1]];
		oldD[6 * i + 1] = im[(y - 2 * d[2 * i + 0]) * pWidth + x - 2 * d[2 * i + 1]] - im[(y - d[2 * i + 0]) * pWidth + x - d[2 * i + 1]];
		oldD[6 * i + 2] = im[(y - d[2 * i + 0]) * pWidth + x - d[2 * i + 1]] - im[(y)*pWidth + x];
		oldD[6 * i + 3] = im[(y)*pWidth + x] - im[(y + d[2 * i + 0]) * pWidth + x + d[2 * i + 1]];
		oldD[6 * i + 4] = im[(y + d[2 * i + 0]) * pWidth + x + d[2 * i + 1]] - im[(y + 2 * d[2 * i + 0]) * pWidth + x + 2 * d[2 * i + 1]];
		oldD[6 * i + 5] = im[(y + 2 * d[2 * i + 0]) * pWidth + x + 2 * d[2 * i + 1]] - im[(y + 3 * d[2 * i + 0]) * pWidth + x + 3 * d[2 * i + 1]];
	}

	/*now, we change the value of the pixel*/
	im[y * pWidth + x] = v;

	for (int i = 0; i < 8; i++)
	{
		newD[6 * i + 0] = im[(y - 3 * d[2 * i + 0]) * pWidth + x - 3 * d[2 * i + 1]] - im[(y - 2 * d[2 * i + 0]) * pWidth + x - 2 * d[2 * i + 1]];
		newD[6 * i + 1] = im[(y - 2 * d[2 * i + 0]) * pWidth + x - 2 * d[2 * i + 1]] - im[(y - d[2 * i + 0]) * pWidth + x - d[2 * i + 1]];
		newD[6 * i + 2] = im[(y - d[2 * i + 0]) * pWidth + x - d[2 * i + 1]] - im[(y)*pWidth + x];
		newD[6 * i + 3] = im[(y)*pWidth + x] - im[(y + d[2 * i + 0]) * pWidth + x + d[2 * i + 1]];
		newD[6 * i + 4] = im[(y + d[2 * i + 0]) * pWidth + x + d[2 * i + 1]] - im[(y + 2 * d[2 * i + 0]) * pWidth + x + 2 * d[2 * i + 1]];
		newD[6 * i + 5] = im[(y + 2 * d[2 * i + 0]) * pWidth + x + 2 * d[2 * i + 1]] - im[(y + 3 * d[2 * i + 0]) * pWidth + x + 3 * d[2 * i + 1]];
	}

	/*finally, we have to update statistics*/
	int t1 = 2 * T1 + 1;
	int t2 = 2 * T2 + 1;
	for (int i = 0; i < 8; i++)
	{
		shared_array<double> p2 = P2[i];
		shared_array<double> f2 = F2[i];
		for (int j = 0; j < 4; j++)
		{
			int x1 = oldD[6 * i + j];
			int x2 = oldD[6 * i + j + 1];
			int py = oldD[6 * i + j + 2];
			if ((abs(x1) <= T2) && (abs(x2) <= T2))
			{
				if (abs(py) <= 255)
				{
					p2[(x1 + T2) * t2 + x2 + T2] -= 1;
				}
				if (abs(py) <= T2)
				{
					f2[(x1 + T2) * t2 * t2 + (x2 + T2) * t2 + py + T2] -= 1;
				}
			}

			x1 = newD[6 * i + j];
			x2 = newD[6 * i + j + 1];
			py = newD[6 * i + j + 2];
			if ((abs(x1) <= T2) && (abs(x2) <= T2))
			{
				if (abs(py) <= 255)
				{
					p2[(x1 + T2) * t2 + x2 + T2] += 1;
				}
				if (abs(py) <= T2)
				{
					f2[(x1 + T2) * t2 * t2 + (x2 + T2) * t2 + py + T2] += 1;
				}
			}
		}

		/*Update 1st order SPAM features */
		shared_array<double> p1 = P1[i];
		shared_array<double> f1 = F1[i];
		for (int j = 0; j < 5; j++)
		{
			int x1 = oldD[6 * i + j];
			int py = oldD[6 * i + j + 1];
			if (abs(x1) <= T1)
			{
				if (abs(py) <= 255)
				{
					p1[(x1 + T1)] -= 1;
				}
				if (abs(py) <= T1)
				{
					f1[(x1 + T1) * t1 + py + T1] -= 1;
				}
			}

			x1 = newD[6 * i + j];
			py = newD[6 * i + j + 1];
			if (abs(x1) <= T1)
			{
				if (abs(py) <= 255)
				{
					p1[(x1 + T1)] += 1;
				}
				if (abs(py) <= T1)
				{
					f1[(x1 + T1) * t1 + py + T1] += 1;
				}
			}
		}
	}
}

/** Verify, if the pixel falls in the model
@param x x coordinate
@param y y coordinate
@param v new value of the pixel
@return true if pixel is within model*/
bool TSpam12::checkPixel(int x, int y, int v)
{
	/*move to the padded  coordinates */
	y = y + 3;
	x = x + 3;

	int oldValue = im[y * pWidth + x];
	im[y * pWidth + x] = v;

	/*first, we calculate differences of the pixel */
	for (int i = 0; i < 8; i++)
	{
		/*check first difference */
		int dy = d[2 * i + 0];
		int dx = d[2 * i + 1];
		int d = im[y * pWidth + x] - im[(y + dy) * pWidth + x + dx];
		if ((abs(d) > T2) && (abs(d) < 256))
		{
			im[y * pWidth + x] = oldValue;
			return false;
		}
		/*check second difference */
		d = im[(y + dy) * pWidth + x + dx] - im[(y + 2 * dy) * pWidth + x + 2 * dx];
		if ((abs(d) > T2) && (abs(d) < 256))
		{
			im[y * pWidth + x] = oldValue;
			return false;
		}
		/*check third difference */
		d = im[(y + 2 * dy) * pWidth + x + 2 * dx] - im[(y + 3 * dy) * pWidth + x + 3 * dx];
		if ((abs(d) > T2) && (abs(d) < 256))
		{
			im[y * pWidth + x] = oldValue;
			return false;
		}
	}
	im[y * pWidth + x] = oldValue;
	return true;
}

/** return the 2nd order SPAM features */
shared_array<double> TSpam12::getF2()
{
	/* to finalize the calculation of the features, we need to normalize matrix */
	int t2 = 2 * T2 + 1;
	int oneMat2 = t2 * t2 * t2;
	shared_array<double> fVec(new double[2 * oneMat2 + 1]);
	for (int i = 0; i < 2 * oneMat2 + 1; i++)
	{
		fVec[i] = 0.0;
	}
	int key = 0;
	for (int i = 0; i < 8; i++)
	{
		boost::shared_array<double> f2 = F2[i];
		boost::shared_array<double> p2 = P2[i];
		key = i / 4;
		for (int x1 = 0; x1 < t2; x1++)
		{
			for (int x2 = 0; x2 < t2; x2++)
			{
				for (int y = 0; y < t2; y++)
				{
					if (p2[x1 * t2 + x2] > 1e-8)
					{
						fVec[key * oneMat2 + x1 * t2 * t2 + x2 * t2 + y] += 0.25 * ((double)f2[x1 * t2 * t2 + x2 * t2 + y]) / p2[x1 * t2 + x2];
					}
				}
			}
		}
	}
	fVec[2 * oneMat2] = width * height;
	return fVec;
}

/** return the 1st order SPAM features */
shared_array<double> TSpam12::getF1()
{
	/* to finalize the calculation of the features, we need to normalize matrix */
	int t1 = 2 * T1 + 1;
	int oneMat1 = t1 * t1;
	shared_array<double> fVec(new double[2 * oneMat1 + 1]);
	for (int i = 0; i < 2 * oneMat1 + 1; i++)
	{
		fVec[i] = 0.0;
	}
	int key = 0;
	for (int i = 0; i < 8; i++)
	{
		boost::shared_array<double> f1 = F1[i];
		boost::shared_array<double> p1 = P1[i];
		key = i / 4;
		/*dirty way to make averaging*/
		for (int x1 = 0; x1 < t1; x1++)
		{
			for (int y = 0; y < t1; y++)
			{
				if (p1[x1] > 1e-8)
				{
					fVec[key * oneMat1 + x1 * t1 + y] += 0.25 * ((double)f1[x1 * t1 + y]) / p1[x1];
				}
			}
		}
	}
	fVec[2 * oneMat1] = width * height;
	return fVec;
}

/** return the 1st order SPAM features */
shared_array<double> TSpam12::getF()
{
	/* to finalize the calculation of the features, we need to normalize matrix */
	int t1 = 2 * T1 + 1;
	int oneMat1 = t1 * t1;
	int t2 = 2 * T2 + 1;
	int oneMat2 = t2 * t2 * t2;
	shared_array<double> fVec(new double[2 * oneMat1 + 2 * oneMat2 + 1]);
	shared_array<double> fVec1 = getF1();
	shared_array<double> fVec2 = getF2();
	for (int i = 0; i < 2 * oneMat1; i++)
	{
		fVec[i] = fVec1[i];
	}
	for (int i = 0; i < 2 * oneMat2; i++)
	{
		fVec[2 * oneMat1 + i] = fVec2[i];
	}

	fVec[2 * oneMat1 + 2 * oneMat2] = width * height;
	return fVec;
}

TSpam12::~TSpam12()
{
}

void TSpam12::write(string fileName)
{
	/* create file */
	FILE *fp = fopen(fileName.c_str(), "wb");
	if (!fp)
		throw TSpamEx("File " + fileName + " could not be opened for writing");

	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		throw TSpamEx("[write_png_file] png_create_write_struct failed");

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		throw TSpamEx("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		throw TSpamEx("[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		throw TSpamEx("[write_png_file] Error during writing header");


	// from http://www.libpng.org/pub/png/libpng-manual.txt
	// info_ptr->channels = 1;
	// You can also set up a pointer to a user structure for use by your
	// callback function, and you can inform libpng that your transform
	// function will change the number of channels or bit depth with the
	// function.

	// bit_depth      - holds the bit depth of one of the
    //                  image channels.  (valid values are
    //                  1, 2, 4, 8, 16 and depend also on
    //                  the color_type.  See also
    //                  significant bits (sBIT) below).

    // color_type     - describes which color/alpha channels
    //                      are present.
    //                  PNG_COLOR_TYPE_GRAY
    //                     (bit depths 1, 2, 4, 8, 16)
	png_set_user_transform_info(png_ptr, info_ptr, 1, 1);



	// png_get_color_type(png_ptr, info_ptr)
	png_set_IHDR(png_ptr, info_ptr, width, height,
				 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/*copy relevant part of the image */
	/* Allocate the memory to read the rows of the image */
	png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++)
		row_pointers[y] = (png_byte *)malloc(sizeof(png_bytep) * width);

	for (int i = 0; i < height; i++)
	{
		png_bytep row = row_pointers[i];
		for (int j = 0; j < width; j++)
		{
			row[j] = im[(i + 3) * pWidth + j + 3];
		}
	}

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		throw TSpamEx("[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		throw TSpamEx("[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (int y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
}
