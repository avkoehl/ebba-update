/* ===============================================================================================
   archv .cpp  

   takes one image and finds matches
   =============================================================================================== */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <fstream>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include <algorithm>  // for sort algorithm
using namespace cv;
using namespace std;


void read_surfparams(string param, string *imgdir, string* keyptdir, string* matchdir, int *minh, int *octaves, int *layers, int *sizemin, double *responsemin, double*scale, double* ratio);
void filter_keypoints (vector<KeyPoint>& keypoints, int sizemin, double responsemin);
int GetFileList(string directory, vector<string> &files);
int ratioTest(vector<vector<cv::DMatch> > &matches, double ratio);
void symmetryTest(const vector<vector<DMatch> >& matches1,const vector<vector<DMatch> >& matches2,vector<DMatch>& symMatches);
Mat ransacTest(const vector<cv::DMatch>& matches,const vector<cv::KeyPoint>& keypoints1, const vector<cv::KeyPoint>& keypoints2, vector<cv::DMatch>& outMatches);
vector<int> ordered(vector<double> const & values, int nval);


int main(int argc, char** argv)
{

    string img = argv[1];
    string param = "../settings.yml";
    string imgfile, imgdir, keyptdir, matchdir;
    int minh = 0, octaves = 0, layers = 0, sizemin = 0;
    double responsemin = 0, scale = 0, ratio = 0;
    imgfile = img;

    read_surfparams (param, &imgdir, &keyptdir, &matchdir, &minh, &octaves, &layers, &sizemin, &responsemin, &scale, &ratio);

    vector<KeyPoint> keypoints1;                                         // Vector of keypoints
    vector<KeyPoint> keypoints2;                                         // Vector of keypoints
    Mat descriptors1, descriptors2;
    BFMatcher matcher;
    vector < vector<DMatch> > matches1;
    vector < vector<DMatch> > matches2;
    vector <DMatch> sym_matches;
    vector <DMatch> matches;

    Mat img1;
    img1 = imread(imgfile);

    string name = "";
    string kfname = "";
    size_t found = img.find_last_of("/");
    name = img.substr(found + 1, string::npos);
    name.erase(name.find_last_of("."), string::npos);
    kfname = keyptdir + name + ".yml";

    FileStorage fs(kfname, FileStorage::READ);
    FileNode kptFileNode = fs["keypoints"];
    read( kptFileNode, keypoints1 );
    if(keypoints1.size()>0) 
            fs["descriptors"] >> descriptors1;

    /* ===============================================================================================
       Go to directory containing images, check it exists, get file list, select those files
       that contain an image (.jpg extension).
       =============================================================================================== */
    struct stat sb;

    const char * dirName;
    dirName = imgdir.c_str ();
    if (stat(dirName, &sb) != 0 || !S_ISDIR(sb.st_mode))
    {
        cout << dirName << " does not exist, or is not a directory; try again"<<endl;
        return -1;
    }

    vector<string> allfiles = vector<string>();                 // vector containing names of all files

    int ierr = GetFileList(imgdir, allfiles);
    if (ierr != 0) 
    {
        cout << " Problem while trying to read in list of histograms; check the directory!" <<endl;
        return -1;
    }

    vector<string> files;
    string filename;
    int counter = 0;

    for(int i = 0; i < allfiles.size(); i++)
    {
        filename = allfiles[i];
        if(filename.substr(filename.find_last_of(".") + 1) == "jpg")
        {
            files.push_back(filename);
            counter++;
        }
    }

    /* ===============================================================================================
       Now loop over all image files:
       =============================================================================================== */
    vector<double> distval(files.size());
    vector<int>    indices(files.size());

    string infofile;
    int pos;

    counter = 0;
    int ndist = 0;

    for(int i = 0; i < files.size(); i++)
    {
        /*      =========================================================================================
                Generate filenames
                ========================================================================================== */

        infofile = files[i];
        pos = 0;
        pos = infofile.find("jpg",pos);
        infofile.replace(pos,3,"yml");
        filename = keyptdir;
        if(*filename.rbegin() != '/') filename.append("/");
        filename.append(infofile);

        /*      =========================================================================================
                Read keypoints from file
                ========================================================================================== */
        keypoints2.clear();

        FileStorage fs(filename, FileStorage::READ);
        FileNode kptFileNode = fs["keypoints"];
        read( kptFileNode, keypoints2 );

        /*      =========================================================================================
                Process image2
                ========================================================================================== */
        if(keypoints1.size()>0 && keypoints2.size() > 0 ) 
        {

            fs["descriptors"] >> descriptors2;

            /*  	=========================================================================================
                    Find matches based on descriptors: 
                    - first from img1 to img2 (with 2 NN), then from img2 to img1
                    - filter based on ratio test
                    - filter for symmetry
                    - filter by RANSAC
                    ======================================================================================== */

            matches1.clear();
            matches2.clear();
            matcher.knnMatch(descriptors1,descriptors2,matches1,2);
            matcher.knnMatch(descriptors2,descriptors1,matches2,2);

            int removed= ratioTest(matches1,ratio);
            removed= ratioTest(matches2,ratio);

            sym_matches.clear();
            matches.clear();

            symmetryTest(matches1,matches2,sym_matches);

            Mat fundamental= ransacTest(sym_matches,keypoints1,keypoints2, matches);
            distval[counter] = matches.size();
        }

        else
            distval[counter] = 0;

        fs.release();
        counter++;
    }
    ndist = counter;

    /* ===============================================================================================
       Sort array of distances: get indices of sorted values
       =============================================================================================== */
    indices = ordered(distval,ndist);

    /* ===============================================================================================
       Write out ordered list of images, with number of matches
       =============================================================================================== */
    string txtfile = matchdir + name + ".txt";

    ofstream file(txtfile.c_str());
    int count = 0;
    int idx;
    double dist;

    file << imgfile;

    for(int i=0; i < ndist; i++)
    {
        idx = indices[i];
        dist = distval[idx];
        if (dist > 1)
        {
            file << "," << files[idx] << " " <<  dist;
        }
    }
    file.close();

    cout << "processed file: " << name << endl;

    return 0;
}

