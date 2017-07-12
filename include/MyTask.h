#pragma once
#include "Task.h"
#include <string>
#include "jfda.hpp"
#include <vector>
#include <assert.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Timer.hpp"
#include "face.hpp"
#include <time.h>
#include <thread>
#include "config.hpp"
#include <fstream>
#include <json/json.h>
#include "GraphUtils.h"
#include "PlayM4.h"
#include "cv.h"
#include "HCNetSDK.h"  

#pragma comment(lib,"HCNetSDK.lib")
#pragma comment(lib,"PlayCtrl.lib")
using namespace std;
using namespace cv;
class MyTask : public Task
{
    friend bool operator<(MyTask &lv, MyTask &rv)
    {
        return lv.priority_ < rv.priority_;
    }
  public:
    MyTask();
    ~MyTask();
    virtual void Run();

    bool CalHead(cv::Mat &img, jfda::JfdaDetector &detector);    // 计算总人数
    void setgpu(int gpu_device);              //设置任务占用的GPU
    
    void setcameraIp(string ip);           //设置摄像头Ip
    void setPlayPort(int port);             //设置播放时时的通道nPort
    void setcameraAccount(string cameraccount);    //设置摄像头账号
    void setcameraPassword(string camerapasswd);      //设置摄像头密码

    void setNvrIp(string nvrip);               //设置Nvr的ip
    void setNvrAccount(string nvraccount);         //设置Nvr的账号
    void setNvrPassword(string nvrpassword);       //设置Nvr的密码
    void setNvrPort(int nvrport);               //设置摄像头在Nvr的通道号
    void image_to_video();                   //将图片转换为视频 

    int nPort;    //对应的播放通道，在该程序中，该变量用来区分摄像头
    
    vector<int> drawrise;   //抬头率数组
    vector<Json::Value> Data;        
    vector<string> path;          //数据保存数组，path[0,1,2,3]分别代表json文件，压缩后的图片，视频，原始图片路径

    Mat transform;     //坐标转换矩阵
    int handle;        //句柄
    LONG UserID;  
    int gpu;
    std::vector<Point2f> corners;
    vector<int> totalnum;
    vector<int> tfxcenter;  //总人数中心x
    vector<int> tfycenter;  //总人数中心y
    vector<FaceInfo> ttface;  //总人数bbox

    int calhead_num;             //该变量代表在计算总人数时的控制变量，当calhead_num = 100时，统计总人数结束
    int total_heacount;           //总人数变量
    int framenum;              // 控制10s进行一次保存数据
    bool IsCalNum;              // 是否开始进行统计总人数变量
    int EndCount;              // 在检测人数少于一定时，停止检测，该变量等于10时，才生效

  private:
    vector<int> txcenter;
    vector<int> tycenter;
    vector<FaceInfo> tface;
    string cameraAccount;
    string cameraPassword;
    string cameraip;  //摄像头的ip
    string NvrAccount;
    string NvrPassword;
    string NvrIp;
    int NvrPort;
    thread thread_this;
};
