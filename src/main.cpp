#include <string>
#include <iostream>
#include "MyThreadPool.h"
#include "MyTask.h"
#include "Timer.hpp"
#include <time.h>
#include "face.hpp"
#include <thread>
#include <vector>
#include <fstream>
#include <ConfigRead.h>
#include<conio.h>
using namespace std;


void Restart();
void GetConfigFile();
void CompareReader(ConfigRead &reader,ConfigRead &tmpreader);
extern vector<int> taskPort;
MyThreadPool mythreadPool(threadnums);
MyTask tasks[tasknum];
ConfigRead reader;



void Restart(){                 //在检测到教室没有人时，十分钟之后重新检测
	while(true){
		this_thread::sleep_for(std::chrono::seconds(10));
		if(taskPort.empty())
			continue;
		else{
			vector<int>::iterator it = taskPort.begin();
			while(it!=taskPort.end()){
				if((*it)!=0){
					for (int i = 0; i < tasknum; ++i){
						if(*it == tasks[i].nPort){
							cout<<"restart"<<endl;
							mythreadPool.AddTask(&tasks[i],20);
							it = taskPort.erase(it);
							tasks[i].IsCalNum = false;
						}
					}
				}
				else{
					++it;
				}
			}
		}	
	}
}

void GetConfigFile(){                   //每隔5分钟向web服务器发送请求，获取配置文件
	this_thread::sleep_for(std::chrono::seconds(10));
	ConfigRead tmpreader;
	tmpreader.GetJson();
	vector<string> cameraip;
	vector<int> cameraid;
	vector<string> cameraaccount;
	vector<string> camerapassword;
	vector<int> cameraNvrid;
	vector<int> cameraNvrport;
	CompareReader(reader,tmpreader);
	for (int i = 0; i < tmpreader.cameraIp.size(); ++i){
		vector<string>::iterator it;
		it = find(reader.cameraIp.begin(),reader.cameraIp.end(),tmpreader.cameraIp[i]);
		if(it==reader.cameraIp.end()){
			cameraip.push_back(tmpreader.cameraIp[i]);
			cameraid.push_back(tmpreader.cameraid[i]);
			cameraaccount.push_back(tmpreader.cameraAccount[i]);
			camerapassword.push_back(tmpreader.cameraPassword[i]);
			cameraNvrid.push_back(tmpreader.cameraNvrID[i]);
			cameraNvrport.push_back(tmpreader.cameraNvrPort[i]);
		}
	}
	for (int i = 0; i <cameraip.size() ; ++i){
		int j = reader.cameraIp.size()+i;
		tasks[j].setgpu(0);
		tasks[j].setcameraIp(cameraip[i]);
		tasks[j].setPlayPort(j+1);
		tasks[j].setcameraAccount(cameraaccount[i]);
		tasks[j].setcameraPassword(camerapassword[i]);
		tasks[j].setNvrPort(cameraNvrport[i]);
		for (int z = 0; z < reader.NvrIp.size(); ++z)
		{
			if(cameraNvrid[i]==reader.Nvrid[z]){
				tasks[j].setNvrIp(reader.NvrIp[z]);
				tasks[j].setNvrAccount(reader.NvrAccount[z]);
				tasks[j].setNvrPassword(reader.NvrPassword[z]);
			}
		}
		mythreadPool.AddTask(&tasks[j],20);	
	}
	for (int i = 0; i <cameraip.size() ; ++i){
		reader.cameraIp.push_back(cameraip[i]);
		reader.cameraid.push_back(cameraNvrid[i]);
		reader.cameraAccount.push_back(cameraaccount[i]);
		reader.cameraPassword.push_back(camerapassword[i]);
		reader.cameraNvrID.push_back(cameraNvrid[i]);
		reader.cameraNvrPort.push_back(cameraNvrport[i]);
	}
}


void CompareReader(ConfigRead &reader,ConfigRead &tmpreader){            //将新获取的json配置文件更新原始json文件
	reader.PeriodNum = tmpreader.PeriodNum;
	reader.StartHour = tmpreader.StartHour;
	reader.RestStartHour = tmpreader.RestStartHour;
	reader.EndHour = tmpreader.EndHour;
	reader.RestEndHour = tmpreader.RestEndHour;
	reader.StartMinute = tmpreader.StartMinute;
	reader.RestStartMinute = tmpreader.RestStartMinute;
	reader.EndMinute = tmpreader.EndMinute;
	reader.RestEndMinute = tmpreader.RestEndMinute;
	reader.NvrIp = tmpreader.NvrIp;
	reader.Nvrid = tmpreader.Nvrid;
	reader.NvrAccount = tmpreader.NvrAccount;
	reader.NvrPassword = tmpreader.NvrPassword;
}


int main(int argc, char* argv[]) {
	thread restart(Restart);
	restart.detach();
	//thread reGetJson(GetConfigFile);
    //reGetJson.detach();
	//createvideo("../tmp/2017/05/25/originPic/1/","1",5,"../tmp/2017/05/25/video/");
	//reader.GetJson();
	
	reader.getCameraInfo();
	reader.getNvrinfo();
	reader.getCourseinfo();
	for (int i = 0; i < reader.cameraIp.size(); ++i)
	{
		tasks[i].setgpu(0);
		tasks[i].setcameraIp(reader.cameraIp[i]);
		tasks[i].setPlayPort(i+1);
		tasks[i].setcameraAccount(reader.cameraAccount[i]);
		tasks[i].setcameraPassword(reader.cameraPassword[i]);
		tasks[i].setNvrPort(reader.cameraNvrPort[i]);
		for (int j = 0; j < reader.NvrIp.size(); ++j)
		{
			if(reader.cameraNvrID[i]==reader.Nvrid[j]){
				tasks[i].setNvrIp(reader.NvrIp[j]);
				tasks[i].setNvrAccount(reader.NvrAccount[j]);
				tasks[i].setNvrPassword(reader.NvrPassword[j]);
			}
		}
	}

	for(int i = 0;i<reader.cameraIp.size();i++){
		mythreadPool.AddTask(&tasks[i],20);	
	}
	
	if (getch()){
		mythreadPool.EndMyThreadPool();
	}
	system("pause");
	return 0;
}








