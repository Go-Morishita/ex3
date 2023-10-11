//
//  main.cpp
//
//  Created by Yonghao Yue on 2019/09/28.
//  Updated: 2021/10/13
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

/*
* 青山学院大学 理工学部情報テクノロジー学科
* デジタルメディア設計演習第3回演習課題2
* 共同制作 森下剛・堀田大智・中江朋弘
* 解答:
*
*
*
*
*
*/

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#define EIGEN_DONT_VECTORIZE

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#include <unistd.h>
#else
#include <GL/freeglut.h>
#include <Windows.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include "Camera.h"

double g_FrameSize_WindowSize_Scale_x = 1.0;
double g_FrameSize_WindowSize_Scale_y = 1.0;

int width = 640;
int height = 480;
int mx, my;

Camera g_Camera;

#define MAX 40000

Eigen::Vector3d g_PointLightPos_Blue{ 2.0, 7.0, 2.0 };
double g_LightIntensity_Blue = 38.0;

Eigen::Vector3d g_PointLightPos_Yellow{ 2.0, 0.5, 2.0 };
double g_LightIntensity_Yellow = 37.0;

void drawShadedTriangle(const Eigen::Vector3d& in_p1, const Eigen::Vector3d& in_p2, const Eigen::Vector3d& in_p3, const Eigen::Vector3d& in_kd_Blue, const Eigen::Vector3d& in_kd_Yellow) {
	const Eigen::Vector3d in_center = (in_p1 + in_p3) / 2;
	const Eigen::Vector3d in_arm_u = (in_p3 - in_p2) / 2;
	const Eigen::Vector3d in_arm_v = (in_p2 - in_p1) / 2;

	const Eigen::Vector3d p_xmym = in_center - in_arm_u - in_arm_v;
	const Eigen::Vector3d p_xpym = in_center + in_arm_u - in_arm_v;
	const Eigen::Vector3d p_xmyp = in_center - in_arm_u + in_arm_v;
	const Eigen::Vector3d p_xpyp = in_center + in_arm_u + in_arm_v;

	Eigen::Vector3d n = in_arm_u.cross(in_arm_v);
	n.normalize();

	//青の光源について
	Eigen::Vector3d L_Blue = g_PointLightPos_Blue - in_center;
	const double dist_Blue = L_Blue.norm();
	L_Blue.normalize();
	const double cos_theta_Blue = std::max<double>(0.0, L_Blue.dot(n));


	const Eigen::Vector3d L_o_Blue = in_kd_Blue * g_LightIntensity_Blue * cos_theta_Blue / (dist_Blue * dist_Blue);

	//黄色の光源について
	Eigen::Vector3d L_Yellow = g_PointLightPos_Yellow - in_center;
	const double dist_Yellow = L_Yellow.norm();
	L_Yellow.normalize();
	const double cos_theta_Yellow = std::max<double>(0.0, L_Yellow.dot(n));


	const Eigen::Vector3d L_o_Yellow = in_kd_Yellow * g_LightIntensity_Yellow * cos_theta_Yellow / (dist_Yellow * dist_Yellow);

	//観測される輝度:青、黄色の和
	const Eigen::Vector3d L_o = L_o_Blue + L_o_Yellow;

	glBegin(GL_TRIANGLES);

	glColor3f(L_o.x(), L_o.y(), L_o.z());

	glVertex3f(p_xmym.x(), p_xmym.y(), p_xmym.z());
	glVertex3f(p_xpym.x(), p_xpym.y(), p_xpym.z());
	glVertex3f(p_xmyp.x(), p_xmyp.y(), p_xmyp.z());

	glVertex3f(p_xmyp.x(), p_xmyp.y(), p_xmyp.z());
	glVertex3f(p_xpym.x(), p_xpym.y(), p_xpym.z());
	glVertex3f(p_xpyp.x(), p_xpyp.y(), p_xpyp.z());

	glEnd();
}

void drawShadedSphere_1(const Eigen::Vector3d& in_c, const double& in_r, const int in_nSegs, const Eigen::Vector3d& in_kd_Blue, const Eigen::Vector3d& in_kd_Yellow) {
	double R = 1.5; // 0で球

	int M = 500;
	int S = 500;
	double radius4 = 0.4;

	const double angleS = 2 * M_PI / S;

	const double angleM = 2 * M_PI / M;

	for (int i = 0; i < M; i++) {

		for (int j = 0; j < S; j++) {

			double x1 = (R + radius4 * sin((i - 1) * angleS)) * cos(j * angleM);
			double y1 = radius4 * cos((i - 1) * angleS);
			double z1 = (R + radius4 * sin((i - 1) * angleS)) * sin(j * angleM);
			

			double x2 = (R + radius4 * sin(i * angleS)) * cos(j * angleM);
			double y2 = radius4 * cos(i * angleS);
			double z2 = (R + radius4 * sin(i * angleS)) * sin(j * angleM);
			

			double x3 = (R + radius4 * sin(i * angleS)) * cos((j + 1) * angleM);
			double y3 = radius4 * cos(i * angleS);
			double z3 = (R + radius4 * sin(i * angleS)) * sin((j + 1) * angleM);

			Eigen::Vector3d in_p1{x1, z1, y1 };
			Eigen::Vector3d in_p2{x2, z2, y2};
			Eigen::Vector3d in_p3{x3, z3, y3 };

			drawShadedTriangle(in_p2, in_p1, in_p3, in_kd_Blue, in_kd_Yellow);

		}
	}
}

