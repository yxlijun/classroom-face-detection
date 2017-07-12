#include "MyTask.h"
#include <thread>
#include <iostream>
#include <vector>

using namespace std;
vector<int> taskPort;


extern MyTask tasks[6];
extern ConfigRead reader;

template <typename T>
int length(T &x)          // 计算数组长度
{
	int s1 = sizeof(x);
	int s2 = sizeof(x[0]);
	int result = s1 / s2;
	return result;
}

MyTask::MyTask()
{
	thread_this = thread(&MyTask::image_to_video,this);
	thread_this.detach();
	calhead_num = 1;
	total_heacount = 0;
	framenum=0;
	IsCalNum = false;
	EndCount = 0;
}

MyTask::~MyTask()
{
	
}


void MyTask::setgpu(int gpu_device)
{
	gpu = gpu_device;
}

void MyTask::setcameraIp(string ip){   //设置摄像头ip地址
	cameraip = ip;
}

void MyTask::setPlayPort(int port){  //设置摄像头端口号
	nPort = port;
}

void MyTask::setcameraAccount(string cameraccount){  //设置摄像头用户名
	cameraAccount = cameraccount;
}
void MyTask::setcameraPassword(string camerapasswd){  //设置摄像密码
	cameraPassword = camerapasswd;
}

void MyTask::setNvrIp(string nvrip){
	NvrIp = nvrip;
}

void  MyTask::setNvrAccount(string nvraccount){
	NvrAccount = nvraccount;
}

void MyTask::setNvrPassword(string nvrpassword){
	NvrPassword = nvrpassword;
}
void MyTask::setNvrPort(int nvrport){    
	NvrPort = nvrport;
}


void MyTask::image_to_video(){   //每天晚上凌晨将视频转化为视频
	this_thread::sleep_for(std::chrono::hours(1));
	tm time = getlocaltime();
	if(time.tm_hour==0){
		for(int i= 0;i<10;i++){
			createvideo(path[3],IntString(nPort),i,path[2]);
		}
	}
	delete_path(path[1]);
	delete_path(path[3]);
}

void yv12toYUV(char *outYuv, char *inYv12, int width, int height,int widthStep)  
{  
	int col,row;  
	unsigned int Y,U,V;  
	int tmp;  
	int idx;  
	for (row=0; row<height; row++)  
	{  
		idx=row * widthStep;  
		int rowptr=row*width;  

		for (col=0; col<width; col++)  
		{    
			tmp = (row/2)*(width/2)+(col/2);
			Y=(unsigned int) inYv12[row*width+col];  
			U=(unsigned int) inYv12[width*height+width*height/4+tmp];  
			V=(unsigned int) inYv12[width*height+tmp];  
			outYuv[idx+col*3]   = Y;  
			outYuv[idx+col*3+1] = U;  
			outYuv[idx+col*3+2] = V;  
		}  
	}  
}  

bool MyTask::CalHead(Mat &img, JfdaDetector &detector){
	vector<FaceInfoInternal> facem;
	vector<int> rxcenter;
	vector<int> rycenter;
	vector<FaceInfo> rface;  //当前帧检测到人脸的信息
	vector<FaceInfo> faces = detector.Detect(img, facem);
	for (int i = 0; i < faces.size(); i++)
	{
		FaceInfo &face = faces[i];
		if (calhead_num == 1)
		{
			txcenter.push_back(face.bbox.x + face.bbox.width / 2);  //如果是第一帧，则将第一帧保存到总的人脸信息中
			tycenter.push_back(face.bbox.y + face.bbox.height / 2);
			tface.push_back(face);
		}
		else
		{
			rxcenter.push_back(face.bbox.x + face.bbox.width / 2);
			rycenter.push_back(face.bbox.y + face.bbox.height / 2);
			rface.push_back(face);
		}
	}
	if (calhead_num >= 2)
	{
		concatpoints(rface, tface, rxcenter, rycenter, txcenter, tycenter);  //将相邻两帧进行人脸合并
	}
	int tfacesize = decface(txcenter, tycenter, tface);

	if(!booldetection(tfacesize)){
		EndCount++;
		if(EndCount==10){
			if(taskPort.empty()){
				taskPort.push_back(nPort);
			}else{
				vector<int>::iterator it;
				it = find(taskPort.begin(),taskPort.end(),nPort);
				if(it==taskPort.end()){
					taskPort.push_back(nPort);
				}
			}
			NET_DVR_Logout(UserID);  
			NET_DVR_Cleanup();
			EndCount = 0;
			return false;
		}
	}
	if (total_heacount < tfacesize)       //获取最大的人脸数
	{
		total_heacount = tfacesize;
		tfxcenter = txcenter;
		tfycenter = tycenter;
		ttface = tface;
	}
	calhead_num++;
	if(calhead_num==100){   //只进行100次检测，统计总人数结束
		txcenter.clear();
		tycenter.clear();
		tface.clear();
		calhead_num=1;
		return true;
	}
	else{
		return false;
	}
}


