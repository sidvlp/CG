//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "scene.h"


#define _USE_MATH_DEFINES
#include "math.h"

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), fb(0), lr(0), mx(0),my(0)
{
    BaseModel* pModel;
    ConstantShader* pConstShader;
    PhongShader* pPhongShader;


    /*
    // create LineGrid model with constant color shader
    pModel = new LinePlaneModel(10, 10, 10, 10);
    pConstShader = new ConstantShader();
	pConstShader->color( Color(1,1,1));
    pModel->shader(pConstShader, true);
    Models.push_back( pModel );

    
    // Exercise 1
    // TODO Load pTankTop & pTankBot
    pTankBot = new Model(ASSET_DIRECTORY "tank_bottom.dae");
    pPhongShader = new PhongShader();
    pTankBot->shader(pPhongShader, true);
    // add to render list
    pTankTop = new Model(ASSET_DIRECTORY "tank_top.dae");
    pPhongShader = new PhongShader();
    pTankTop->shader(pPhongShader, true);
    // add to render list
    
    Models.push_back(pTankBot);
    Models.push_back(pTankTop);
    */

    // Exercise 2
    /**/
    pPhongShader = new PhongShader();
    pTank = new Tank();
    pTank->shader(pPhongShader, true);
    pTank->loadModels(ASSET_DIRECTORY "tank_bottom.dae", ASSET_DIRECTORY "tank_top.dae");
    Models.push_back( pTank );
    
    
    // Exercise 3
    /**/
    Scene* pScene = new Scene();
    pScene->shader(new PhongShader(), true);
    pScene->addSceneFile(ASSET_DIRECTORY "scene.osh");
    Models.push_back(pScene);
    
}
void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

float Application::toRadian(float degrees) {

    return (float) degrees * M_PI /180;

}


void Application::update(float dtime)
{

   

   /* // Exercise 1
    Matrix pbot, pb1, pb2, pb3, ptop, pt1;

    //anfangszustand
    pbot.identity();
    pb1.translation(4, 0, 0);
    pb2.rotationY(toRadian(270));
    //fahren in kreis
    pb3.rotationY(-dtime);

    pbot = pb3 * pb1 * pb2;
    
    pTankBot->transform(pbot);

    
    ptop.identity();
    //cannon gegen rotieren
    pt1.rotationY(dtime);

    ptop = pbot * pt1;

    pTankTop->transform(ptop);
   */

    // Exercise 2
    Vector pos;

    glfwGetCursorPos(pWindow,&mx,&my);
    Vector collision = Application::calc3DRay(mx, my, pos);
    pTank->aim(collision);
    keyPress(fb, lr);
    pTank->steer(fb, lr);
    fb = 0;
    lr = 0;
    pTank->update(dtime);
    Cam.update();
}

void Application::keyPress(float &fb, float &lr) {
    
    
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
        fb = -1;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
        fb = 1;
    }
  
   
    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
        lr = -1;
    }
  
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
        lr = 1;
    }
   

}

Vector Application::calc3DRay( float x, float y, Vector& Pos)
{   
    //normalisierte Koordinaten berechnen
    int windowWidthHalf = 0;
    int windowHeightHalf = 0;
    glfwGetWindowSize(pWindow, &windowWidthHalf, &windowHeightHalf);
    windowHeightHalf /= 2;
    windowWidthHalf /= 2;

    //Feld auf -1 bis 1 normalisieren
    float xNormal, yNormal;
    xNormal = (x - windowWidthHalf) / windowWidthHalf;
    yNormal = -(y - windowHeightHalf) / windowHeightHalf;
    
    //Richtungsvector(Kamararaum) erzeugen mit inverser Projektionsmatrix
    Matrix projectionCam = this->Cam.getProjectionMatrix(); 
    Vector normalCursor(xNormal, yNormal, 0);
    Vector direction = projectionCam.invert() * normalCursor;

    //Umwandlung des Richtungsvectors in den Weltraum
    Matrix viewMatrix = this->Cam.getViewMatrix();
    Vector directionInWeltraum = viewMatrix.invert().transformVec3x3(direction);

    //Schnittpunkt mit der Ebene y=0 bestimmen
    Vector camPos = this->Cam.position();
    directionInWeltraum.normalize();
    float s;
    camPos.triangleIntersection(directionInWeltraum, Vector(0, 0, 1), Vector(0, 0, 0), Vector(1, 0, 0), s);

    //falls directionWolrld von der Ebene weg zeigt (0,0,0) zurückgeben
    if (s < 0) {
        return Vector(0, 0, 0);
    }

    //Vektor zum Punkt auf der Ebene y=0 berechnen
    Vector positionOnGround = camPos + directionInWeltraum * s;

    //float Ungenauigkeiten umgehen indem der Vektor erneut auf y = 0 gesetzt wird
    return Vector(positionOnGround.X, 0, positionOnGround.Z);

 
    // Pos:Ray Origin
    // return:Ray Direction
    
}

void Application::draw()
{
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
    
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}
void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}