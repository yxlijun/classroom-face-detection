#pragma once

#include <string>
#include <iostream>
#include "MyTask.h"
#include "face.hpp"
#include <vector>
#include <fstream>
#include "curl.h"
#include "easy.h"
#include "curlbuild.h"
#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <ctime>


using namespace std;


/*

解析配置文件类
*/
class ConfigRead
{
public:
	vector<string> cameraIp;
	vector<int> cameraid;
	vector<string> cameraAccount;
	vector<string> cameraPassword;
	vector<int> cameraNvrID;
	vector<int> cameraNvrPort;


	vector<string> NvrIp;
	vector<int> Nvrid;
	vector<string> NvrAccount;
	vector<string> NvrPassword;

	vector<int> PeriodNum;
	vector<int> StartHour;
	vector<int> RestStartHour;
	vector<int> EndHour;
	vector<int> RestEndHour;
	vector<int> StartMinute;
	vector<int> RestStartMinute;
	vector<int> EndMinute;
	vector<int> RestEndMinute;
	ConfigRead();
	~ConfigRead();
	void getCameraInfo();   //解析摄像头信息
	void getNvrinfo();       //解析Nvr信息
	void getCourseinfo();   //解析上课信息
	void GetJson();         //发起请求，获取配置文件



protected:
	string path;
};