/* ===============================================================================================
   Procedure to read parameters for SURF
   =============================================================================================== */
void read_surfparams(string param, string* imgdir, string* keyptdir, string*matchdir, int *minHessian, int *octaves, int *octaveLayers, int *SizeMin, double *RespMin, double* scale, double*ratio)
{
    ifstream inFile;
    inFile.open(param.c_str());
    string record;
    stringstream ss;

    while ( !inFile.eof () ) {    
        getline(inFile,record);
        if (record.find("images") != std::string::npos) {
            ss<<record.substr(record.find_last_of(":") + 1);
            ss>> *imgdir;
            ss.str("");
            ss.clear();
        }
        if (record.find("keypoints") != std::string::npos) {
            ss<<record.substr(record.find_last_of(":") + 1);
            ss>> *keyptdir;
            ss.str("");
            ss.clear();
        }
        if (record.find("matches") != std::string::npos) {
            ss<<record.substr(record.find_last_of(":") + 1);
            ss>> *matchdir;
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
        if (record.find("scale") != std::string::npos) {
            ss<<record.substr(record.find_last_of(":") + 1);
            ss>> *scale;
            ss.str("");
            ss.clear();
        }
        if (record.find("ratio") != std::string::npos) {
            ss<<record.substr(record.find_last_of(":") + 1);
            ss>> *ratio;
            ss.str("");
            ss.clear();
        }
    }
}

/* ===============================================================================================
   Procedure to filter keypoints based on size and response
   =============================================================================================== */
void filter_keypoints (vector<KeyPoint>& keypoints, int sizemin, double responsemin)
{
    vector<KeyPoint> temp;
    int npoints=keypoints.size();
    int size;
    double response;

    //filter based on size and reponse size
    for(int i = 0; i < npoints; i++)
    {
        size = keypoints[i].size;
        response = keypoints[i].response;
        if(size > sizemin && response > responsemin)
            temp.push_back(keypoints[i]);
    }

    keypoints.clear();
    keypoints = temp;

    return;
}


/* ===============================================================================================
   Procedure to sort an array (in descending order) by providing the indices of the sorted positions
   (does not change the array)
   Use simple bubble sort! Could be easily improved....
   =============================================================================================== */

vector<int> ordered(vector<double> const & values, int nval)
{
    vector<int> indices(values.size());
    for(int i =0; i < nval ; i++)
    {
        indices[i] = i;
    }
    int idx;
    for(int i = 0; i < nval-1; i++)
    {
        for(int j = i+1; j< nval; j++)
        {
            if(values[indices[i]] < values[indices[j]])
            {
                idx = indices[i];
                indices[i]=indices[j];
                indices[j]=idx;
            }
        }
    }

    return indices;
}

/* ===============================================================================================
   This function perform a ratio test: 
   - Clear matches for which NN ratio is > than threshold
   - return the number of removed points
   =============================================================================================== */

int ratioTest(vector<vector<cv::DMatch> > &matches, double ratio) 
{
    int removed=0;
    for (vector<vector<cv::DMatch> >::iterator
            matchIterator= matches.begin();
            matchIterator!= matches.end(); ++matchIterator) 
    {

        if (matchIterator->size() > 1)
        {
            if ((*matchIterator)[0].distance/ (*matchIterator)[1].distance > ratio)
            {
                matchIterator->clear(); // remove match
                removed++;
            }
        }//if: NN > 1 
        else 
        {
            matchIterator->clear(); // remove match
            removed++;
        }//else: NN < 2
    }
    return removed;
}

/* ===============================================================================================
   This function perform a symmetry test:
   matches from 1 to 2 should match with matches from 2 to 1
   =============================================================================================== */
void symmetryTest(const vector<vector<DMatch> >& matches1,const vector<vector<DMatch> >& matches2, vector<DMatch>& symMatches) 
{
    for (vector<vector<cv::DMatch> >:: const_iterator matchIterator1= matches1.begin();
            matchIterator1!= matches1.end(); ++matchIterator1) 
    {
        if (matchIterator1->size() < 2) continue;
        for (vector<vector<cv::DMatch> >:: const_iterator matchIterator2= matches2.begin();
                matchIterator2!= matches2.end(); ++matchIterator2) 
        {

            // ignore deleted matches
            if (matchIterator2->size() < 2) continue;

            /*     			=========================================================================
                            Symmetry test
                            ========================================================================= */

            if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx &&
                    (*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx) 
            {

                symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,
                            (*matchIterator1)[0].trainIdx,(*matchIterator1)[0].distance));

                break; // next match in image 1 -> image 2

            }
        }
    }
}

