#ifndef THREAD_MANAGER_H_
#define THREAD_MANAGER_H_

#include "dem.h"
#include "dem1.h"

#include "libTof.h"

#include <mutex>
#include <thread>
#include <iostream>

class ThreadManager
{
public:
	ThreadManager();

	void DemInitThread();

	void DemThread();

	void ImageThread();

	void ExpressionThread();

	void CameraThread();

public:
	int local_cframe_count_;
	std::mutex local_expression_lock_;
	std::condition_variable local_new_expression_;
	int local_expression_count_;
	Eigen::MatrixXd local_expression_;

	int base;
};

#endif // !THREAD_MANAGER_H_