void CALLBACK DecCBFun(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, long nReserved1, long nReserved2)
{  
	long lFrameType = pFrameInfo->nType;   
	cout<< pFrameInfo->nStamp<<endl;
	for(int i = 0;i<2;i++){
		if(nPort==tasks[i].nPort){
			if(lFrameType ==T_YV12)  
			{  
    			IplImage* pImgYCrCb = cvCreateImage(cvSize(pFrameInfo->nWidth,pFrameInfo->nHeight), 8, 3);//得到图像的Y分量    
        		yv12toYUV(pImgYCrCb->imageData, pBuf, pFrameInfo->nWidth,pFrameInfo->nHeight,pImgYCrCb->widthStep);//得到全部RGB图像  
        		IplImage* pImg = cvCreateImage(cvSize(pFrameInfo->nWidth,pFrameInfo->nHeight), 8, 3);    
        		cvCvtColor(pImgYCrCb,pImg,CV_YCrCb2RGB);
        		cv::Mat img(pImg,true);
        		cvReleaseImage(&pImgYCrCb);  
        		cvReleaseImage(&pImg);
        		JfdaDetector detector("F:/jfda/model/jfda/p.prototxt", "F:/jfda/model/jfda/p.caffemodel",
        			"F:/jfda/model/jfda/r.prototxt", "F:/jfda/model/jfda/r.caffemodel",
        			"F:/jfda/model/jfda/o.prototxt", "F:/jfda/model/jfda/o.caffemodel",
        			"F:/jfda/model/jfda/l.prototxt", "F:/jfda/model/jfda/l.caffemodel");
        		detector.SetMinSize(20);
        		detector.SetMaxImageSize(100000000);
        		detector.SetStageThresholds(0.5, 0.4, 0.4);
        		PLAYM4_SYSTEM_TIME pstSystemTime;
        		PlayM4_GetSystemTime(nPort,&pstSystemTime);  // 获取当前视频的时间
        		int sequence;                               //代表是第几节课
        		bool Isclasstime = false;                  //是否是上课时间
        		bool isFinished = false;                    // 一节课是否结束
        		vector<string> allpath = tasks[i].path;
        		for (int j = 0; j < reader.StartHour.size(); j++){        //判断当前视频时间是否在某一上课时间
        			if ((pstSystemTime.dwHour*60 + pstSystemTime.dwMin) >= (reader.StartHour[j] * 60 + reader.StartMinute[j]) && (pstSystemTime.dwHour * 60 + pstSystemTime.dwMin) <=(reader.EndHour[j] * 60 + reader.EndMinute[j])){
        				sequence = j;   
        				Isclasstime = true;
        				if(pstSystemTime.dwHour == reader.EndHour[j] && pstSystemTime.dwMin == reader.EndMinute[j]){
        					isFinished = true;
        				}
        				break;
        			}
        		}
        		if(!Isclasstime){
        			for (int z = 0; z < reader.EndHour.size()-1; ++z){          // 判断当前时间是否在两节课之间的时间大于一个小时内，若在，则中断任务，退出线程，若不在  ，则直接返回函数，等待上课时间不进行之后的操作
        				int j = z+1;
        				if((reader.StartHour[j]*60+reader.StartMinute[j])-(reader.EndHour[z]*60+reader.EndMinute[z])>=60){
        					if((pstSystemTime.dwHour*60+pstSystemTime.dwMin)>(reader.EndHour[z]*60+reader.EndMinute[z]) && (pstSystemTime.dwHour * 60 + pstSystemTime.dwMin) < (reader.StartHour[j] * 60 + reader.StartMinute[j]))
        					{
        						if(taskPort.empty()){
        							taskPort.push_back(nPort);
        						}else{
        							vector<int>::iterator it;
        							it = find(taskPort.begin(),taskPort.end(),nPort);
        							if(it==taskPort.end()){
        								taskPort.push_back(nPort);
        							}
        						}
        						NET_DVR_Logout(tasks[i].UserID);  
        						NET_DVR_Cleanup();
        						break;
        					}
        				}
        			}
        			tasks[i].totalnum.clear();
        			tasks[i].Data.clear();
        			return;
        		}
        		else{  // 判断当前时间是否该进行总人数的统计
					tasks[i].framenum++;

        			if(Checktime(reader.StartHour[sequence],reader.StartMinute[sequence],reader.RestStartHour[sequence],reader.RestStartMinute[sequence],reader.RestEndHour[sequence],reader.RestEndMinute[i],
        				reader.EndHour[sequence],reader.RestEndMinute[sequence],pstSystemTime)){
        				tasks[i].IsCalNum = true;
					}
					else{
						tasks[i].IsCalNum = false;
					}
        		if(tasks[i].IsCalNum){
        			if(tasks[i].CalHead(img,detector)){   //统计总人数
        				tasks[i].totalnum.push_back(tasks[i].total_heacount);
        				tasks[i].ttface.clear();
        				tasks[i].tfxcenter.clear();
        				tasks[i].tfycenter.clear();
        				tasks[i].total_heacount = 0;
        				tasks[i].IsCalNum = false;
        				tasks[i].EndCount = 0;
        			}
        		}
        		if(tasks[i].framenum%10==0)            //每10帧进行一次保存数据，下面的代码都是保存数据的
        		{
        			cout<<pstSystemTime.dwHour<<"::"<<pstSystemTime.dwMin<<":"<<pstSystemTime.dwSec<<endl;
        			cout<<"framenum:"<<tasks[i].framenum<<endl;
        			Json::Value riseroot;
        			Json::Value riseData;
        			Json::Value data;
        			vector<FaceInfoInternal> facem;
        			vector<FaceInfo> faces = detector.Detect(img, facem);
        			int headcount = faces.size();
        			vector<Point2f> points_trans,points;
        			for (int j = 0; j < faces.size() ; ++j){
        				FaceInfo& face = faces[j];
        				double x = face.bbox.x+face.bbox.width/2;
        				double y = face.bbox.y+face.bbox.height/2;
        				Point pCur = Point(x,y);
        				Point p = JointPoint(pCur,tasks[i].corners[0],tasks[i].corners[2],tasks[i].corners[3],tasks[i].corners[1]);
        				points.push_back(Point2f(p.x,p.y));
        			}
        			if(headcount>0)
        				perspectiveTransform(points, points_trans, tasks[i].transform);   //进行坐标转换
        			for (int j = 0; j < faces.size(); j++)
        			{
        				FaceInfo& face = faces[j];
        				cv::rectangle(img, face.bbox, Scalar(0, 0, 255), 2);
        				cv::circle(img, Point(face.bbox.x+face.bbox.width/2,face.bbox.y+face.bbox.height/2), 2, Scalar(0, 255, 0), -1);
        				if(points_trans[j].x<0){
        					data[0U] = 10;
        				}else if(points_trans[j].x>=1920){
        					data[0U] = 1910;
        				}
        				else{
        					data[0U] = (int)points_trans[j].x;
        				}
        				if(points_trans[j].y<0){
        					data[1] = 10;
        				}else if(points_trans[j].y>=1080){
        					data[1] = 1070;
        				}
        				else{
        					data[1] = (int)points_trans[j].y;
        				}
        				riseData["headLocation"].append(data);  //保存坐标信息
        			}

        			stringstream str,oriPicPath;
        			str <<allpath[1]<<IntString(nPort)+"_"+IntString(sequence)+"_"+IntString(tasks[i].framenum/10)+".jpg";     //压缩后的图片保存路径
        			oriPicPath <<allpath[3]<<IntString(nPort)+"_"+IntString(sequence)+"_"+IntString(tasks[i].framenum/10)+".jpg";  //原始图片的保存路径

        			string date = timestr(pstSystemTime.dwYear,pstSystemTime.dwMon,pstSystemTime.dwDay,pstSystemTime.dwHour,pstSystemTime.dwMin,pstSystemTime.dwSec);
        			string picname = IntString(nPort)+"_"+IntString(sequence)+"_"+IntString(tasks[i].framenum/10)+".jpg";   // 图片格式为 教室_节次_图片序号.jpg

        			int rise;      //抬头率变量
        			vector<int> tmptotalnum = tasks[i].totalnum;     //将该节课得到总人数数组赋给临时变量
        			if(!tmptotalnum.empty()){      	                             //本节课进行了一次总人数统计
        				rise = ceil(100*headcount/tmptotalnum[tmptotalnum.size()-1]);  //抬头率用最近一次总人数计算
        				riseData["headCount"] = tmptotalnum[tmptotalnum.size()-1];	 // 总人数也是最近一次的
        				sort(tmptotalnum.begin(),tmptotalnum.end());                //对其数组排序
        				riseroot["maxHeadCount"] = tmptotalnum[tmptotalnum.size()-1];  //在所有总人数中，获得最大值
        				riseroot["minHeadCount"] = tmptotalnum[0];   //获取最小值

        			}
        			else{     //没有进行总人数统计，构造一个假设值
        				rise = 100;
        				riseData["headCount"] = headcount;
        				riseroot["maxHeadCount"] = headcount;
        				riseroot["minHeadCount"] = headcount;
        			}
        			tasks[i].drawrise.push_back(rise);  //抬头率加入抬头率数组，以便后续画图的使用
        			riseData["time"] = date;          //保存日期
        			riseData["riseCount"] = headcount;   //保存当前时刻的抬头数
        			riseData["picName"] = picname;      //图片，名称

        			tasks[i].Data.push_back(riseData);    

        			for(int j = 0;j<tasks[i].Data.size();j++){
        				riseroot["data"].append(tasks[i].Data[j]);			 
        			}
        			riseroot["cameraId"]  = IntString(tasks[i].nPort);    //教室ID
        			riseroot["coursePeriod"] = sequence;               //课堂节次
        			riseroot["fileName"] = IntString(nPort)+"_"+IntString(sequence)+".json";   //保存的json文件名，格式为 教室_节次.json
        			riseroot["videoName"] = IntString(nPort)+"_"+IntString(sequence)+".mp4";   //生成的视频名, 格式为教室_节次.mp4

        			if(isFinished){  //本节课是否结束
        				riseroot["isFinished"] = "true";
        				ofstream ofs;
        				ofs.open(allpath[0]+ IntString(nPort)+"_"+IntString(sequence)+".json");
        				ofs << riseroot.toStyledString() << endl;
        			}else{
        				riseroot["isFinished"] = "false";
        				ofstream ofs;
        				ofs.open(allpath[0]+ IntString(nPort)+"_"+IntString(sequence)+".json");
        				ofs << riseroot.toStyledString() << endl;
        			}

        			string rises = IntString(rise);   
        			cv::line(img,Point(20, 20), Point(1880, 20), Scalar(0, 0, 0), 1);
        			cv::line(img,Point(1880, 20), Point(1880, 220), Scalar(0, 0, 0), 1);
        			cv::line(img,Point(1880, 220), Point(20, 220), Scalar(0, 0, 0), 1);
        			cv::line(img,Point(20, 220), Point(20, 20), Scalar(0, 0, 0), 1);
        			cv::line(img, Point(40, 130), Point(40, 130 - rise), Scalar(0, 255, 0), 20);
        			cv::putText(img, rises, Point(30,170), 0, 0.8, Scalar(255, 0, 0), 2);
        			IplImage* image;
        			image = cvCreateImage(cvSize(img.cols,img.rows),8,3);
        			IplImage ipltemp=img;
        			cvCopy(&ipltemp,image);
        			drawIntGraph(&tasks[i].drawrise[0], tasks[i].drawrise.size(),CV_RGB(255, 0 , 0),image);
        			IplImage *saveimg;
        			double fscale = 0.4;
        			CvSize czSize;
        			czSize.width = image->width * fscale;  
        			czSize.height = image->height * fscale;
        			saveimg = cvCreateImage(czSize,image->depth, image->nChannels);
        			cvResize(image, saveimg, CV_INTER_AREA);  
        			cvSaveImage(str.str().c_str(), saveimg);
        			cvSaveImage(oriPicPath.str().c_str(), image);
					cvReleaseImage(&saveimg);
					cvReleaseImage(&image);

        		}
        	}
        }
    }
}
}  


