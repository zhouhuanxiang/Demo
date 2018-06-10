#ifndef GUI_THREAD_H_
#define GUI_THREAD_H_

#include "dem.h"
#include "mainwidget.h"
#include <QObject>
#include <QThread>


class GuiThread : public QThread
{
	Q_OBJECT
	//void run() override;

public:
	GuiThread(MainWidget *glWidget);
private:
	int gui_frame_count_;
	Eigen::MatrixXd gui_expression_eg_;

	MainWidget *glWidget;
};

#endif