void drawShadedSphere_2(const Eigen::Vector3d& in_c, const double& in_r, const int in_nSegs, const Eigen::Vector3d& in_kd_Blue, const Eigen::Vector3d& in_kd_Yellow) {
	double R = 7;

	int M = 500;
	int S = 500;
	double radius4 = 0.4;

	const double angleS = 2 * M_PI / S;

	const double angleM = 2 * M_PI / M;

	for (int i = 0; i < M; i++) {

		for (int j = 0; j < S; j++) {

			double x1 = (R - cos((i - 1) * angleS) + sin(5 * j * angleM)) * cos(2 * j * angleM);
			double y1 = cos(5 * j * angleM) + sin((i - 1) * angleS);
			double z1 = (R - cos((i - 1) * angleS) + sin(5 * j * angleM)) * sin(2 * j * angleM);

			double x2 = (R - cos(i * angleS) + sin(5 * j * angleM)) * cos(2 * j * angleM);
			double y2 = cos(5 * j * angleM) + sin(i * angleS);
			double z2 = (R - cos(i * angleS) + sin(5 * j * angleM)) * sin(2 * j * angleM);
			

			double x3 = (R - cos(i * angleS) + sin(5 * (j + 1) * angleM)) * cos(2 * (j + 1) * angleM);
			double y3 = cos(5 * (j + 1) * angleM) + sin(i * angleS);
			double z3 = (R - cos(i * angleS) + sin(5 * (j + 1) * angleM)) * sin(2 * (j + 1) * angleM);
			
			

			Eigen::Vector3d in_p1{ x1, y1, z1 };
			Eigen::Vector3d in_p2{ x2, y2, z2 };
			Eigen::Vector3d in_p3{ x3, y3, z3 };

			drawShadedTriangle(in_p1, in_p2, in_p3, in_kd_Blue, in_kd_Yellow);

		}

	}
}

void drawShadedSphere_3(const Eigen::Vector3d& in_c, const double& in_r, const int in_nSegs, const Eigen::Vector3d& in_kd_Blue, const Eigen::Vector3d& in_kd_Yellow) {
	int i, j, k, l;
	int c = 0;
	double ido[MAX], keido[MAX];
	double v[MAX][3], v1[MAX][3], v2[MAX][3], v3[MAX][3];

	//それぞれの角度を定める(θ:緯度(ido)、Φ:経度(keido))
	for (i = 0; i < in_nSegs + 1; i++) {
		for (j = 0; j < in_nSegs * 2; j++) {
			ido[c] = i * 3.1415 / in_nSegs;
			keido[c] = j * 3.1415 / in_nSegs;

			if (ido[c] >= 3.1415) {
				ido[c] = 3.14159265;
			}

			if (keido[c] >= 6.283) {
				keido[c] = 0.0;

			}
			c++;
		}
	}

	for (k = 0; k < c + 1; k++) {
		//現在の点
		v[k][1] = in_r * sin(ido[k]) * cos(keido[k]);
		v[k][2] = in_r * sin(ido[k]) * sin(keido[k]);
		v[k][3] = in_r * cos(ido[k]);

		//次の経度(keido + 1)の点
		v1[k][1] = in_r * sin(ido[k]) * cos(keido[k] + 3.1415 / in_nSegs);
		v1[k][2] = in_r * sin(ido[k]) * sin(keido[k] + 3.1415 / in_nSegs);
		v1[k][3] = in_r * cos(ido[k]);

		//次の緯度(ido + 1)の点
		v2[k][1] = in_r * sin(ido[k] + 3.1415 / in_nSegs) * cos(keido[k]);
		v2[k][2] = in_r * sin(ido[k] + 3.1415 / in_nSegs) * sin(keido[k]);
		v2[k][3] = in_r * cos(ido[k] + 3.1415 / in_nSegs);

		//次の緯度(ido + 1)、経度(keido + 1)の点
		v3[k][1] = in_r * sin(ido[k] + 3.1415 / in_nSegs) * cos(keido[k] + 3.1415 / in_nSegs);
		v3[k][2] = in_r * sin(ido[k] + 3.1415 / in_nSegs) * sin(keido[k] + 3.1415 / in_nSegs);
		v3[k][3] = in_r * cos(ido[k] + 3.1415 / in_nSegs);

		Eigen::Vector3d in_p1{ v[k][1], v[k][3], v[k][2] };		//点v の位置ベクトル
		Eigen::Vector3d in_p2{ v2[k][1], v2[k][3], v2[k][2] };	//点v2 の位置ベクトル
		//Eigen::Vector3d in_p2S{ 0.0, 3.0, 3.0 };	//点v1 の位置ベクトル
		Eigen::Vector3d in_p3{ v3[k][1], v3[k][3], v3[k][2] };		//点v3 の位置ベクトル

		drawShadedTriangle(in_p2 + in_c, in_p1 + in_c, in_p3 + in_c, in_kd_Blue, in_kd_Yellow);

	}
}