void CALLBACK fRealDataCallBack(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD dwBufSize,void *pUser)  
{  
	for (int i = 0; i < 2; ++i)
	{
		if(tasks[i].handle == lRealHandle){
			DWORD dRet;
			switch (dwDataType)  
			{  
	    		case NET_DVR_SYSHEAD:    //系统头  
	    		if(dwBufSize > 0)  
	    		{  
	    			if (!PlayM4_OpenStream(tasks[i].nPort,pBuffer,dwBufSize,SOURCE_BUF_MAX))  
	    			{  
	    				dRet=PlayM4_GetLastError(tasks[i].nPort);  
	    				break;  
	    			}  
	            //设置解码回调函数 只解码不显示  
	    			if (!PlayM4_SetDecCallBack(tasks[i].nPort,DecCBFun))  
	    			{  
	    				dRet=PlayM4_GetLastError(tasks[i].nPort);  
	    				break;  
	    			}  
	            //打开视频解码 
	    			PlayM4_SetDecodeFrameType(tasks[i].nPort,1);
	    			if (!PlayM4_Play(tasks[i].nPort,NULL))  
	    			{  
	    				dRet=PlayM4_GetLastError(tasks[i].nPort);  
	    				break;  
	    			}  

	    		}  
	    		break;  

	    		case NET_DVR_STREAMDATA:   //码流数据  
	    		if (dwBufSize > 0 && tasks[i].nPort != -1)  
	    		{  
	    			BOOL inData=PlayM4_InputData(tasks[i].nPort,pBuffer,dwBufSize);  
	    			while (!inData)  
	    			{  
	    				Sleep(10);  
	    				inData=PlayM4_InputData(tasks[i].nPort,pBuffer,dwBufSize);  
	    			}  
	    		}  
	    		break;    
	    	}  
	    	break;       
	    }
	}
}  


