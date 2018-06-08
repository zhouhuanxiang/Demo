#include "../dem.h"
#include "../dem1.h"
#include "mlibutil/mlibutil.h"

#include "mainwindow.h"

#include <QtGui>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Eigen/Eigen>

#define START_FRAME 25
#define END_FRAME 390

MainWindow::MainWindow()
{
	centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	timer = new QTimer(this);
	glWidget = new MainWidget;
	pixmapLabel = new QLabel(this);

	//cv::Mat m1, m2;
	//m1 = cv::imread("F:/SunnuMars/zhx4/c15.png");
	//cv::pyrDown(m1, m2, m1.size() / 2);
	//pixmapLabel->setPixmap(QPixmap::fromImage(QImage((unsigned char*)m2.data, m2.cols, m2.rows, QImage::Format_RGB888).rgbSwapped()));
	QImage image = QImage(672, 380, QImage::Format_RGB888);
	pixmapLabel->setPixmap(QPixmap::fromImage(image));
	debugLabel = new QLabel(this);

	startButton = new QPushButton("start");
	stopButton = new QPushButton("stop");
	nextButton = new QPushButton("next");
	prevButton = new QPushButton("prev");
	next10Button = new QPushButton("next 10");
	prev10Button = new QPushButton("prev 10");

	glWidgetArea = new QScrollArea(this);
	glWidgetArea->setWidget(glWidget);
	glWidgetArea->setWidgetResizable(true);
	glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	glWidgetArea->setFixedSize(672 + 10, 380 + 10);

	pixmapLabelArea = new QScrollArea(this);
	pixmapLabelArea->setWidget(pixmapLabel);
	glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pixmapLabelArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	pixmapLabelArea->setFixedSize(672 + 10, 380 + 10);

	QVBoxLayout *centralLayout = new QVBoxLayout;
	QHBoxLayout *ccentralLayout = new QHBoxLayout;
	QHBoxLayout *upLayout1 = new QHBoxLayout;
	QHBoxLayout *upLayout2 = new QHBoxLayout;
	QHBoxLayout *upLayout3 = new QHBoxLayout;
	QHBoxLayout *upLayout4 = new QHBoxLayout;
	QHBoxLayout *buttomLayout = new QHBoxLayout;
	upLayout1->addWidget(debugLabel);
	upLayout1->addStretch(1);
	upLayout2->addWidget(startButton);
	upLayout2->addWidget(stopButton);
	upLayout2->addStretch(1);
	upLayout3->addWidget(prevButton);
	upLayout3->addWidget(nextButton);
	upLayout3->addWidget(prev10Button);
	upLayout3->addWidget(next10Button);
	upLayout3->addStretch(1);
	buttomLayout->addWidget(pixmapLabelArea);
	buttomLayout->addWidget(glWidgetArea);

	centralLayout->addLayout(upLayout1);
	centralLayout->addLayout(upLayout2);
	centralLayout->addLayout(upLayout3);
	centralLayout->addLayout(upLayout4);
	centralLayout->addLayout(buttomLayout);
	centralLayout->addStretch(1);
	ccentralLayout->addLayout(centralLayout);
	ccentralLayout->addStretch(1);
	centralWidget->setLayout(ccentralLayout);

	setWindowTitle(tr("Viewer"));
	resize(400, 300);

	connect(startButton, SIGNAL(clicked()), this, SLOT(button_start()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(button_stop()));
	connect(nextButton, SIGNAL(clicked()), this, SLOT(button_next()));
	connect(prevButton, SIGNAL(clicked()), this, SLOT(button_prev()));
	connect(next10Button, SIGNAL(clicked()), this, SLOT(button_next10()));
	connect(prev10Button, SIGNAL(clicked()), this, SLOT(button_prev10()));
	connect(timer, SIGNAL(timeout()), this, SLOT(button_next()));

	count = START_FRAME;
}

void MainWindow::button_start()
{
	//timer->start(50);
	DEM();

	int base = 25;
	for (frame_count_ = base; frame_count_ <= base + 5; frame_count_++) {
		//LOG(INFO) << "\n\nframe No." << frame_count_;
		//std::cout << "# " << frame_count_ << "\n";
		UpdateFrame(true);
		Initialize();
	}

	std::chrono::steady_clock::time_point total_start = std::chrono::steady_clock::now();
	long long t1, t2;
	t1 = t2 = 0;
	for (frame_count_ = base + 6; frame_count_ <= frame_count_end;) {
		LOG(INFO) << "\n\nframe No." << frame_count_;
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

void MainWindow::button_stop()
{
	timer->stop();
}

void MainWindow::button_next()
{
	count++;
	count = std::min(END_FRAME, count);

	UpdateAll();
}

void MainWindow::button_prev()
{
	count--;
	count = std::max(START_FRAME, count);

	UpdateAll();
}

void MainWindow::button_next10()
{
	count += 10;
	count = std::min(END_FRAME, count);

	UpdateAll();
}

void MainWindow::button_prev10()
{
	count -= 10;
	count = std::max(15, count);

	UpdateAll();
}

void MainWindow::UpdateAll()
{
	UpdateImage();
	UpdateFace();
}

void MainWindow::UpdateImage()
{
	char str[100];
	sprintf(str, "F:/SunnuMars/zhx4/c%d.png", count);
	cv::Mat m1, m2;
	m1 = cv::imread(str);
	cv::pyrDown(m1, m2, m1.size() / 2);
	pixmapLabel->setPixmap(QPixmap::fromImage(QImage((unsigned char*)m2.data, m2.cols, m2.rows, QImage::Format_RGB888).rgbSwapped()));
	sprintf(str, "Frame No.%d", count);
	debugLabel->setText(str);
}

void MainWindow::UpdateFace()
{
	char str[100];
	sprintf(str, "C:/Users/zhx/Desktop/result/%d/e.obj", count);
	ml::MeshDatad mesh;
	ml::MeshIOd::loadFromOBJ(str, mesh);
	mesh.computeVertexNormals();

	int vsize = mesh.m_Vertices.size();
	int fsize = mesh.m_FaceIndicesVertices.size();
	Eigen::VectorXf pos(vsize * 3);
	Eigen::VectorXf nor(vsize * 3);
	Eigen::VectorXf ind(fsize * 3);
	for (int i = 0; i < vsize; i++) {
		for (int j = 0; j < 3; j++) {
			pos[3 * i + j] = mesh.m_Vertices[i][j];
			nor[3 * i + j] = mesh.m_Normals[i][j] * -1;
		}
	}
	for (int i = 0; i < fsize; i++) {
		auto& face = mesh.m_FaceIndicesVertices[i];
		for (int j = 0; j < 3; j++) {
			ind[i * 3 + j] = face[j];
		}
	}

	glWidget->updateFaceGeometry(pos, nor, ind);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	count++;
	count = std::min(END_FRAME, count);

	UpdateAll();
}