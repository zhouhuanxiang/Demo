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

#include "../parameters.h"
#include "../dem.h"
#include "mlibutil/mlibutil.h"
#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>

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

	local_pos_.resize(3 * vertex_size);
	local_nor_.resize(3 * vertex_size);

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

void GeometryEngine::setNormal(QOpenGLShaderProgram *program)
{
	normalBuf.bind();
	normalBuf.allocate(normal_eg_f_.data(), vertex_size * 3 * sizeof(float));
	//int normalLocation = program->attributeLocation("a_normal");
	//program->enableAttributeArray(normalLocation);
	//normalBuf.bind();
	//program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(float) * 3);

	local_nor_ = normal_eg_f_;
}

void GeometryEngine::setConstant(QOpenGLShaderProgram *program)
{
	indexBuf.bind();
	indexBuf.allocate(mesh_indices_.data(), face_size * 3 * sizeof(GLushort));

	mesh_.computeVertexNormals();
	std::cout << mesh_.m_Normals.size() << "\n";
	for (int i = 0; i < vertex_size; i++) {
		for (int j = 0; j < 3; j++) {
			local_nor_(3 * i + j) = -1 * mesh_.m_Normals[i][j];
		}
	}
	normalBuf.bind();
	normalBuf.allocate(local_nor_.data(), vertex_size * 3 * sizeof(float));
	//int normalLocation = program->attributeLocation("a_normal");
	//program->enableAttributeArray(normalLocation);
	//normalBuf.bind();
	//program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(float) * 3);
}

void GeometryEngine::updateFaceGeometry(Eigen::MatrixXd &pos)
{
	if (local_pos_.rows() != 3 * vertex_size || pos.rows() != 3 * vertex_size)
		return;

	for (int i = 0; i < 3 * vertex_size; i++) {
		local_pos_(i) = pos(i, 0);
	}

//	Eigen::Map<VectorXf> nor_map(local_nor_.data(), 3, vertex_size);
//	Eigen::Map<VectorXf> exp_map(local_pos_.data(), 3, vertex_size);
//	nor_map.setZero();
//#pragma omp parallel for
//	for (int f = 0; f < (int)mesh_.m_FaceIndicesVertices.size(); f++) {
//		ml::MeshDatad::Indices::Face &ind = mesh_.m_FaceIndicesVertices[f];
//		Vector3f vec1 = exp_map.col(ind[1]) - exp_map.col(ind[0]);
//		Vector3f vec2 = exp_map.col(ind[2]) - exp_map.col(ind[0]);
//		Vector3f n = vec2.cross(vec1).normalized();
//		nor_map.col(ind[0]) += n;
//		nor_map.col(ind[1]) += n;
//		nor_map.col(ind[2]) += n;
//	}
	//for (int i = 0; i < vertex_size; i++) {
	//	local_nor_(i * 3 + 0) = 0;
	//	local_nor_(i * 3 + 1) = 0;
	//	local_nor_(i * 3 + 2) = 1;
	//}

	//! [1]
	// Transfer vertex data to VBO 0
	//arrayBuf.bind();
	//arrayBuf.allocate(vertices, vertex_size * 6 * sizeof(float));
	positionBuf.bind();
	positionBuf.allocate(local_pos_.data(), vertex_size * 3 * sizeof(float));
	//normalBuf.bind();
	//normalBuf.allocate(local_nor_.data(), vertex_size * 3 * sizeof(float));

	// Transfer index data to VBO 1
	//static bool first = true;
	//if (first) {
	//	first = false;
	//	indexBuf.bind();
	//	indexBuf.allocate(mesh_indices_.data(), face_size * 3 * sizeof(GLushort));
	//}
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
	glDrawElements(GL_TRIANGLES, face_size * 3 * sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}
