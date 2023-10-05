//
//  main.cpp
//
//  Created by Yonghao Yue on 2019/09/28.
//  Updated: 2021/10/13
//  Copyright © 2019 Yonghao Yue. All rights reserved.
//

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

void idle()
{
#ifdef __APPLE__
  usleep( 1000.0 * 1000.0 / 60.0 ); // in microseconds
#else
  Sleep(1000.0/60.0); // in milliseconds
#endif
  glutPostRedisplay();
}

void projection_and_modelview( const Camera& in_Camera )
{
  const double fovy_deg = ( 2.0 * 180.0 / M_PI ) * atan( 0.024 * 0.5 / in_Camera.getFocalLength() );
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( fovy_deg, double( width ) / double( height ), 0.01 * in_Camera.getFocalLength(), 1000.0 );
  
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  const Eigen::Vector3d lookAtPoint = in_Camera.getLookAtPoint();
  gluLookAt( in_Camera.getEyePoint().x(), in_Camera.getEyePoint().y(), in_Camera.getEyePoint().z(), lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z(), in_Camera.getYVector().x(), in_Camera.getYVector().y(), in_Camera.getYVector().z() );
}

void drawFloor()
{
  glBegin( GL_TRIANGLES );
  for( int j=-20; j<20; j++ )
  {
    for( int i=-20; i<20; i++ )
    {
      int checker_bw = ( i+j ) % 2;
      if( checker_bw == 0 )
      {
        glColor3f( 0.3, 0.3, 0.3 );

        glVertex3f( i*0.5, 0.0, j*0.5 );
        glVertex3f( i*0.5, 0.0, (j+1)*0.5 );
        glVertex3f( (i+1)*0.5, 0.0, j*0.5 );
        
        glVertex3f( i*0.5, 0.0, (j+1)*0.5 );
        glVertex3f( (i+1)*0.5, 0.0, (j+1)*0.5 );
        glVertex3f( (i+1)*0.5, 0.0, j*0.5 );
      }
    }
  }
  glEnd();
}

//*
void drawSphereHint()
{
  glBegin(GL_TRIANGLES);
  const double t1 = M_PI * 0.5 * 30.0 / 90.0;
  const double t2 = M_PI * 0.5 * 45.0 / 90.0;
  const double r = 0.8;
  int N = 36;
  for( int i = 0; i < N; i++ )
  {
    const double p1 = 2.0 * M_PI * double(i) / double(N);
    const double p2 = 2.0 * M_PI * double(i + 1) / double(N);
         
    glColor3f( 0.5, 0.5 + 0.5 * sin( 2.0 * M_PI * double(i)/double(N) ), 0.3 );
    glVertex3f(r * sinf(t1) * cosf(p1), r * cosf(t1), r * sinf(t1) * sinf(p1));
    glColor3f( 0.5, 0.5 + 0.5 * sin( 2.0 * M_PI * double(i)/double(N) ), 0.3 );
    glVertex3f(r * sinf(t1) * cosf(p2), r * cosf(t1), r * sinf(t1) * sinf(p2));
    glColor3f( 0.5, 0.5 + 0.5 * sin( 2.0 * M_PI * double(i)/double(N) ), 0.3 );
    glVertex3f(r * sinf(t2) * cosf(p1), r * cosf(t2), r * sinf(t2) * sinf(p1));

    glColor3f( 0.5, 0.5 + 0.5 * sin( 2.0 * M_PI * double(i)/double(N) ), 0.3 );
    glVertex3f(r * sinf(t1) * cosf(p2), r * cosf(t1), r * sinf(t1) * sinf(p2));
    glColor3f( 0.5, 0.5 + 0.5 * sin( 2.0 * M_PI * double(i)/double(N) ), 0.3 );
    glVertex3f(r * sinf(t2) * cosf(p1), r * cosf(t2), r * sinf(t2) * sinf(p1));
    glColor3f( 0.5, 0.5 + 0.5 * sin( 2.0 * M_PI * double(i)/double(N) ), 0.3 );
    glVertex3f(r * sinf(t2) * cosf(p2), r * cosf(t2), r * sinf(t2) * sinf(p2));
  }
  glEnd();
}
//*/

void display()
{
  glViewport( 0, 0, width * g_FrameSize_WindowSize_Scale_x, height * g_FrameSize_WindowSize_Scale_y );
  
  glClearColor( 1.0, 1.0, 1.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  projection_and_modelview( g_Camera );
  
  glEnable( GL_DEPTH_TEST );

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_BLEND );
  glEnable( GL_POINT_SMOOTH );
  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POLYGON_SMOOTH);
  glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
  
  drawFloor();
  
  drawSphereHint();
  
  glutSwapBuffers();
}

void resize( int w, int h )
{
  width = w;
  height = h;
}

void mouseDrag( int x, int y )
{
  const int _dx = x - mx;
  mx = x; my = y;
  
  const double dx = double( _dx ) / double( width );
  const double scale = 2.0;
  
  g_Camera.rotateCameraInLocalFrameFixLookAt( dx * scale );
  glutPostRedisplay();
}

void mouseDown( int x, int y )
{
  mx = x; my = y;
}

void mouse( int button, int state, int x, int y )
{
  if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    mouseDown( x, y );
}

int main( int argc, char* argv[] )
{
  g_Camera.setEyePoint( Eigen::Vector3d{ 0.0, 1.0, 3.0 } );
  g_Camera.lookAt( Eigen::Vector3d{ 0.0, 0.0, 0.0 }, Eigen::Vector3d{ 0.0, 1.0, 0.0} );
  
  glutInit( &argc, argv );
  glutInitWindowSize( width, height );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE );
  
  glutCreateWindow( "Hello world!!" );
  
  // With retina display, frame buffer size is twice the window size.
  // Viewport size should be set on the basis of the frame buffer size, rather than the window size.
  // g_FrameSize_WindowSize_Scale_x and g_FrameSize_WindowSize_Scale_y account for this factor.
  GLint dims[4] = {0};
  glGetIntegerv(GL_VIEWPORT, dims);
  g_FrameSize_WindowSize_Scale_x = double( dims[2] ) / double( width );
  g_FrameSize_WindowSize_Scale_y = double( dims[3] ) / double( height );
  
  glutDisplayFunc( display );
  glutIdleFunc( idle );
  glutReshapeFunc( resize );
  glutMouseFunc( mouse );
  glutMotionFunc( mouseDrag );

  glutMainLoop();
  return 0;
}
