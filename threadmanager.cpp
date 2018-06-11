#include "threadmanager.h"

#define ROUND_UP(x, align) (x+(align-1))&(~(align-1))

ThreadManager::ThreadManager()
	:local_cframe_count_(0),
	local_expression_count_(0)
{
	DEM();

	base = 25;

#if REAL_MODE
	std::thread t0(&ThreadManager::CameraThread, this);
	t0.detach();
#else
#endif

	std::thread t1(&ThreadManager::DemInitThread, this);
	t1.detach();

	std::thread t2(&ThreadManager::ExpressionThread, this);
	t2.detach();
}

void ThreadManager::DemInitThread()
{
#if REAL_MODE
	while (raw_frame_count_ < 100) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		continue;
	}
	for (int i = 0; i < 10;) {
		if (!dem_running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}
		bool result = UpdateFrame(true);
		if (!result) {
			continue;
		}
		Initialize();
		i++;
	}
#else
	for (frame_count_ = base; frame_count_ <= base + 5;) {
		if (!dem_running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}
		frame_count_++;
		//LOG(INFO) << "\n\nframe No." << frame_count_;
		//std::cout << "# " << frame_count_ << "\n";
		bool result = UpdateFrame(true);
		if (!result) {
			std::cout << "wrong";
			base++;
			continue;
		}
		Initialize();
	}
	cv::destroyAllWindows();
#endif
	UpdateNormalCPU();
	dem_init_done = true;

	std::thread t1(&ThreadManager::DemThread, this);
	t1.detach();
}

void ThreadManager::DemThread()
{
#if REAL_MODE
	while(true){
		if (!dem_running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}
		if (!UpdateFrame(true))
			continue;
		Track();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
#else
	std::chrono::steady_clock::time_point total_start = std::chrono::steady_clock::now();
	long long t1, t2;
	t1 = t2 = 0;
	for (frame_count_ = base + 6; frame_count_ <= frame_count_end;) {
		if (!dem_running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}
		std::cout << "# " << frame_count_ << "\n";
		std::chrono::steady_clock::time_point tp1 = std::chrono::steady_clock::now();
		if (!UpdateFrame(true))
			continue;
		std::chrono::steady_clock::time_point tp2 = std::chrono::steady_clock::now();
		Track();
		//TrackCeres();
		//Refine();
		std::chrono::steady_clock::time_point tp3 = std::chrono::steady_clock::now();
		frame_count_ += 1;

		t1 += std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
		t2 += std::chrono::duration_cast<std::chrono::milliseconds>(tp3 - tp2).count();

		//std::thread t(&MainWindow::UpdateAll, mainwindow);
		//t.detach();
		//mainwindow->UpdateAllProxy(cframes_[frame_ptr_], expression_eg_);
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	std::chrono::steady_clock::time_point total_end = std::chrono::steady_clock::now();
	std::cout << "\nsummary\n";
	std::cout << "frame   :" << t1 << "ms\n";
	std::cout << "  motion:" << track_time_ << "ms\n";
	std::cout << "track   :" << t2 << "ms\n";
	std::cout << "  step1 :" << track_time1_ << "ms\n";
	std::cout << "  step2 :" << track_time2_ << "ms\n";
	std::cout << "  step21:" << solve_time1_ << "ms\n";
	std::cout << "  step22:" << solve_time2_ << "ms\n";
	std::cout << "  step3 :" << track_time3_ << "ms\n";
	std::cout << "total   :" << std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count() << "ms\n";
#endif
}

void ThreadManager::ImageThread()
{

}

void ThreadManager::ExpressionThread()
{
	while (true) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(30));

		Eigen::MatrixXd tmp;
		std::unique_lock<std::mutex> lock1(expression_eg_lock_);
		new_expression_eg_.wait(lock1, [this] { return (local_expression_count_ != frame_count_); });
		tmp = expression_eg_;
		lock1.unlock();

		{	
			std::lock_guard<std::mutex> lock2(local_expression_lock_);
			local_expression_ = tmp;
			local_expression_count_ = frame_count_;
		}

		local_new_expression_.notify_all();
	}
}

