#pragma once

#include "../threadmanager.h"

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QPixmap>
#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QtConcurrent/QtConcurrent>

#include "mainwidget.h"
#include <opencv2/imgproc/imgproc.hpp>

class QWidget;
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
class QPixmap;
class MainWidget;
class QHorizonalLayout;
class QVerticalLayout;
class QTimer;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(ThreadManager *tmanager);

	QWidget *centralWidget;
	QScrollArea *glWidgetArea;
	//QScrollArea *pixmapLabelArea;
	MainWidget *glWidget;
	//QLabel *pixmapLabel;
	//QLabel *debugLabel;

	QTimer *timer1;
	QTimer *timer2;
	int count1;
	int count2;

signals:

	public slots:
	//
	void UpdateFace();
	void UpdateImage();
	//
	void UpdateFace1();
	void UpdateImage1();
	void UpdateAll1();
	//
	void StartTimer();
	void StopTimer();

protected:

private:
	ThreadManager *tmanager;
};