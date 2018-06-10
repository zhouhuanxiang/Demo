#ifndef DEM_H_
#define DEM_H_

#include "ceres/ceres_param.h"
#include "ceres/ceres_initialization.h"
#include "ceres/ceres_track.h"
#include "ceres/ceres_motion.h"
#include "reader/model_reader.h"
#include "reader/image_reader_kinect.h"
#include "dlib/dlib_landmark_detector.h"

#include <Eigen/Eigen>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace Eigen;

extern bool dem_running;
extern bool dem_init_done;

extern long long track_time_;
extern long long track_time1_;
extern long long track_time2_;
extern long long track_time3_;
extern long long solve_time1_;
extern long long solve_time2_;
extern long long solve_time3_;
extern std::vector<double> face_path_;

extern MatrixXd M_eg_;
extern MatrixXd P_eg_;
extern MatrixXd M_eg_;
extern MatrixXd P_eg_;
extern MatrixXd delta_B1_eg_;
extern MatrixXd delta_B2_eg_;
extern MatrixXd delta_B_eg_;

extern double motion_param[frame_size][6];
extern Vector3d translation_eg_;
extern Matrix<double, 3, 3> rotation_eg_;
extern cv::Mat translation_cv_;
extern cv::Mat rotation_cv_;

extern MatrixXd x_coeff_eg_;
extern MatrixXd xx_coeff_eg_, xxx_coeff_eg_;
extern MatrixXd y_coeff_eg_;
extern VectorXd y_weights_eg_;

extern std::mutex expression_eg_lock_;
extern std::condition_variable  new_expression_eg_;
extern ml::MeshDatad mesh_;
extern std::vector<unsigned short> mesh_indices_;
extern MatrixXd neutral_eg_;
extern MatrixXd expression_eg_;	
extern MatrixXd normal_eg_;

// track
extern SparseMatrix<double> A_track_eg_;
extern MatrixXd C_track_eg_;

extern int frame_count_;
extern int frame_ptr_;
extern std::vector<cv::Mat> dframes_;
extern std::vector<cv::Mat> cframes_;
//extern cv::Mat dframe_;
//extern cv::Mat cframe_;
extern DepthCameraIntrinsic depth_camera_;
extern RgbCameraIntrinsic rgb_camera_;
extern Matrix<double, 3, 3> depth_camera_project_;
extern Matrix<double, 3, 3> depth_camera_reproject_;
extern Matrix<double, 3, 3> rgb_camera_project_;
//extern Matrix<double, 3, 3> rgb_camera_reproject_;

extern DlibLandmarkDetector landmark_detector_;
extern ImageReaderKinect image_reader;

void DEM();

Vector3d ReprojectionDepth(Vector2d p2, int depth);
Vector3d ReprojectionRgb(Vector2d p2, int depth);
Vector3d ProjectionDepth(Vector3d p3);
Vector3d ProjectionRgb(Vector3d p3);

void Initialize();

void TrackCeres();

void GenerateIcpMatrix();

void Track();
void EyeMouthTrack();
void EyeTrack();
void MouthTrack();

void UpdateNeutralFaceCPU();
void UpdateDeltaBlendshapeCPU();
void UpdateExpressionFaceCPU();
void UpdateNormalCPU();

void WriteNeutralFace(int count, MatrixXd tmesh);
void WriteExpressionFace(int count, MatrixXd tmesh, Vector3d translation_eg, Matrix<double, 3, 3> rotation_eg);
void WritePointCloud();

#endif