#include "tspam12.h"
#include "tspamex.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <memory>
#include <boost/shared_array.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace boost;

void printHelp(){
	cout <<"	extracts 1st and 2nd order  spam features from all greyscale png images in the directory inputDir."<<endl;
	cout <<"	for further details read:"<<endl;
	cout <<"	Steganalysis by Subtractive Pixel Adjacency Matrix, T. Pevny, P. Bas, J. Fridrich, ACM Workshop of Multimedia and Digital Security, 2009, Princeton, NJ, USA"<<endl<<endl;
	cout <<"usage: spam inputDir [-T1] [-T2] [--oFile1st] [--oFile2nd]"<<endl<<endl;
	cout <<"	-T1 --- specify upper bound on absolute value of differences on 1st order SPAM features [default=4]"<<endl;
	cout <<"	-T2 --- specify upper bound on absolute value of differences on 2nd order SPAM features [default=3]"<<endl<<endl;
	cout <<"	--oFile1st fileName --- specify the file to output 1st order SPAM features"<<endl;
	cout <<"	--oFile2nd fileName --- specify the file to output 2nd order SPAM features"<<endl;
	cout <<"	--singleFile fileName --- extracts the features from a single file"<<endl;
}

void saveF(string fileName,shared_array<double> F,int length,string imgName){
	try {
		ofstream oFile(fileName.c_str(),ios_base::app);
		oFile<<scientific;
		oFile.precision(10);
		/*export 1st order features --- averaged version*/
		for (int i=0;i<length;i++){
			oFile<<F[i]<<" ";
		}
		oFile <<imgName<<endl;
	}
  catch(std::exception& ex){
		cerr <<"error when saving feature file"<<endl;
		cerr <<ex.what()<<endl;
	};
}


int main(int argc, char** argv){
	if (argc<3){
		printHelp();
		return 0;
		}

	/*start with parsing arguments*/
	int T1=4;
	int T2=3;
	string iDir="";
	string iFile="";
	string oFile1="",oFile2="";
	int i=1;
	while (i<argc){
		string arg=string(argv[i]);
		if (arg=="-T1"){
			i++;
			if (i<argc){
				T1=atoi(argv[i]);
				i++;
				continue;
				}
			else {
				printHelp();
				return 1;
				}
			}

		if (arg=="-T2"){
			i++;
			if (i<argc){
				T2=atoi(argv[i]);
				i++;
				continue;
				}
			else {
				printHelp();
				return 1;
				}
			}

		if (arg=="--singleFile"){
			i++;
			if (i<argc){
				iFile=string(argv[i]);
				i++;
				continue;
				}
			else {
				printHelp();
				return 1;
				}
			}

		if (arg=="--oFile1st"){
			i++;
			if (i<argc){
				oFile1=string(argv[i]);
				i++;
				continue;
				}
			else {
				printHelp();
				return 1;
				}
			}

		if (arg=="--oFile2nd"){
			i++;
			if (i<argc){
				oFile2=string(argv[i]);
				i++;
				continue;
				}
			else {
				printHelp();
				return 1;
				}
			}

		if (iDir==""){
			iDir=string(argv[i]);
			i++;
			continue;
			}

		cout <<"unknown option "<<arg<<endl;
		printHelp();
		return 1;
		}

	if (iFile!=""){
		try {
				cout <<"processing "<<iFile<<endl;
				/*extracts features from the image */
				TSpam12 spam(iFile,T1,T2);
				if (oFile1!=""){
					saveF(oFile1,spam.getF1(),spam.f1Num(),iFile);
					}
				if (oFile2!=""){
					saveF(oFile2,spam.getF2(),spam.f2Num(),iFile);
					}

			} catch (TSpamEx &ex){
			cerr<<ex.what()<<endl;
		}
		return 1;
	}

	if (iDir!=""){
		/*define regular expression matcher */
		boost::regex e(".png");

		boost::filesystem::path dir(iDir.c_str());
		/* iterate through directory*/
		boost::filesystem::directory_iterator end_iter;
		for (boost::filesystem::directory_iterator file(dir);file != end_iter;++file ){

			/*check if file is image */
			// string fileName=file->leaf();
			boost::filesystem::path p(file->path());
			string fileName = p.filename().string();
			// https://stackoverflow.com/questions/4430780/how-can-i-extract-the-file-name-and-extension-from-a-path-in-c
			boost::match_results<string::const_iterator> what;
			if (0 == boost::regex_search(fileName, what, e, boost::match_default)) {
				continue;
				};

	// 		unsigned int runtime=(unsigned int)time((time_t *)NULL);
			try {
				
				ostringstream imagePath;
				imagePath << dir.string() <<"/"<< fileName; // https://www.boost.org/doc/libs/1_45_0/libs/filesystem/v3/doc/deprecated.html
				cout <<"processing "<<imagePath.str()<<endl;
				
				/* extracts features from the image */
				TSpam12 spam(imagePath.str(),T1,T2);
				if (oFile1!=""){
					saveF(oFile1,spam.getF1(),spam.f1Num(),fileName);
					}
				if (oFile2!=""){
					saveF(oFile2,spam.getF2(),spam.f2Num(),fileName);
					}

			} catch (TSpamEx &ex){
			cerr<<ex.what()<<endl;
			}
		}
	return 1;
	}

	printHelp();
	return 1;
};
