#pragma once

#include <cstdio>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

namespace common {
  cv::Mat CropPatch(const cv::Mat& img, cv::Rect& bbox);
}  
