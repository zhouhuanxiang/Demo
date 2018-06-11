/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>
#include <iostream>

#include <math.h>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0)
{
	local_pos_.resize(3 * vertex_size, 1);
	local_pos_.setZero();
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete geometries;
    doneCurrent();

	//program = new QOpenGLShaderProgram;
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(72 / 255.0, 49 / 255.0, 79 / 255.0, 0.1);

    initShaders();
//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

	geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

//! [3]
void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}
//! [3]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 100.0, zFar = 1000.0, fov = 45.0;

    // Reset projection
	projection.setToIdentity();
    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
	//
	view.lookAt(QVector3D(0, 0, 0), QVector3D(0, 0, 1), QVector3D(0, 1, 0));
	//
	QVector3D lightPos(0, 0, 0);
	program.setUniformValue("lightPos", lightPos);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//! [6]
    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * view * model);
//! [6]

	static bool first = true;
	if (first) {
		geometries->setConstant(&program);
		first = false;
	}
	if (dem_init_done) {
		geometries->setNormal(&program);
		dem_init_done = false;
	}
	geometries->updateFaceGeometry(local_pos_);

	geometries->drawFaceGeometry(&program);
}

void MainWidget::updateFaceGeometry(Eigen::MatrixXd &pos, QVector3D &trans, QQuaternion &Quat)
{
	// Calculate model view transformation
	model.setToIdentity();
	//model.translate(QVector3D(0, 0, 500));
	model.translate(trans);
	//model.rotate(Quat);
	//model.rotate(10, QVector3D(0, 1, 0));

	//static bool first = true;
	//if (first) {
	//	geometries->setConstant(&program);
	//	first = false;
	//}
	//if (dem_init_done) {
	//	geometries->setNormal(&program);
	//	dem_init_done = false;
	//}
	//geometries->updateFaceGeometry(pos);

	local_pos_ = pos;

	update();
}
