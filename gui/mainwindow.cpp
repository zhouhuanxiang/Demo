#include "../dem.h"
#include "../dem1.h"
#include "mlibutil/mlibutil.h"

#include "mainwindow.h"

#include <QtGui>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Eigen/Eigen>
#include <thread>

MainWindow::MainWindow(ThreadManager *tmanager)
	:tmanager(tmanager), count1(frame_count_begin), count2(frame_count_begin)
{
	centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	timer1 = new QTimer(this);
	timer2 = new QTimer(this);
	glWidget = new MainWidget;
	//pixmapLabel = new QLabel(this);

	//QImage image = QImage(672, 380, QImage::Format_RGB888);
	//pixmapLabel->setPixmap(QPixmap::fromImage(image));
	//debugLabel = new QLabel(this);

	glWidgetArea = new QScrollArea(this);
	glWidgetArea->setWidget(glWidget);
	glWidgetArea->setWidgetResizable(true);
	glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	glWidgetArea->setFixedSize(672 + 10, 380 + 10);

	//pixmapLabelArea = new QScrollArea(this);
	//pixmapLabelArea->setWidget(pixmapLabel);
	//glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//pixmapLabelArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//pixmapLabelArea->setFixedSize(672 + 10, 380 + 10);

	QPushButton* startButton = new QPushButton("start", this);
	QPushButton* stopButton = new QPushButton("stop", this);

	QVBoxLayout *centralLayout = new QVBoxLayout;
	QHBoxLayout *ccentralLayout = new QHBoxLayout;
	QHBoxLayout *layout0 = new QHBoxLayout;
	QHBoxLayout *layout1 = new QHBoxLayout;

	layout1->addWidget(startButton);
	layout1->addWidget(stopButton);
	layout1->addStretch(1);

	//layout0->addWidget(pixmapLabelArea);
	layout0->addWidget(glWidgetArea);

	centralLayout->addLayout(layout1);
	centralLayout->addLayout(layout0);
	centralLayout->addStretch(1);
	ccentralLayout->addLayout(centralLayout);
	ccentralLayout->addStretch(1);
	centralWidget->setLayout(ccentralLayout);

	setWindowTitle(tr("Viewer"));
	resize(400, 300);

	connect(timer1, SIGNAL(timeout()), this, SLOT(UpdateImage1()));
	connect(timer2, SIGNAL(timeout()), this, SLOT(UpdateFace1()));

	connect(startButton, SIGNAL(clicked()), this, SLOT(StartTimer()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(StopTimer()));
}

void MainWindow::StartTimer()
{
	//timer1->start(30);
	timer2->start(20);
	dem_running = true;
}

void MainWindow::StopTimer()
{
	timer1->stop();
	timer2->stop();
	dem_running = false;
}

void MainWindow::UpdateImage()
{
	//char str[100];
	//sprintf(str, "F:/SunnuMars/zhx2/c%d.png", count);
	//cv::Mat m1, m2;
	//m1 = cv::imread(str);
	//cv::pyrDown(m1, m2, m1.size() / 2);
	//pixmapLabel->setPixmap(QPixmap::fromImage(QImage((unsigned char*)m2.data, m2.cols, m2.rows, QImage::Format_RGB888).rgbSwapped()));
	//sprintf(str, "Frame No.%d", count);
	//debugLabel->setText(str);
	//std::cout << str << "\n";
}

void MainWindow::UpdateFace()
{
	//char str[100];
	//sprintf(str, "C:/Users/zhx/Desktop/result/%d/e.obj", count1);
	//ml::MeshDatad mesh;
	//ml::MeshIOd::loadFromOBJ(str, mesh);

	//Eigen::VectorXf pos(vertex_size * 3);
	//for (int i = 0; i < vertex_size; i++) {
	//	for (int j = 0; j < 3; j++) {
	//		pos[3 * i + j] = mesh.m_Vertices[i][j];
	//	}
	//}

	//glWidget->updateFaceGeometry(pos);
}

void MainWindow::UpdateFace1()
{
	Eigen::MatrixXd m;

	std::unique_lock<std::mutex> lock(tmanager->local_expression_lock_);
	tmanager->local_new_expression_.wait(lock, 
		//1000ms,
		[this] { return (count1 != (tmanager->local_expression_count_)); });
	if (count1 != tmanager->local_expression_count_) {
		count1 = tmanager->local_expression_count_;
		m = tmanager->local_expression_;
		lock.unlock();
	}
	else {
		if(lock.owns_lock())
			lock.unlock();
		//return;
	}
	//m = tmanager->local_expression_;

	double angle_axis[6];
	memcpy(angle_axis, motion_param[frame_ptr_], 6 * sizeof(double));

	//for (int i = 0; i < 6; i++) {
	//	std::cout << angle_axis[i] << " ";
	//}
	//std::cout << "\n";

	QVector3D trans;
	QQuaternion Quat;
	trans = QVector3D(angle_axis[3], angle_axis[4], angle_axis[5]);
	float angle = std::sqrt(angle_axis[0] * angle_axis[0] + angle_axis[1] * angle_axis[1] + angle_axis[2] * angle_axis[2]);
	float s = std::sin(angle / 2) / angle;
	Quat = QQuaternion((float)std::cos(angle / 2), (float)angle_axis[0] * s, (float)angle_axis[1] * s, (float)angle_axis[2] * s);

	glWidget->updateFaceGeometry(m, trans, Quat);

	static int total = 0;
	std::cout << "@@@" << count1 << "\n";
}

void MainWindow::UpdateImage1()
{
	//pixmapLabel->setPixmap(QPixmap::fromImage(QImage((unsigned char*)cframes_[count % frame_size].data, cframe_width, cframe_height, QImage::Format_RGB888).scaled(cframe_width / 4, cframe_height / 4).rgbSwapped()));
	//char str[100];
	//sprintf(str, "Frame No.%d", count);
	//debugLabel->setText(str);
	//static int total = 0;
	//std::cout << "$$$" << ++total << "\n";

	if (count2 == frame_count_)
		return;
	count2 = frame_count_;

	cv::Mat tmp;
	double scale = 0.25;
	cv::resize(cframes_[frame_ptr_], tmp, cv::Size(), scale, scale);
	cv::imshow("Color", tmp);
	// Key Check
	const int key = cv::waitKey(1);

	std::cout << "###" << count2 << "\n";
}

void MainWindow::UpdateAll1()
{
	UpdateFace1();
	UpdateImage1();
}