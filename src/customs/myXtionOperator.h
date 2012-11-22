//
//  myXtionOperator.h
//  xtionExample
//
//  Created by 奥村 周也 on 12/10/05.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

/*
 xtionの制御は基本的にこのクラスから一行で呼び出す
 */

#ifndef xtionExample_myXtionOperator_h
#define xtionExample_myXtionOperator_h


#include "ofxOpenNIHeaders.h"
#include "XnCppWrapper.h"
#include "myDepthGenerator.h"

#define XTION_NUM 2

extern int thresholdNear[XTION_NUM];
extern int thresholdFar[XTION_NUM];

class myXtionOperator {
public:
    myXtionOperator();
    ~myXtionOperator();
    
    void setup();
    void update();
    void testDraw(float x, float y, float w = 640.0f, float h = 480.0f);
    void customEnumerateProductionTrees(xn::Context& con, XnProductionNodeType type);
    
    myDepthGenerator& getDepthGenerator(int index);
    int counter;
    
    ofVboMesh vboMesh;

private:
    myXtionOperator(const myXtionOperator& other);
    myXtionOperator& operator = (const myXtionOperator&);
    
    ofxOpenNIContext context;
    myDepthGenerator depth_GRs[XTION_NUM];
    soDepthThresholds thresholds[XTION_NUM];
    int generatorNum;
    
    xn::EnumerationErrors errors;
    void logErrors(xn::EnumerationErrors& rErrors);

    
};

#endif
