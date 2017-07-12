#include "face.hpp"
#include "config.hpp"
#include <ctime>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <cctype>
#pragma comment(lib,"python27.lib")

bool booldetection(int facenum)  // 少于10人中断检测
{
	if (facenum < 10)
		return false;
	else
		return true;
}
int randomrange(int a, int b){   //生成{a-b}之间的随机数
	
	int range = b - a + 1;
	int random = a + rand()%range;
	return random;
}
string IntString(int x){    //整形转换为字符串
	stringstream ss;
	string rise;
	ss << x;
	ss>>rise;
	return rise;
}

string timestr(int year,int month,int day,int hour,int minute,int second){ //将所有整形时间数据生成日期形式
	stringstream s1, s2,s3,s4,s5,s6;
	string syear,smonth,sday,shour, smin,sec;
	s1 << year;s2 << month;s3 << day;s4 << hour;s5 << minute;s6 << second;
	s1 >> syear;s2 >> smonth;s3 >> sday;s4 >> shour;s5 >> smin;s6 >> sec;
	string str = syear+"-"+smonth+"-"+sday+" "+shour+":"+smin+":"+sec;
	return str;
}

vector<Point2f> markarea(vector<int> &txcenter,vector<int> &tycenter)
{
	vector<Point2f> corner(4);
	vector<int> xcenter = txcenter;
	vector<int> ycenter = tycenter;
	sort(xcenter.begin(),xcenter.end());
	sort(ycenter.begin(),ycenter.end());
	vector<int>::iterator maxbottomx = std::max_element(std::begin(txcenter), std::end(txcenter));
	vector<int>::iterator minbottomx = std::min_element(std::begin(txcenter), std::end(txcenter));
	int maxbottomindex = std::distance(std::begin(txcenter),maxbottomx);
	int minbottomindex = std::distance(std::begin(txcenter),minbottomx);
	if(tycenter[minbottomindex]>tycenter[maxbottomindex]){
		corner[2] = Point2f(*minbottomx,tycenter[minbottomindex]);
		corner[3] = Point2f(*maxbottomx,tycenter[minbottomindex]);
	}else{
		corner[2] = Point2f(*minbottomx,tycenter[maxbottomindex]);
		corner[3] = Point2f(*maxbottomx,tycenter[maxbottomindex]);
	}
	int topy = ycenter[0];
	int topx;

	for(int i = 0;i<tycenter.size();i++){
		if(topy==tycenter[i]){
			topx = txcenter[i];
		}
	}
	int xtopmin = 0;
	int xtopmax = 0;
	int xtempmin = 0;
	int xtempmax = 0;

	for(int i = 0;i<tycenter.size();i++){
		if((tycenter[i]-topy)<50){
			if((topx-txcenter[i])<xtempmax){
				xtempmax = topx-txcenter[i];
				xtopmax = txcenter[i];
			}
			if((txcenter[i]-topx)<xtempmin){
				xtempmin = txcenter[i] - topx;
				xtopmin = txcenter[i];
			}
		}
	}

	corner[0] = Point2f(xtopmin,topy);
	corner[1] = Point2f(xtopmax,topy);
	return corner;
}

Point JointPoint(Point pCur, Point pLT, Point pLB, Point pRB, Point pRT){       // 坐标转换后的矫正，当点在所框出四边形外时，将其矫正到四边形内
	 //任意四边形有4个顶点  
	int nCount = 4;  
	Point RectPoints[4] = { pLT, pLB, pRB, pRT };  
	int nCross = 0;
	std::vector<double> jointpointx;
	std::vector<double> jointpointy;  

	for (int i = 0; i < nCount; i++)   
	{     
		Point pStart = RectPoints[i];   
		Point pEnd = RectPoints[(i + 1) % nCount];  
		if(i%2==0){
			if ( pCur.y < min(pStart.y, pEnd.y) || pCur.y > max(pStart.y, pEnd.y))   
				continue;  
			double x = (double)(pCur.y - pStart.y) * (double)(pEnd.x - pStart.x) / (double)(pEnd.y - pStart.y) + pStart.x;
			jointpointx.push_back(x); 
		}else{
			if(pCur.x<min(pStart.x,pEnd.x) || pCur.x >max(pStart.x,pEnd.x))
				continue;
			double	y = (double)(pCur.y - pStart.y) * (double)(pEnd.y - pStart.y) / (double)(pEnd.x - pStart.x) + pStart.y;
			jointpointy.push_back(y);
		}
	} 
    // 单边交点为偶数，点在多边形之外
	if(jointpointx.size()==2){
		if(jointpointx[0]>pCur.x && jointpointx[1]>pCur.x){
			int range = randomrange(10,150);
			return Point(jointpointx[0]+range,pCur.y);
		}
		if (jointpointx[0]<pCur.x && jointpointx[1]<pCur.x)
		{
			int range = randomrange(10,150);
			return Point(jointpointx[1]-range,pCur.y);
		}
	}
	if(jointpointy.size()==2){
		if(jointpointy[0]<pCur.y && jointpointy[1]<pCur.y){
			int range = randomrange(10,150);
			return Point(pCur.x,jointpointy[0]-range);
		}
		if (jointpointy[0]>pCur.y && jointpointy[1]>pCur.y)
		{
			int range = randomrange(10,150);
			return Point(pCur.x,jointpointy[1]+range);
		}
	}  

	return pCur;   
}



