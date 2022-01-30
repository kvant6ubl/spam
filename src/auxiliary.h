#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "tspamex.h"


/** print vector to stdout
	@param dimension here will be returned the dimension of loaded vector*/
template<class T>
void printVector(boost::shared_array<T> vec,int dimension){
	for (int i=0;i<dimension;i++){
		std::cout <<vec[i]<<" ";
		}
}

/** Loads vector from the file 
	@param fileName name of the file
	@param dimension here will be returned the dimension of loaded vector*/
template<class T>
void saveVector(boost::shared_array<T> vec,std::string fileName,int &dimension){
	std::ofstream oFile(fileName.c_str());
	for (int i=0;i<dimension;i++){
		oFile <<vec[i]<<" ";
		}
}

/** Loads vector from the file 
	@param fileName name of the file
	@param dimension here will be returned the dimension of loaded vector*/
template<class T>
boost::shared_array<T> loadVector(std::string fileName,int &dimension){
	std::ifstream iFile(fileName.c_str());
	if (!iFile){
		throw TSpamEx(std::string("Cannot open file ")+fileName);
		}
	std::vector<T> temp;
	T value;
	iFile >>value;
	while (iFile){
		temp.push_back(value);
		iFile >>value;
		}
	boost::shared_array<T> vec(new double[temp.size()]);
	for (unsigned int i=0;i<temp.size();i++){
		vec[i]=temp[i];
		}

	dimension=temp.size();
	return vec;
}

/** Saves matrix to file
@param fileName name of the file where the matrix will be saved
@param m matrix to be stored
@param rows number of rows of the matrix m
@param cols number of columns of the matrix m*/
template<class T>
void saveMatrix(std::string fileName,boost::shared_array<T> m,int rows,int cols){
	std::ofstream oFile(fileName.c_str());
	for (int i=0;i<rows;i++){
		for (int j=0;j<cols;j++){
			oFile<<m[i*cols+j]<<" ";
		}
		oFile <<std::endl;
	}
}

/** generate random message with given length. The random message is vector of int with 0 or 1
@param length length of the random message
@return vector with random message */
boost::shared_array<int> randomMessage(int length);

/** Create random walk (permutation). The random number generator is not initialized, do that before.
	@param length - length of the random walk 
	@return random walk*/
boost::shared_array<int> getRandomWalk(int length);


#endif
