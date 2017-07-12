#include <opencv2/core/core.hpp>
#include <vector>
#include "jfda.hpp"
using namespace std;


struct cluster
{
	int facenum;
	vector<FaceInfo> faceinfo;
};


struct PerFrame
{
	vector<long> frame;
	vector<float> rise;
	vector<float> down;
	vector<int> totalface;
};