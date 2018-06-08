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

#include "mlibutil/mlibutil.h"
#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>


#define VERTEX_SIZE 3223
#define FACE_SIZE 6226

struct VertexData
{
	QVector3D position;
};

//! [0]
GeometryEngine::GeometryEngine()
	: indexBuf(QOpenGLBuffer::IndexBuffer)
{
	initializeOpenGLFunctions();

	// Generate 2 VBOs
	arrayBuf.create();
	positionBuf.create();
	normalBuf.create();
	indexBuf.create();

	// Initializes cube geometry and transfers it to VBOs
	//updateFaceGeometry();
}

GeometryEngine::~GeometryEngine()
{
	arrayBuf.destroy();
	positionBuf.destroy();
	normalBuf.destroy();
	indexBuf.destroy();
}
//! [0]

void GeometryEngine::updateFaceGeometry(Eigen::VectorXf &pos, Eigen::VectorXf &nor, Eigen::VectorXf &ind)
{
	GLushort indices[FACE_SIZE * 3];
	for (int i = 0; i < FACE_SIZE; i++) {
		//auto& face = mesh.m_FaceIndicesVertices[i];
		indices[i * 3 + 0] = ind[3 * i + 0];
		indices[i * 3 + 1] = ind[3 * i + 1];
		indices[i * 3 + 2] = ind[3 * i + 2];
	}

	//! [1]
	// Transfer vertex data to VBO 0
	//arrayBuf.bind();
	//arrayBuf.allocate(vertices, VERTEX_SIZE * 6 * sizeof(float));
	positionBuf.bind();
	arrayBuf.allocate(pos.data(), VERTEX_SIZE * 3 * sizeof(float));
	normalBuf.bind();
	arrayBuf.allocate(nor.data(), VERTEX_SIZE * 3 * sizeof(float));

	// Transfer index data to VBO 1
	indexBuf.bind();
	indexBuf.allocate(indices, FACE_SIZE * 3 * sizeof(GLushort));
	//! [1]
}

void GeometryEngine::drawFaceGeometry(QOpenGLShaderProgram *program)
{
	// Tell OpenGL programmable pipeline how to locate vertex position data
	int vertexLocation = program->attributeLocation("a_position");
	program->enableAttributeArray(vertexLocation);
	positionBuf.bind();
	program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(float) * 3);

	int normalLocation = program->attributeLocation("a_normal");
	program->enableAttributeArray(normalLocation);
	normalBuf.bind();
	program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(float) * 3);

	// Draw cube geometry using indices from VBO 1
	indexBuf.bind();
	glDrawElements(GL_TRIANGLES, FACE_SIZE * 3 * sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}
