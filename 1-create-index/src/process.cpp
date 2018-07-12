/* ============================================================================================
  process_image.cpp          

  takes one image and computes the surf features
 ============================================================================================ */

#include <iostream>
#include <dirent.h>
#include <iomanip>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace std;
using namespace cv;

void read_surfparams (string param, string* odir, int *minh, int *octaves, int *layers, int *sizemin, double *responsemin);
void filter_keypoints (vector <KeyPoint> &keypoints, int sizemin, double responsemin);

int main(int argc, char **argv)
{

  string ifile = argv[1];
  string odir = "";
  string param = "../settings.yml";
  int minh = 0, octaves = 0, layers = 0, sizemin = 0;
  double responsemin = 0;
  string name, nameful;
  string extension = ".yml";

  read_surfparams (param, &odir, &minh, &octaves, &layers, &sizemin, &responsemin);


  //Create all structures needed for SURF key point detection and feature extraction
  SurfFeatureDetector detector (minh, octaves, layers);
  Ptr<DescriptorExtractor> extractor = new SurfDescriptorExtractor();
  vector <KeyPoint> keypoints;
  Mat descriptors;


  //read in image file and generate output file name
  Mat image = imread (ifile);

  size_t found = ifile.find_last_of("/");
  name = ifile.substr(found + 1, string::npos);
  name.erase(name.find_last_of("."), string::npos);
  nameful = odir + name + extension;

   //SURF detection and then filter
  detector.detect (image, keypoints);
  filter_keypoints (keypoints, sizemin, responsemin);
  cout << "ifile: " << ifile << " numkeypoints: " << keypoints.size() << endl;

  //write into output file
  FileStorage fs (nameful, FileStorage::WRITE);
  fs << "numkeypoints" << int(keypoints.size());
  fs << "minh" << minh;
  fs << "octaves" << octaves;
  fs << "layers" << layers;
  fs << "sizemin" << sizemin;
  fs << "responsemin" <<  responsemin;
  fs << "keypoints" << keypoints;
  if (keypoints.size() > 0)
  {
    extractor->compute (image, keypoints, descriptors);
    fs << "descriptors" << descriptors;
  }
  fs.release();
 
  return 0;
}  


/* ===============================================================================================
   Procedure to read parameters for SURF from the parameter file
   =============================================================================================== */
void read_surfparams(string param, string*odir, int *minHessian, int *octaves, int *octaveLayers, int *SizeMin, double *RespMin)
{
  ifstream inFile;
  inFile.open(param.c_str());
  string record;
  stringstream ss;

	while ( !inFile.eof () ) {    
		getline(inFile,record);
		if (record.find("keypoints") != std::string::npos) {
			ss<<record.substr(record.find_last_of(":") + 1);
			ss>> *odir;
			ss.str("");
			ss.clear();
		}
		if (record.find("minHessian") != std::string::npos) {
			ss<<record.substr(record.find_last_of(":") + 1);
			ss>> *minHessian;
			ss.str("");
			ss.clear();
		}
		if (record.find("octaves") != std::string::npos) {
			ss<<record.substr(record.find_last_of(":") + 1);
			ss>> *octaves;
			ss.str("");
			ss.clear();
		}
		if (record.find("octaveLayers") != std::string::npos) {
			ss<<record.substr(record.find_last_of(":") + 1);
			ss>> *octaveLayers;
			ss.str("");
			ss.clear();
		}
		if (record.find("min Size") != std::string::npos) {
			ss<<record.substr(record.find_last_of(":") + 1);
			ss>> *SizeMin;
			ss.str("");
			ss.clear();
		}
		if (record.find("min Resp") != std::string::npos) {
			ss<<record.substr(record.find_last_of(":") + 1);
			ss>> *RespMin;
			ss.str("");
			ss.clear();
		}
	}
}


/* ===============================================================================================
   Procedure to filter the keypoints from the keypoint vector by minimum size and response
   =============================================================================================== */
void filter_keypoints (vector <KeyPoint> &keypoints, int sizemin, double responsemin)
{
  vector <KeyPoint> temp;
  int npoints = keypoints.size();
  int size;
  double response;

  //filter based on size and response size
  for (int i = 0; i < npoints; i++)
  {
    size = keypoints[i].size;
    response = keypoints[i].response;
    if (size > sizemin && response > responsemin)
      temp.push_back(keypoints[i]);
  }

  keypoints.clear();
  keypoints = temp;

  return;
}



  
