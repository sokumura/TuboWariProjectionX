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
#define CAPTURE_TIMES 30

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
    
    void captureBgDepth();
    void freeBgDepth();
    bool bBgDepth;
    int capturePlay;
    unsigned int getCaptureCount() const;
    void setCapturedCount();
    void runCapture();

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
    XnDepthPixel bgDepth[640 * 480];
    //XnDepthPixel *          depth_map;
    
    XnDepthPixel depthMIN, depthMAX;
    
    int number;
    int counter, counter2, counter3;
    
    unsigned int bgCaptureCount, totalPixel, captureTimes;
    bool bRunningCapture;
    
    unsigned char mono_texture[640*480];
    
    soDepthThresholds privThresholds;
    
#ifdef HAVE_TEXTURE
    unsigned char monitor_texture[640*480*4];
#endif
	
};