tm getlocaltime()   //获取本地时间
{
	time_t curtime = time(NULL);
	tm *p = localtime(&curtime);
	return *p;
}

vector<string> yieldpath(string classid){  //产生保存数据的路径   filepath[0,1,2,3]代表json文件，保存压缩后的图片，视频和原始图片文件夹
	tm time  = getlocaltime();
	std::vector<string> allpath;
	std::vector<string> filepath;
	string rootpath = "F:/jfda/tmp/";
	string Yearpath = rootpath+IntString(time.tm_year+1900);
	string Monpath = Yearpath+"/"+IntString(time.tm_mon+1);
	string daypath;
	if(time.tm_mon+1<10)
		Monpath = Yearpath+"/0"+IntString(time.tm_mon+1);
	else
		Monpath = Yearpath+"/"+IntString(time.tm_mon+1);
	if(time.tm_mday<10)
		daypath = Monpath+"/0"+IntString(time.tm_mday);
	else
		daypath = Monpath+"/"+IntString(time.tm_mday);
	allpath.push_back(Yearpath);
	allpath.push_back(Monpath);
	allpath.push_back(daypath);
	allpath.push_back(daypath+"/"+"img/");
	allpath.push_back(daypath+"/"+"originPic/");
	allpath.push_back(daypath+"/"+"file/");
	allpath.push_back(daypath+"/"+"img/"+classid+"/");
	allpath.push_back(daypath+"/"+"video/");
	allpath.push_back(daypath+"/"+"originPic/"+classid+"/");
	for (int i = 0; i < allpath.size(); i++)
	{
		if(access(allpath[i].c_str(),0)==-1)
			mkdir(allpath[i].c_str());
		if(i>4)
			filepath.push_back(allpath[i]);
	}
	return filepath;
}


bool Checktime(int starthour,int startminute,int reststarthour,int reststartminute,int restendhour,int restendminute,int endhour,int endminute,PLAYM4_SYSTEM_TIME pstSystemTime){  //将时间加十分钟，判断是否开始统计总人数
	for (int i = 0; i < 9; ++i){
		int minute = (startminute + 10*i) % 60;
		int hour = starthour + (startminute + 10*i) / 60;
		if(pstSystemTime.dwHour==hour && pstSystemTime.dwMin==minute){
			if(((hour*60+minute)>=(starthour*60+startminute) && (hour*60+minute)<=(reststarthour*60+reststartminute)) || ((hour*60+minute)>=(restendhour*60+restendminute) && (hour*60+minute)<=(endhour*60+endminute)))
			{
				return true;
			}
		}
	}
	return false;
	
}

