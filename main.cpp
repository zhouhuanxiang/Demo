#include <ceres/ceres.h>
#include <ceres/rotation.h>
#include <glog/logging.h>

#include <chrono>
#include <fstream>
#include "dem.h"
#include "dem1.h"
#include <direct.h>

#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>

#ifndef QT_NO_OPENGL
#include "gui/mainwidget.h"
#include "gui/mainwindow.h"
#endif

int main(int argc, char *argv[])
{
	Eigen::initParallel();
	Eigen::setNbThreads(128);

	FLAGS_logtostderr = false;
	google::SetLogDestination(google::GLOG_INFO, "C:/Users/zhx/Desktop/log/log");
	google::SetLogDestination(google::GLOG_WARNING, "C:/Users/zhx/Desktop/log/warning");
	google::InitGoogleLogging(argv[0]);

	QApplication app(argc, argv);
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);
	app.setApplicationName("Face Animation");
	app.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL
	MainWindow window;
	window.show();
#else
	QLabel note("OpenGL Support required");
	note.show();
#endif

	app.exec();
	return 0;
}
