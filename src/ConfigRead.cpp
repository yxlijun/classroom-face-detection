#include <ConfigRead.h>
#pragma comment(lib,"libcurl_a.lib")

ConfigRead::ConfigRead(){
	path = "F:/jfda/data/configFile/setting.json";
}
ConfigRead::~ConfigRead(){

}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	string data((const char*) ptr, (size_t) size * nmemb);
	*((stringstream*) stream) << data << endl;
	return size * nmemb;
}
void ConfigRead::GetJson(){
	for (int i = 0; i < 100; ++i)
	{
		time_t now = time(0);
		char* dt = ctime(&now);

 
		std::stringstream out;
        //curl_global_init(CURL_GLOBAL_WIN32);
		void* curl = curl_easy_init();
        // 设置URL

		curl_easy_setopt(curl, CURLOPT_URL, "http://10.11.51.217:8000/getsetting");
        // 设置接收数据的处理函数和存放变量
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

        // 执行HTTP GET操作
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}else{
			string str_json = out.str();
			curl_easy_cleanup(curl);

			Json::Reader reader;
			Json::Value root;
			ofstream ofs;
			ofs.open(path);

			if(reader.parse(str_json.c_str(),root)){
				ofs << root.toStyledString() << endl;
				getCameraInfo();
				getNvrinfo();
				getCourseinfo();
				break;
			}
		}
	}
}

void ConfigRead::getCameraInfo(){
	ifstream fin;
	fin.open(path);
	Json::Reader reader;
	Json::Value root;
	reader.parse(fin, root, false);
	int cameraSize = root["cameraConfigList"].size();
	for(int i = 0;i<cameraSize;i++){
		Json::Value id = root["cameraConfigList"][i]["id"];
		Json::Value ip = root["cameraConfigList"][i]["cameraIp"];
		Json::Value account = root["cameraConfigList"][i]["cameraAccount"];
		Json::Value password = root["cameraConfigList"][i]["cameraPassword"];
		Json::Value nvrid = root["cameraConfigList"][i]["nvrID"];
		Json::Value port = root["cameraConfigList"][i]["nvrPort"];

		string Ip = ip.asString();
		int Id = id.asInt();
		string Account = account.asString();
		string Password = password.asString();
		int Nvrid = nvrid.asInt();
		int Port = port.asInt();

		cameraIp.push_back(Ip);
		cameraid.push_back(Id);
		cameraAccount.push_back(Account);
		cameraPassword.push_back(Password);
		cameraNvrID.push_back(Nvrid);
		cameraNvrPort.push_back(Port);
	}
}


void ConfigRead::getNvrinfo(){
	ifstream fin;
	fin.open(path);
	Json::Reader reader;
	Json::Value root;
	reader.parse(fin, root, false);
	int nvrSize = root["nvr"].size();
	for(int i = 0;i<nvrSize;i++){
		Json::Value id = root["nvr"][i]["id"];
		Json::Value ip = root["nvr"][i]["nvrIp"];
		Json::Value account = root["nvr"][i]["nvrAccount"];
		Json::Value password = root["nvr"][i]["nvrPassword"];
		int Id = id.asInt();
		string Ip = ip.asString();
		string Account = account.asString();
		string Password = password.asString();
		Nvrid.push_back(Id);
		NvrIp.push_back(Ip);
		NvrAccount.push_back(Account);
		NvrPassword.push_back(Password);
	}
}
void ConfigRead::getCourseinfo(){
	ifstream fin;
	fin.open(path);
	Json::Reader reader;
	Json::Value root;
	reader.parse(fin, root, false);
	std::vector<string> StartTime;
	std::vector<string> EndTime;
	std::vector<string> RestStartTime;
	std::vector<string> RestEndTime;
	int courseSize = root["coursePeriodTime"].size();
	for(int i = 0;i<courseSize;i++){
		Json::Value period = root["coursePeriodTime"][i]["periodNum"];
		Json::Value start = root["coursePeriodTime"][i]["startTime"];
		Json::Value end = root["coursePeriodTime"][i]["endTime"];
		Json::Value startrest = root["coursePeriodTime"][i]["restStartTime"];
		Json::Value endrest = root["coursePeriodTime"][i]["restEndTime"];
		int course = period.asInt();
		string Start = start.asString();
		string End = end.asString();
		string StartRest = startrest.asString();
		string EndRest = endrest.asString();
		PeriodNum.push_back(course);
		StartTime.push_back(Start);
		EndTime.push_back(End);
		RestStartTime.push_back(StartRest);
		RestEndTime.push_back(EndRest);
	}

	for (int i = 0; i < StartTime.size(); ++i)
	{
		vector<string> TempStartTime;
		vector<string> TempEndTime;
		vector<string> TempRestStartTime;
		vector<string> TempRestEndTime;
		boost::split(TempStartTime, StartTime[i], boost::is_any_of(":"), boost::token_compress_on);
		boost::split(TempEndTime, EndTime[i], boost::is_any_of(":"), boost::token_compress_on);
		boost::split(TempRestStartTime,RestStartTime[i], boost::is_any_of(":"), boost::token_compress_on);
		boost::split(TempRestEndTime, RestEndTime[i], boost::is_any_of(":"), boost::token_compress_on);
		for(int j = 0;j<TempStartTime.size();j++){
			if(j==0){
				if(TempStartTime[j][0]=='0'){
					int tempStartHour = TempStartTime[j][1]-'0';
					StartHour.push_back(tempStartHour);
				}else{
					int tempStartHour = atoi(TempStartTime[j].c_str());
					StartHour.push_back(tempStartHour);
				}
				if(TempEndTime[j][0]=='0'){
					int tempEndHour = TempEndTime[j][1]-'0';
					EndHour.push_back(tempEndHour);
				}else{
					int tempEndHour = atoi(TempEndTime[j].c_str());
					EndHour.push_back(tempEndHour);
				}
				if(TempRestStartTime[j][0]=='0'){
					int tempRestStartHour = TempRestStartTime[j][1]-'0';
					RestStartHour.push_back(tempRestStartHour);
				}else{
					int tempRestStartHour = atoi(TempRestStartTime[j].c_str());
					RestStartHour.push_back(tempRestStartHour);
				}
				if(TempRestEndTime[j][0]=='0'){
					int tempRestEndtHour = TempRestEndTime[j][1]-'0';
					RestEndHour.push_back(tempRestEndtHour);
				}else{
					int tempRestEndtHour = atoi(TempRestEndTime[j].c_str());
					RestEndHour.push_back(tempRestEndtHour);
				}
			}else{
				if(TempStartTime[j][0]=='0'){
					int temp = TempStartTime[j][1]-'0';
					StartMinute.push_back(temp);
				}else{
					int temp = atoi(TempStartTime[j].c_str());
					StartMinute.push_back(temp);
				}
				if(TempEndTime[j][0]=='0'){
					int temp= TempEndTime[j][1]-'0';
					EndMinute.push_back(temp);
				}else{
					int temp = atoi(TempEndTime[j].c_str());
					EndMinute.push_back(temp);
				}
				if(TempRestStartTime[j][0]=='0'){
					int temp = TempRestStartTime[j][1]-'0';
					RestStartMinute.push_back(temp);
				}else{
					int temp = atoi(TempRestStartTime[j].c_str());
					RestStartMinute.push_back(temp);
				}
				if(TempRestEndTime[j][0]=='0'){
					int temp = TempRestEndTime[j][1]-'0';
					RestEndMinute.push_back(temp);
				}else{
					int temp = atoi(TempRestEndTime[j].c_str());
					RestEndMinute.push_back(temp);
				}
			}
		}
	}
}

