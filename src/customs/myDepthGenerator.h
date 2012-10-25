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
    XnUInt16 near = 0;
    XnUInt16 far = 10000;
    XnUInt16 min = 0;
    XnUInt16 max = 10000;
};

class myDepthGenerator {
public:
    
    myDepthGenerator();
    ~myDepthGenerator();
	bool setup(xn::NodeInfo const& node, int num);
    void startGenerating();
    void update();
    
    //options
    XnDepthPixel * getDepthPixels();//soDepthThresholds const& thre);
    int getNumber() const { return number;};
    void setThreshold(XnUInt16 const& _near, XnUInt16 const& _far);
    
    bool bUpdateMasks;
    XnMapOutputMode& getMapMode();
    //inlines
    inline XnDepthPixel* changeToRealDistance(const XnDepthPixel * dep_pixels);
    inline XnUInt16 changeToRealDistance(XnUInt16 const depthValue);
#ifdef HAVE_TEXTURE
    unsigned char * generateTexture();
#endif
    
private:
    myDepthGenerator(const myDepthGenerator& other);
	myDepthGenerator& operator = (const myDepthGenerator&);
    
    xn::DepthGenerator      depth_generator;
    xn::DepthMetaData       dmd;
    XnMapOutputMode         out_put_modes;
    soDepthThresholds       thresholds;
    XnDepthPixel *          backCaptured_map;
    XnDepthPixel *          depth_map;
    
    int number;
    int counter, counter2;
    
#ifdef HAVE_TEXTURE
    unsigned char * depth_texture;
    unsigned char * mapDepthToChar(XnDepthPixel * const depthMap);
#endif
	
};