void createvideo(string picpath,string classid,int index,string videopath)  // 根据图片生成视频
{
	vector<string> files;
	files = ListDir(picpath, {"jpg"});
	Mat img = cv::imread(picpath + files[0], CV_LOAD_IMAGE_COLOR);
	cv::Size size_frame = cv::Size(img.cols, img.rows);
	VideoWriter outputvideo;
	string videoname = videopath+classid+"_"+IntString(index)+".avi";
	outputvideo.open(videoname, CV_FOURCC('X', 'V', 'I', 'D'), 15, size_frame, true);
	vector<int> picfile;
	for (int i = 0; i < files.size(); i++)
	{
		vector<string> temp;
		boost::split(temp, files[i], boost::is_any_of("_"), boost::token_compress_on);
		if (atoi(temp[1].c_str()) == index){
			picfile.push_back(atoi(temp[2].c_str()));
		}
	}
	sort(picfile.begin(), picfile.end());
	for (int i = 0; i < picfile.size(); i++)
	{
		for (int j = 0; j < files.size(); j++){
			vector<string> temp;
			boost::split(temp, files[j], boost::is_any_of("_"), boost::token_compress_on);
			if (atoi(temp[1].c_str()) == index){
				if (atoi(temp[2].c_str()) == picfile[i]){
					Mat frame = cv::imread(picpath + files[j], CV_LOAD_IMAGE_COLOR);
					cout << files[j] << endl;
					outputvideo.write(frame);
				}
			}
		}
	}
	Py_Initialize();//使用python之前，要调用Py_Initialize();这个函数进行初始化
	PyObject * pModule = NULL;//声明变量
	PyObject * pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule("2");//这里是要调用的文件名
	if (!pModule)
		cout << "cann't find module" << endl;
	pFunc = PyObject_GetAttrString(pModule, "trans");//这里是要调用的函数名
	if (!pFunc)
		cout << "cann't find trans" << endl;
	PyObject* args = PyTuple_New(1);
	PyObject* arg1 = Py_BuildValue("s", videopath.c_str()); // 字符串参数
	PyTuple_SetItem(args, 0, arg1);
	PyEval_CallObject(pFunc, args);//调用函数
	Py_Finalize();//调用Py_Finalize，这个根Py_Initialize相对应的。
	remove(videoname.c_str());
}





void concatface(vector<int> &txcenter, vector<int> &tycenter, vector<FaceInfo> &tface){
	for(int i = 0;i<tface.size();i++){
		for(int j = 0;j<tface.size();j++){
			if(i!=j){
				int area = calArea(tface[i].bbox,tface[j].bbox);
				if(area>6){
					txcenter[j] = 0;
					tycenter[j] = 0;
					tface[j].bbox.width = 0;
					tface[j].bbox.height = 0;
				}
			}
		}
	}
	vector<int>::iterator it = txcenter.begin();
	vector<int>::iterator it2 = tycenter.begin();
	vector<FaceInfo>::iterator it3 = tface.begin();
	while(it!=txcenter.end() && it2!=tycenter.end()){
		if((*it3).bbox.width==0){
			it = txcenter.erase(it);
			it2=tycenter.erase(it2);
			it3 = tface.erase(it3);
		}
		else{
			++it;
			++it2;
			++it3;
		}
	}
}


int decface(vector<int> &txcenter, vector<int> &tycenter, vector<FaceInfo> &tface)   //若多个点在同一个人脸框，则合并为一个点
{
	int totalface = 0;
	for (int i = 0; i < txcenter.size(); i++)
	{
		for (int j = 0; j < txcenter.size(); j++)
		{
			if(i!=j){
				if ((abs(txcenter[i] - txcenter[j]) < tface[i].bbox.width && abs(tycenter[i] - tycenter[j]) < tface[i].bbox.height) || 
					(abs(txcenter[i] - txcenter[j]) < tface[j].bbox.width && abs(tycenter[i] - tycenter[j]) < tface[j].bbox.height))
				{
					txcenter[j] = 0;
					tycenter[j] = 0;
					tface[j].bbox.width = 0;
					tface[j].bbox.height = 0;
				}
			} 
		}
	}
	vector<int>::iterator it = txcenter.begin();
	vector<int>::iterator it2 = tycenter.begin();
	vector<FaceInfo>::iterator it3 = tface.begin();
	while(it!=txcenter.end() && it2!=tycenter.end()){
		if((*it3).bbox.width==0){
			it = txcenter.erase(it);
			it2=tycenter.erase(it2);
			it3 = tface.erase(it3);
		}
		else{
			++it;
			++it2;
			++it3;
		}
	}
	totalface = txcenter.size();
	return totalface;
}

int calArea(cv::Rect rect1, cv::Rect rect2)
{
	int area = 0.0;
	int x1 = max(rect1.x,rect2.x);
	int y1 = max(rect1.y, rect2.y);
	int x2 = min(rect1.x + rect1.width, rect2.x + rect2.width);
	int y2 = min(rect1.y + rect1.height, rect2.y + rect2.height);
	if((x2-x1)>=0 && (y2-y1)>=0){
		area = (x2 - x1) * (y2 - y1);
	}
	else{
		area = 0;
	}		
	return area;
}