/* ===============================================================================================
   Identify good matches using RANSAC; return fundamental matrix
   =============================================================================================== */

Mat ransacTest(const vector<cv::DMatch>& matches,const vector<cv::KeyPoint>& keypoints1, const vector<cv::KeyPoint>& keypoints2, vector<cv::DMatch>& outMatches) 
{

    /*     	=========================================================================================
            Convert keypoints1 and keypoints2 into Point2f
            ========================================================================================= */

    vector<cv::Point2f> points1, points2;

    Mat fundemental;

    for (vector<cv::DMatch>::const_iterator it= matches.begin();it!= matches.end(); ++it) 
    {

        double x= keypoints1[it->queryIdx].pt.x;
        double y= keypoints1[it->queryIdx].pt.y;
        points1.push_back(Point2f(x,y));

        x= keypoints2[it->trainIdx].pt.x;
        y= keypoints2[it->trainIdx].pt.y;
        points2.push_back(cv::Point2f(x,y));
    }

    /*     	=========================================================================================
            Compute fundamental matrix using RANSAC
            ========================================================================================= */

    vector<uchar> inliers(points1.size(),0);

    double confidence = 0.99;
    double distance = 3.0;
    int refineF = 1;

    if (points1.size()>0&&points2.size()>0)
    {

        Mat fundemental= cv::findFundamentalMat(
                cv::Mat(points1),cv::Mat(points2), // matching points
                inliers,       // match status (inlier or outlier)
                CV_FM_RANSAC, // RANSAC method
                distance,      // distance to epipolar line
                confidence); // confidence probability

        // extract the surviving (inliers) matches

        vector<uchar>::const_iterator itIn= inliers.begin();
        vector<cv::DMatch>::const_iterator itM= matches.begin();

        for ( ;itIn!= inliers.end(); ++itIn, ++itM) 
        {
            if (*itIn) { // it is a valid match
                outMatches.push_back(*itM);
            }
        }

        if (refineF) {

            // The F matrix will be recomputed with all accepted matches
            // Convert keypoints into Point2f for final F computation

            points1.clear();
            points2.clear();

            for (std::vector<cv::DMatch>::const_iterator it= outMatches.begin();it!= outMatches.end(); ++it) 
            {
                double x= keypoints1[it->queryIdx].pt.x;
                double y= keypoints1[it->queryIdx].pt.y;
                points1.push_back(cv::Point2f(x,y));

                x= keypoints2[it->trainIdx].pt.x;
                y= keypoints2[it->trainIdx].pt.y;
                points2.push_back(cv::Point2f(x,y));
            }

            // Compute 8-point F from all accepted matches

            if (points1.size()>0&&points2.size()>0){

                fundemental= cv::findFundamentalMat(cv::Mat(points1),cv::Mat(points2), // matches
                        CV_FM_8POINT); // 8-point method

            }
        }
    }

    return fundemental;
}

int GetFileList(string directory, vector<string> &files)
{
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(directory.c_str())) == NULL) {
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return 0;
}
