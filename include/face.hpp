#pragma once
#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>

#undef DeleteFile
#undef MoveFile
#else
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif // WIN32
#include "jfda.hpp"
#include <vector>
#include <assert.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Timer.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <Python.h>
#include <stdio.h>
#include <ConfigRead.h>
#include "PlayM4.h"
#include "HCNetSDK.h"

using namespace std;
using namespace cv;
using namespace jfda;
vector<string> ListDir(const string &dpath, const vector<string> &exts);
int calArea(Rect rect1, Rect rect2);
string GetFileExtension(const string &path);
bool IsExists(const string &path);
static bool StringCompareCaseInsensitive(const string &s1, const string &s2);
double calDistance(int x1, int y1, int x2, int y2);
vector<Point2f> markarea(vector<int> &txcenter,vector<int> &tycenter);
bool booldetection(int facenum);
int decface(vector<int> &txcenter, vector<int> &tycenter, vector<FaceInfo> &tface);
void createvideo(string picpath,string classid,int index,string videopath);
tm getlocaltime();
void concatpoints(vector<FaceInfo> &rface, vector<FaceInfo> &tface, vector<int> &rxcenter, vector<int> &rycenter, vector<int> &txcenter, vector<int> &tycenter);
bool Checktime(int starthour,int startminute,int reststarthour,int reststartminute,int restendhour,int restendminute,int endhour,int endminute,PLAYM4_SYSTEM_TIME pstSystemTime);
void concatface(vector<int> &txcenter, vector<int> &tycenter, vector<FaceInfo> &tface);
string timestr(int year,int month,int day,int hour,int minute,int second);
int average(vector<int> risenumber);
string IntString(int x);
int randomrange(int a, int b);
vector<string> yieldpath(string classid);
Point JointPoint(Point pCur, Point pLT, Point pLB, Point pRB, Point pRT);
void delete_path(string path);
