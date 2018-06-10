#ifndef DEM1_H_
#define DEM2_H_

#include "dem.h"

void UpdateMotion(cv::Mat &dframe, std::vector<Eigen::Vector2d> pts, MatrixXd expression_eg, int xmin, int xmax, int ymin, int ymax);
bool UpdateFrame(bool force_motion);

#endif