#ifndef TSPAM12_H_
#define TSPAM12_H_

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <exception>
#include <string>
#include <vector>
#include "auxiliary.h"
#include "tspamex.h"
#include <png.h>
#include <stdint.h>

/** Class implementing calculation of 2nd order SPAM features and their incremental update */
class TSpam12{
public:
	TSpam12(std::string fileName,int T1=4,int T2=3);

	/** set the pixel of the image at location x,y to value v.
	Matrices for the calculation of the features are appropriately recalculated.
	@param x x coordinate
	@param y y coordinate
	@param v new value of the pixel */
	void setPixel(int x,int y,int v);

	/** return the pixel of the image at location x,y
	@param x x coordinate
	@param y y coordinate
	@return value of the pixel  */
	int getPixel(int x,int y){
		return im[(y+3)*pWidth+x+3];
		}

	/** return the 2nd order SPAM features */
	boost::shared_array<double> getF2();

	/** return the 1st order SPAM features */
	boost::shared_array<double> getF1();

	/** return the 1st and 2nd order SPAM features */
	boost::shared_array<double> getF();

	/** @return width of the  image */
	virtual int getWidth(){
		return width;
		}

	/** @return height of the  image */
	int getHeight(){
		return height;
		}

	/** @return number of features in the model */
	int fNum(){
			int t2=2*T2+1;
			int t1=2*T1+1;
			return 2*t2*t2*t2+2*t1*t1+1;
		}

	/** @return number of features in the model */
	int f1Num(){
			int t1=2*T1+1;
			return 2*t1*t1+1;
		}

	/** @return number of features in the model */
	int f2Num(){
			int t2=2*T2+1;
			return 2*t2*t2*t2+1;
		}


	void write(std::string fileName);

	bool checkPixel(int x,int y,int v);

	~TSpam12();

private:
	/** Loads image to the array im. It fils the width, heigth, pWidth, pHeigth approariately.
	@param fileName path to the image to load */
	void loadImage(std::string fileName);

	/** Substracts two submatrices of im (padded) (M1 - M2). Submatrices have to 
	have same dimension rows x cols. 
	@param stIndex1 starting index of submatrix M1
	@param stIndex2 starting index of submatrix M2
	@param rows number of rows of submatrix
	@param cols number of columns of submatrix 
	@return matrix rows-cols containing M1-M2*/
	boost::shared_array<int> subIm(int stIndex1,int stIndex2,int rows,int cols);

	/** Width of the original image */
	// int width;
	png_uint_32 width;
	/** Height of the original image */
	// int height;
	png_uint_32 height;

	/** Width of the padded image */
	int pWidth;
	/** Height of the padded image */
	int pHeight;
	
	/** range of differences that are considered in the 2nd order SPAM features */
	int T2;
	/** vector of cooccurences x1,x2,y for 2nd order SPAM features*/
	std::vector<boost::shared_array<double> > F2;
	/** vector of cooccurences x1,x2  for 2nd order SPAM features*/
	std::vector<boost::shared_array<double> > P2;

	/** range of differences that are considered in the SPAM features */
	int T1;
	/** vector of cooccurences x1,x2,y*/
	std::vector<boost::shared_array<double> > F1;
	/** vector of cooccurences x1,x2*/
	std::vector<boost::shared_array<double> > P1;

	/** image plane padded by 3 pixels from each side */
	boost::shared_array<int> im;

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
	void cooc2nd(int x1,int x2, int y,int cols, int rows,boost::shared_array<int> diff,int dColls,int key);

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
	void cooc(int x1,int y,int cols, int rows,boost::shared_array<int> diff,int dColls,int key);

};

#endif
