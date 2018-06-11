#include "dem1.h"
	
void UpdateMotion(cv::Mat &dframe, std::vector<Eigen::Vector2d> pts,
	MatrixXd expression_eg,
	int xmin, int xmax, int ymin, int ymax)
{
	//LOG(INFO) << "update motion start";
	std::chrono::steady_clock::time_point tp1 = std::chrono::steady_clock::now();
	ceres::Problem problem1;
	ceres::Solver::Options options1;
	options1.linear_solver_type = ceres::DENSE_QR;
	options1.minimizer_progress_to_stdout = false;
	options1.max_num_iterations = 500;
	options1.num_threads = 1;
	ceres::LossFunctionWrapper* loss_function_wrapper1 = new ceres::LossFunctionWrapper(new ceres::HuberLoss(1.0), ceres::TAKE_OWNERSHIP);

	double tmp_motion[6];
	memcpy(tmp_motion, motion_param[(frame_ptr_ - 1 + frame_size) % frame_size], 6 * sizeof(double));
	for (int i = 27; i <= 47; i++) {
		problem1.AddResidualBlock(
			CeresMotionLandmarkError::Create(dframe,
				pts[i],
				expression_eg.block(3 * face_landmark[i], 0, 3, 1),
				xmin, xmax, ymin, ymax),
			loss_function_wrapper1,
			tmp_motion, tmp_motion + 3
		);
	}
	problem1.AddResidualBlock(
		CeresMotionSmoothError::Create(motion_param[(frame_ptr_ - 1 + frame_size) % frame_size],
			motion_param[(frame_ptr_ - 2 + frame_size) % frame_size]),
		0,
		tmp_motion, tmp_motion + 3
	);

	ceres::Solver::Summary summary1;
	ceres::Solve(options1, &problem1, &summary1);
	std::chrono::steady_clock::time_point tp2 = std::chrono::steady_clock::now();
	track_time_ += std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
	//LOG(INFO) << "update motion end";

	double *p = motion_param[frame_ptr_];
	double *prev = motion_param[(frame_ptr_ - 1 + frame_size) % frame_size];
	double *pprev = motion_param[(frame_ptr_ - 2 + frame_size) % frame_size];
	double *ppprev = motion_param[(frame_ptr_ - 3 + frame_size) % frame_size];
	for (int i = 0; i < 6; i++) {
		//p[i] = tmp_motion[i];
		p[i] = tmp_motion[i] * 0.6 + prev[i] * 0.4;
	}

	//for (int i = 0; i < 6; i++) {
	//	std::cout << motion_param_tmp[i] << " ";
	//}
	//std::cout << "@update\n";

	//ceres::Problem problem2;
	//for (int i = 0; i < vertex_size; i += 25) {
	//	problem1.AddResidualBlock(
	//		CeresMotionDenseError::Create(dframe_,
	//			Vector2d(0, 0),
	//			expression_eg_.block(3 * i, 0, 3, 1),
	//			landmark_detector_.xmin, landmark_detector_.xmax, landmark_detector_.ymin, landmark_detector_.ymax),
	//		0,
	//		motion_param, motion_param + 3
	//	);
	//}
	//ceres::Solve(options1, &problem1, &summary1);

	//for (int i = 0; i < vertex_size; i += 25) {
	//	CeresMotionDenseError error = CeresMotionDenseError(dframe_,
	//		Vector2d(0, 0),
	//		expression_eg_.block(3 * i, 0, 3, 1),
	//		landmark_detector_.xmin, landmark_detector_.xmax, landmark_detector_.ymin, landmark_detector_.ymax);
	//	double residuals;
	//	error(motion_param, motion_param + 3, &residuals);
	//	//std::cout << setw(15) << residuals << "\n";
	//	////LOG(INFO) << setw(15) << residuals;
	//}

	//for (int i = 27; i <= 47; i++) {
	//	CeresMotionLandmarkError error = CeresMotionLandmarkError(dframe_,
	//		landmark_detector_.pts_[i],
	//		expression_eg_.block(3 * face_landmark[i], 0, 3, 1),
	//		landmark_detector_.xmin, landmark_detector_.xmax, landmark_detector_.ymin, landmark_detector_.ymax);
	//	double residuals[3];
	//	error(motion_param, motion_param + 3, residuals);
	//	//std::cout << setw(15) << residuals[0] << " " << setw(15) << residuals[1] << " " << setw(15) << residuals[2] << "\n";
	//	////LOG(INFO) << setw(15) << residuals[0] << " " << setw(15) << residuals[1] << "\n";
	//}

	Ceres2Eigen(rotation_eg_, translation_eg_, motion_param[frame_ptr_]);
	////LOG(INFO) << "translation: " << Map<RowVectorXd>(translation_eg_.data(), 3);
	//std::cout << "translation: " << Map<RowVectorXd>(translation_eg_.data(), 3) << "@\n";
}

bool UpdateFrame(bool force_motion)
{
	frame_ptr_ = frame_count_ % frame_size;
	//static ImageReaderKinect image_reader(Kinect_Data_Dir);
	image_reader.GetFrame(frame_count_, cframes_[frame_ptr_], dframes_[frame_ptr_]);
	//return true;
	landmark_detector_.Detect(cframes_[frame_ptr_], frame_count_, false);
	//return true;

	UpdateMotion(dframes_[frame_ptr_],
		landmark_detector_.pts_,
		expression_eg_,
		landmark_detector_.xmin,
		landmark_detector_.xmax,
		landmark_detector_.ymin,
		landmark_detector_.ymax);
	face_path_.push_back(motion_param[frame_ptr_][3]);
	face_path_.push_back(motion_param[frame_ptr_][4]);
	face_path_.push_back(motion_param[frame_ptr_][5]);

	return true;
}