void idle()
{
#ifdef __APPLE__
	usleep(1000.0 * 1000.0 / 60.0); // in microseconds
#else
	Sleep(1000.0 / 60.0); // in milliseconds
#endif
	glutPostRedisplay();
}

void projection_and_modelview(const Camera& in_Camera)
{
	const double fovy_deg = (2.0 * 180.0 / M_PI) * atan(0.024 * 0.5 / in_Camera.getFocalLength());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy_deg, double(width) / double(height), 0.01 * in_Camera.getFocalLength(), 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const Eigen::Vector3d lookAtPoint = in_Camera.getLookAtPoint();
	gluLookAt(in_Camera.getEyePoint().x(), in_Camera.getEyePoint().y(), in_Camera.getEyePoint().z(), lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z(), in_Camera.getYVector().x(), in_Camera.getYVector().y(), in_Camera.getYVector().z());
}

void drawFloor(const Eigen::Vector3d& in_kd_Blue, const Eigen::Vector3d& in_kd_Yellow)
{
	glBegin(GL_TRIANGLES);
	for (int j = -100; j < 100; j++)
	{
		for (int i = -100; i < 100; i++)
		{
			int checker_bw = (i + j) % 2;
			if (checker_bw == 0)
			{
				/*
				glColor3f(0.3, 0.3, 0.3);

				glVertex3f(i * 0.5, 0.0, j * 0.5);
				glVertex3f(i * 0.5, 0.0, (j + 1) * 0.5);
				glVertex3f((i + 1) * 0.5, 0.0, j * 0.5);

				glVertex3f(i * 0.5, 0.0, (j + 1) * 0.5);
				glVertex3f((i + 1) * 0.5, 0.0, (j + 1) * 0.5);
				glVertex3f((i + 1) * 0.5, 0.0, j * 0.5);
				*/

				Eigen::Vector3d in_p1{ i * 0.1, 0.0, j * 0.1 };
				Eigen::Vector3d in_p2{ i * 0.1, 0.0, (j + 1) * 0.1 };
				Eigen::Vector3d in_p3{ (i + 1) * 0.1, 0.0, j * 0.1 };

				drawShadedTriangle(in_p2, in_p1, in_p3, in_kd_Blue, in_kd_Yellow);
			}
		}
	}
	glEnd();
}


void display()
{
	glViewport(0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y);

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection_and_modelview(g_Camera);

	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	drawFloor({ 0.1, 0.1, 0.5 }, { 0.4, 0.4, 0.1 });

	drawShadedSphere_1({ 0, 0.5, 0 }, 0.5, 140, { 0.1, 0.1, 0.5 }, { 0.4, 0.4, 0.1 });

	drawShadedSphere_2({ 0, 0.5, 0 }, 0.5, 140, { 0.1, 0.1, 0.5 }, { 0.4, 0.4, 0.1 });

	drawShadedSphere_3({ 2.0, 0.5, 2.0 }, 0.2, 140, { 0.1, 0.1, 0.5 }, { 0.4, 0.4, 0.1 });

	glutSwapBuffers();
}

void resize(int w, int h)
{
	width = w;
	height = h;
}

void mouseDrag(int x, int y)
{
	const int _dx = x - mx;
	mx = x; my = y;

	const double dx = double(_dx) / double(width);
	const double scale = 2.0;

	g_Camera.rotateCameraInLocalFrameFixLookAt(dx * scale);
	glutPostRedisplay();
}

void mouseDown(int x, int y)
{
	mx = x; my = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		mouseDown(x, y);
}

int main(int argc, char* argv[])
{
	g_Camera.setEyePoint(Eigen::Vector3d{ 0.0, 1.0, 3.0 });
	g_Camera.lookAt(Eigen::Vector3d{ 0.0, 0.0, 0.0 }, Eigen::Vector3d{ 0.0, 1.0, 0.0 });

	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutCreateWindow("Hello world!!");

	// With retina display, frame buffer size is twice the window size.
	// Viewport size should be set on the basis of the frame buffer size, rather than the window size.
	// g_FrameSize_WindowSize_Scale_x and g_FrameSize_WindowSize_Scale_y account for this factor.
	GLint dims[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, dims);
	g_FrameSize_WindowSize_Scale_x = double(dims[2]) / double(width);
	g_FrameSize_WindowSize_Scale_y = double(dims[3]) / double(height);

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);

	glutMainLoop();
	return 0;
}
