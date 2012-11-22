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

#define TOTAL_PIXEL 640*480

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
    int               getNumber() const { return xtionNum;};
    
    bool              bUpdateMasks;
    XnMapOutputMode&  getMapMode();
    
    void  generateMonoTexture();
    const unsigned char * getMonoTexture() const;
    
    void captureBgDepth();
    void freeBgDepth();
    bool bBgDepth;
    int capturePlay;
    unsigned int getCaptureCount() const;
    void setCapturedCount();
    void runCapture();

    void  generateTexture();
    const unsigned char * getMonitorTexture() const;
    
private:
    myDepthGenerator(const myDepthGenerator& other);
	myDepthGenerator& operator = (const myDepthGenerator&);
    
    void console(bool bOut);
    
    xn::DepthGenerator      depth_generator;
    xn::DepthMetaData       dmd;
    XnMapOutputMode         out_put_modes;
    
    XnDepthPixel bgDepth[TOTAL_PIXEL];
    unsigned char mono_texture[TOTAL_PIXEL];
    unsigned char monitor_texture[TOTAL_PIXEL *4];
    
    XnDepthPixel depthMIN, depthMAX;
    soDepthThresholds privThresholds;//このxtionのthresholds
    int xtionNum;//このxtionの番号
    
    unsigned int bgCaptureCount, totalPixel;
	
};

