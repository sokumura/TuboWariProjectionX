//
//  myDepthGenerator.h
//  xtionExample
//
//  Created by Shuya Okumura on 12/10/03.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

/*
 DepthGenerator 1つが行う処理については全てここに書く
 入出力に関しては、距離の数値を色の数値に変換しない。
 */

#pragma once

#include "XnCppWrapper.h"
#include "ofMain.h"


#define HAVE_TEXTURE
#define COUNTER_MAX 0

struct soDepthThresholds {
    int near = 3000;
    int far = 5000;
    int min = 0;
    int max = 8000;
};

class myDepthGenerator {
public:
    
    myDepthGenerator();
    ~myDepthGenerator();
	bool setup(xn::NodeInfo const& node, int num);
    void startGenerating();
    void update(soDepthThresholds thresholds);
    
    //options
    int               getNumber() const { return number;};
    
    bool              bUpdateMasks;
    XnMapOutputMode&  getMapMode();
    
    void  generateMonoTexture();
    const unsigned char * getMonoTexture() const;
    
    ofVboMesh vboMesh;
    ofVbo vbo;

#ifdef HAVE_TEXTURE
    void  generateTexture();
    const unsigned char * getMonitorTexture() const;
#endif
    
private:
    myDepthGenerator(const myDepthGenerator& other);
	myDepthGenerator& operator = (const myDepthGenerator&);
    
    void console(bool bOut);
    
    xn::DepthGenerator      depth_generator;
    xn::DepthMetaData       dmd;
    XnMapOutputMode         out_put_modes;
    XnDepthPixel *          backCaptured_map;
    XnDepthPixel *          depth_map;
    
    XnDepthPixel depthMIN, depthMAX;
    
    int number;
    int counter, counter2;
    
    unsigned char * mono_texture;
    
    soDepthThresholds privThresholds;
    
#ifdef HAVE_TEXTURE
    unsigned char * monitor_texture;
    
#endif
	
};

