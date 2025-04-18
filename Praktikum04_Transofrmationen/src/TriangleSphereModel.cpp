//
//  TriangleSphereModel.cpp
//  ogl4
//
//  Created by Philipp Lensing on 20.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleSphereModel.h"
#define _USE_MATH_DEFINES
#include <math.h>

TriangleSphereModel::TriangleSphereModel( float Radius, int Stacks, int Slices )
{

    BoundingBox.Min = Vector(-Radius, -Radius, -Radius);
    BoundingBox.Max = Vector(Radius, Radius, Radius);
    VB.begin();
    for( int i=0; i<Stacks; ++i)
        for( int j=0; j<Slices; ++j)
        {
            float phi = (float)j*(float)M_PI*2.0f/(float)(Slices-1);
            float theta = (float)i*(float)M_PI/(float)(Stacks-1);
            float x = Radius*sin( phi )*sin(theta);
            float z = Radius*cos( phi )*sin(theta);
            float y = Radius*cos( theta );
            VB.addNormal( Vector(x,y,z).normalize());
            VB.addTexcoord0( phi/((float)M_PI*2.0f), theta/ (float)M_PI );
            VB.addTexcoord1( (float)Slices*phi/((float)M_PI*2.0f), (float)Stacks*(theta/ (float)M_PI) );
            VB.addVertex(x, y, z);
        }
    VB.end();
    
    IB.begin();
    for( int i=0; i<Stacks-1; ++i)
        for( int j=0; j<Slices-1; ++j)
        {
            IB.addIndex( i*Slices + j+1 );
            IB.addIndex( i*Slices + j );
            IB.addIndex( (i+1)*Slices + j );

            IB.addIndex( (i+1)*Slices + j );
            IB.addIndex( (i+1)*Slices + j+1 );
            IB.addIndex( i*Slices + j+1 );
        }
    
    IB.end();
}
void TriangleSphereModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);
    
    VB.activate();
    IB.activate();
    glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
    IB.deactivate();
    VB.deactivate();
}

void TriangleSphereModel::calculateBoundingBox() 
{
   
}

AABB& TriangleSphereModel::getBoundingBox()
{
    return BoundingBox;
}