void MyTask::Run()
{
	// 注册摄像头，并执行相应的回调函数
	char * Cameraip = new char[cameraip.length()+1];
	char *CameraAccount = new char[cameraAccount.length()+1];
	char *CameraPassword = new char[cameraPassword.length()+1];
	strcpy(CameraAccount,cameraAccount.c_str());
	strcpy(Cameraip,cameraip.c_str());
	strcpy(CameraPassword,cameraPassword.c_str());

	NET_DVR_Init();
  //设置连接时间与重连时间
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);
	LONG lUserID;  
	NET_DVR_DEVICEINFO_V30 struDeviceInfo; 
	lUserID = NET_DVR_Login_V30(Cameraip, 8000, CameraAccount, CameraPassword, &struDeviceInfo);  

	if (lUserID < 0){  
		printf("Login error, %d\n", NET_DVR_GetLastError()); 
		NET_DVR_Logout(lUserID);   
		NET_DVR_Cleanup();  
		return ;  
	}

	NET_DVR_PREVIEWINFO ClientInfo;
	ClientInfo.lChannel = 1;
	ClientInfo.dwLinkMode = 0;
	ClientInfo.hPlayWnd = NULL;
	ClientInfo.dwStreamType = 0;
	LONG lRealPlayHandle;  



	lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID,&ClientInfo,fRealDataCallBack,NULL);  
	handle = lRealPlayHandle;
	UserID = lUserID;
	if (lRealPlayHandle<0)  
	{  
		printf("NET_DVR_RealPlay_V30 failed! Error number: %d\n",NET_DVR_GetLastError());  
		return ;  
	}  
	vector<Point2f> corners_trans(4),corner(4);
	corner[0] = Point2f(692,354);
	corner[1] = Point2f(1202,356);
	corner[2] = Point2f(3,762);
	corner[3] = Point2f(1867,831);
	corners_trans[0] = Point2f(0,100);  
	corners_trans[1] = Point2f(1920,100);  
	corners_trans[2] = Point2f(0,1080);  
	corners_trans[3] = Point2f(1920,1080); 

	transform = getPerspectiveTransform(corner,corners_trans);   //获取坐标转换矩阵
	path = yieldpath(IntString(nPort));   // 生成需要保存数据的文件夹

	Sleep(-1);
	NET_DVR_Logout(lUserID);  
	NET_DVR_Cleanup();  
}