void ThreadManager::CameraThread()
{
	std::shared_ptr<CLibTof> libtof;
	libtof = shared_ptr<CLibTof>(CLibTof::LibTOF_NewInstance());
	if (libtof->LibTOF_Connect() < 0) {
		throw std::runtime_error("LibTOF_Connect failed");
	};
	DeviceInfo_t info;
	memset(&info, 0, sizeof(decltype(info)));
	if (libtof->LibTOF_GetDeviceInfo(&info) < 0) {
		throw std::runtime_error("LibTOF_GetlibtofInfo failed");
	}
	libtof->LibTOF_RgbdEnSet((bool)(info.RgbdEn));
	libtof->LibTOF_SetUseCase(MODE_3_15FPS_850);

	//
	int DEPTHMAP_W = 224;
	int DEPTHMAP_H = 172;
	int DEPTHVIDEO_W = 1344;
	int DEPTHVIDEO_H = 760;
	//
	int DEPTHVIDEO_FRAME_SIZE = ROUND_UP(DEPTHVIDEO_W*DEPTHVIDEO_H * 3 / 2 + DEPTHVIDEO_W, 1024);
	//
	unsigned char* PColorbuffer_s = (unsigned char *)malloc(DEPTHMAP_W*DEPTHMAP_H * 4);
	FrameData_t* frame_data = (FrameData_t*)malloc(sizeof(FrameData_t)*DEPTHMAP_W*DEPTHMAP_H + DEPTHMAP_W);
	memset(frame_data, 0, sizeof(FrameData_t)*DEPTHMAP_W*DEPTHMAP_H + DEPTHMAP_W);
	FrameDataRgb_t* frame_data_Rgb = (FrameDataRgb_t*)malloc(sizeof(FrameDataRgb_t)*DEPTHMAP_W*DEPTHMAP_H + DEPTHMAP_W);
	memset(frame_data_Rgb, 0, sizeof(FrameDataRgb_t)*DEPTHMAP_W*DEPTHMAP_H + DEPTHMAP_W);
	unsigned char* frame_data_gray = (unsigned char*)malloc(DEPTHMAP_W*DEPTHMAP_H * 2 + DEPTHMAP_W);
	memset(frame_data_gray, 0, DEPTHMAP_W*DEPTHMAP_H * 2 + DEPTHMAP_W);
	short * g_Video_data = (short *)malloc(DEPTHVIDEO_FRAME_SIZE);
	//
	FrameData_t* frame_data_p;
	FrameDataRgb_t* frame_data_Rgb_tmp;
	unsigned char* PColorbuffer_d;
	unsigned char* frame_data_gray_tmp;
	while (true)
	{
		frame_data_p = frame_data;
		frame_data_Rgb_tmp = frame_data_Rgb;
		PColorbuffer_d = PColorbuffer_s;
		frame_data_gray_tmp = frame_data_gray;

		int rs = libtof->LibTOF_RcvDepthFrame(frame_data_p, frame_data_Rgb_tmp, DEPTHMAP_W, DEPTHMAP_H, frame_data_gray_tmp);
		cv::Mat cv_depth(DEPTHMAP_H, DEPTHMAP_W, CV_32FC3, frame_data_p);
		//color
		rs = libtof->LibTOF_RcvVideoFrame(g_Video_data, DEPTHVIDEO_FRAME_SIZE);//目前获取到的数据格式为YUYV
		cv::Mat cv_yv2(DEPTHVIDEO_H + DEPTHVIDEO_H / 2, DEPTHVIDEO_W, CV_8UC1, g_Video_data);
		cv::Mat cv_rgb;
		cv::cvtColor(cv_yv2, cv_rgb, CV_YUV2RGB_YV12);

		cv::Mat resizeMat;
		const double scale = 0.25;
		cv::resize(cv_rgb, resizeMat, cv::Size(), scale, scale);
		cv::imshow("Color", resizeMat);
		const int key = cv::waitKey(10);

		raw_frame_ptr_ = (raw_frame_count_ + 1)% raw_frame_size;
		cv_rgb.copyTo(raw_cframes_[raw_frame_ptr_]);
		cv_depth.copyTo(raw_dframes_[raw_frame_ptr_]);
		raw_frame_count_++;
	}
}