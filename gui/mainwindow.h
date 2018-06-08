#pragma once
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

#include "mainwidget.h"

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
	MainWindow();

	QWidget *centralWidget;
	QScrollArea *glWidgetArea;
	QScrollArea *pixmapLabelArea;
	MainWidget *glWidget;
	QLabel *pixmapLabel;
	QLabel *debugLabel;
	
	QPushButton *startButton;
	QPushButton *stopButton;
	QPushButton *nextButton;
	QPushButton *prevButton;
	QPushButton *next10Button;
	QPushButton *prev10Button;

	QTimer *timer;
	int timerId;
	int count;

	public slots:
	void button_start();
	void button_stop();
	void button_next();
	void button_prev();
	void button_next10();
	void button_prev10();

	void UpdateAll();
	void UpdateFace();
	void UpdateImage();
protected:
	void timerEvent(QTimerEvent *event);
};