void concatpoints(vector<FaceInfo> &rface, vector<FaceInfo> &tface, vector<int> &rxcenter, vector<int> &rycenter, vector<int> &txcenter, vector<int> &tycenter)  //人脸合并和跟踪
{
	int total = txcenter.size();
	vector<int> x;
	vector<int> y;
	int concatsize = 0;
	vector<int> tempxcenter;
	vector<int> tempycenter;
	vector<FaceInfo> tempface;
	if(rxcenter.size()==0 || txcenter.size()==0)
		return;
	for (int i = 0; i < rxcenter.size(); i++)
	{
		concatsize = 0;
		int tempx;
		int tempy;
		bool add = false;
		for (int j = 0; j < txcenter.size(); j++)
		{
			double distance = 100000.0;
			if ((abs(rxcenter[i] - txcenter[j]) < tface[j].bbox.width && abs(rycenter[i] - tycenter[j]) < tface[j].bbox.height) ||
				(abs(rxcenter[i] - txcenter[j]) < rface[i].bbox.width && abs(rycenter[i] - tycenter[j]) < rface[i].bbox.height))
			{
				double dis = calDistance(rxcenter[i], rycenter[i], txcenter[j], tycenter[j]);
				add = true;
				if (dis < distance)
				{
					distance = dis;
					tempx = i;
					tempy = j;
				}
				continue;
			}
			else
			{
				concatsize++;
			}
		}
		if (add)
		{
			x.push_back(tempx);
			y.push_back(tempy);
		}
		if (concatsize == total)
		{
			tempxcenter.push_back(rxcenter[i]);
			tempycenter.push_back(rycenter[i]);
			tempface.push_back(rface[i]);
		}
	}
	for (int i = 0; i < x.size(); i++)
	{
		txcenter[y[i]] = rxcenter[x[i]];
		tycenter[y[i]] = rycenter[x[i]];
		tface[y[i]] = rface[x[i]];
	}
	for (int i = 0; i < tempxcenter.size(); i++)
	{
		txcenter.push_back(tempxcenter[i]);
		tycenter.push_back(tempycenter[i]);
		tface.push_back(tempface[i]);
	}
	rxcenter.clear();
	rycenter.clear();
	rface.clear();
	tempface.clear();
	tempxcenter.clear();
	tempycenter.clear();
}

double calDistance(int x1, int y1, int x2, int y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

vector<string> ListDir(const string &dpath, const vector<string> &exts)
{
	vector<string> lists;
	if (!IsExists(dpath))
	{
		return lists;
	}
#ifdef WIN32
	intptr_t fd;
	_finddata_t fi;
	string mp = dpath + "/*";
	if ((fd = _findfirst(mp.c_str(), &fi)) != -1)
	{
		do
		{
			string ext = GetFileExtension(fi.name);
			for (int i = 0; i < exts.size(); i++)
			{
				if (StringCompareCaseInsensitive(ext, exts[i]))
				{
					lists.push_back(fi.name);
					break;
				}
			}
		} while (_findnext(fd, &fi) == 0);
		_findclose(fd);
	}
#else
	DIR *dir;
	struct dirent *file;
	struct stat st;
	if (!(dir = opendir(dpath.c_str())))
	{
		return lists;
	}
	while ((file = readdir(dir)) != NULL)
	{
		string fn(file->d_name);
	// remove '.', '..'
		if (fn == "." || fn == "..")
		{
			continue;
		}
		string ext = GetFileExtension(fn);
		for (int i = 0; i < exts.size(); i++)
		{
			if (StringCompareCaseInsensitive(ext, exts[i]))
			{
				lists.push_back(fn);
				break;
			}
		}
	}
	closedir(dir);
#endif // WIN32
	return lists;
}

string GetFileExtension(const string &path)
{
	int pos = path.find_last_of(".");
	if (pos == string::npos)
	{
		return "";
	}
	else
	{
		return path.substr(pos + 1);
	}
}
/*!
* \brief compare two string with case insensitive
* \param s1  string1
* \param s2  string2
* \return    true if equal else false
*/
static bool StringCompareCaseInsensitive(const string &s1, const string &s2)
{
	if (s1.length() != s2.length())
		return false;
	const int n = s1.length();
	for (int i = 0; i < n; i++)
	{
		if (std::tolower(s1[i]) != std::tolower(s2[i]))
			return false;
	}
	return true;
}

bool IsExists(const string &path)
{
	if (access(path.c_str(), 0) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}



void delete_path(string path){  
	vector<string> file = ListDir(path, { "jpg" });
	for (int i = 0; i < file.size(); i++)
	{
		string temp = path + file[i];
		remove(temp.c_str());
	}
} 

