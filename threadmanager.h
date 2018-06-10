#ifndef THREAD_MANAGER_H_
#define THREAD_MANAGER_H_

#include "dem.h"
#include "dem1.h"

#include <mutex>
#include <thread>
#include <iostream>

class ThreadManager
{
public:
	ThreadManager();

	void DemThread();

	void ImageThread();

	void ExpressionThread();

public:
	int local_cframe_count_;
	std::mutex local_expression_lock_;
	std::condition_variable local_new_expression_;
	int local_expression_count_;
	Eigen::MatrixXd local_expression_;
};

#endif // !THREAD_MANAGER_H_
