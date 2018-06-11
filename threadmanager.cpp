#include "threadmanager.h"

ThreadManager::ThreadManager()
	:local_cframe_count_(0),
	local_expression_count_(0)
{
	DEM();

	std::thread t1(&ThreadManager::DemThread, this);
	t1.detach();

	std::thread t2(&ThreadManager::ExpressionThread, this);
	t2.detach();
}

void ThreadManager::DemThread()
{
	int base = 25;
	for (frame_count_ = base; frame_count_ <= base + 5; frame_count_++) {
		//LOG(INFO) << "\n\nframe No." << frame_count_;
		//std::cout << "# " << frame_count_ << "\n";
		UpdateFrame(true);
		Initialize();
	}
	UpdateNormalCPU();
	dem_init_done = true;

	std::chrono::steady_clock::time_point total_start = std::chrono::steady_clock::now();
	long long t1, t2;
	t1 = t2 = 0;
	for (frame_count_ = base + 6; frame_count_ <= frame_count_end;) {
		if (!dem_running) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}

		//LOG(INFO) << "\n\nframe No." << frame_count_;
		std::cout << "# " << frame_count_ << "\n";
		std::chrono::steady_clock::time_point tp1 = std::chrono::steady_clock::now();
		UpdateFrame(true);
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
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
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