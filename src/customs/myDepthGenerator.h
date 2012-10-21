//
//  myDepthGenerator.h
//  xtionExample
//
//  Created by 奥村 周也 on 12/10/03.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef xtionExample_myDepthGenerator_h
#define xtionExample_myDepthGenerator_h

#include "XnCppWrapper.h"
#include "ofxOpenNI.h"
#include "ofMain.h"


typedef struct {
	XnUInt16 nearThreshold;
	XnUInt16 farThreshold;
} mydepth_threshold;

class myDepthGenerator {
	
public:
    std::vector<xn::DepthGenerator>	depth_generator;
    std::vector<ofTexture>	depth_texture;
    std::vector<unsigned char*>	depth_pixels;
    std::vector<unsigned char*>	mask_pixels;
    std::vector<mydepth_threshold> depth_thresholds;
    std::vector<XnMapOutputMode> out_put_modes;
    std::vector<XnUInt16> max_depth;
    
    bool bUpdateMasks;
    //TODO: テクスチャーを作る（プレビュー）かどうかを受けて処理する記述を加える
    bool bGenerateTexture;
    int generatorNum;
    
    myDepthGenerator(){
        generatorNum = 0;
        bUpdateMasks = false;
        bGenerateTexture = true;//初期設定としては、プレビュー有り
    }

	
    //----------------------------------------------
	bool setup(xn::NodeInfo const& node){
        XnStatus result = XN_STATUS_OK;	
        xn::DepthGenerator dg;
        result = node.GetInstance(dg);
        if (result != XN_STATUS_OK) {
            ofLogError("myDepthGenerator.setup()", xnGetStatusString(result));
            return false;
        } else {
            depth_generator.push_back(dg);
            unsigned char maskp[640 * 480];
            mask_pixels.push_back(maskp);
           

            // make new map mode -> default to 640 x 480 @ 30fps
            XnMapOutputMode map_mode;
            map_mode.nXRes = XN_VGA_X_RES;
            map_mode.nYRes = XN_VGA_Y_RES;
            map_mode.nFPS  = 30;
            out_put_modes.push_back(map_mode);
            
            depth_generator[generatorNum].SetMapOutputMode(map_mode);
            
            // TODO: add capability for b+w depth maps (more efficient for draw)
            ofTexture tex;
            tex.allocate(map_mode.nXRes, map_mode.nYRes, GL_RGBA);
            depth_texture.push_back(tex);
            depth_pixels.push_back(new unsigned char[map_mode.nXRes * map_mode.nYRes * 4]);
            memset(depth_pixels[generatorNum], 0, map_mode.nXRes * map_mode.nYRes * 4 * sizeof(unsigned char));
            
            // setup mask pixelskk
            XnUInt16 depthMax = depth_generator[generatorNum].GetDeviceMaxDepth();
            max_depth.push_back(depthMax);
            mask_pixels[generatorNum] = new unsigned char[out_put_modes[generatorNum].nXRes * out_put_modes[generatorNum].nYRes];
            mydepth_threshold thre;//これを使う
            depth_thresholds.push_back(thre);
            depth_thresholds[generatorNum].nearThreshold = 0;
            depth_thresholds[generatorNum].farThreshold = max_depth[generatorNum - 1];
            
            printf("Depth camera No.%d inited\n", generatorNum);
            generatorNum++;
            
            return true;
        }
    }
    //----------------------------------------------
    void startGeneratingAll(){
        for (int i = 0; i < generatorNum; i++){
            depth_generator[i].StartGenerating();
            printf("DepthGenerator%i Generating Start!\n", i);
        }
    }
	//----------------------------------------------
	void testDraw(float x=0, float y=0, float w=640, float h=480){
        glColor3f(1,1,1);
        for (int i = 0; i < generatorNum; i++) {
            depth_texture[i].draw(x + i * 660.0, y, w, h);
        }
    }
    //----------------------------------------------
	void update(){
        
        for (int i = 0; i < generatorNum; i++) {
            if (depth_generator[i].IsNewDataAvailable()) {
                depth_generator[i].WaitAndUpdateData();
                xn::DepthMetaData depthMD;
                depth_generator[i].GetMetaData(depthMD);
                if (bGenerateTexture) generateTexture(depthMD, i);
                if (bUpdateMasks) {
                    updateMaskPixels(depthMD, i);
                }
            }
        }
    }
    //----------------------------------------------
    void generateTexture(const xn::DepthMetaData& dmd, int textureNum = 0, XnUInt16 nearThreshold = 0, XnUInt16 farThreshold = 10000){
        if (textureNum > generatorNum) {
            ofLogError("myDepthGenerator.generateTexture()", "不正なアクセス");
            return;
        } else {
            if (farThreshold == 10000) farThreshold = max_depth[textureNum];
            // get the pixels
            const XnDepthPixel* depth = dmd.Data();
            XN_ASSERT(depth);
            
            if (dmd.FrameID() == 0) return;
            
            float max;
            for (XnUInt16 y = dmd.YOffset(); y < dmd.YRes() + dmd.YOffset(); y++) {
                unsigned char * texture = (unsigned char*)depth_pixels[textureNum] + y * dmd.XRes() * 4 + dmd.XOffset() * 4;
                for (XnUInt16 x = 0; x < dmd.XRes(); x++, depth++, texture += 4) {
                    XnUInt8 red = 0;
                    XnUInt8 green = 0;
                    XnUInt8 blue = 0;
                    XnUInt8 alpha = 255;
                    
                    max = 255;	// half depth
                    XnUInt8 a = (XnUInt8)(((*depth) / ( max_depth[textureNum] / max)));
                    red		= a;
                    green	= a;
                    blue	= a;
                    
                    texture[0] = red;
                    texture[1] = green;
                    texture[2] = blue;
                    
                    if (*depth == 0)
                        texture[3] = 0;
                    else
                        texture[3] = alpha;
                }
            }
            depth_texture[textureNum].loadData((unsigned char *)depth_pixels[textureNum], dmd.XRes(), dmd.YRes(), GL_RGBA);
            //printf("depth_texture[%i]を書き換えました。\n", textureNum);
        }
    }
	//----------------------------------------------
    void setDepthThreshold(int nearThreshold, int farThreshold, int forDepthThresholdNumber = 0){
        if (forDepthThresholdNumber > generatorNum) {
            ofLogError("myDepthGenerator.setDepthThreshold()", "不正なアクセス");
        } else {
            depth_thresholds[forDepthThresholdNumber].nearThreshold = nearThreshold;
            depth_thresholds[forDepthThresholdNumber].farThreshold = farThreshold;
        }
    }
	//----------------------------------------------
   void setThresholds(int nearThreshold, int farThreshold, int thresholdsNumber = 0){
        if (thresholdsNumber > generatorNum) {
            ofLogError("myDepthGenerator.getDepthPixels()", "不正なアクセス");
        } else {
            depth_thresholds[thresholdsNumber].nearThreshold = nearThreshold;
            depth_thresholds[thresholdsNumber].farThreshold = farThreshold;
        }
 
    }
    //----------------------------------------------
    unsigned char*	getDepthPixels(int maskNumber = 0){
        if (maskNumber > generatorNum) {
            ofLogError("myDepthGenerator.getDepthPixels()", "不正なアクセス");
        } else return mask_pixels[maskNumber];
    }

    //----------------------------------------------
    void updateMaskPixels(const xn::DepthMetaData& dmd, int maskNumber = 0){
        if (maskNumber > generatorNum) {
            ofLogError("myDepthGenerator.updateMaskPixels()", "不正なアクセス");
            return;
        }
        const XnDepthPixel* depth = changeToRealDistance(dmd.Data());
        
        int numPixels = dmd.XRes() * dmd.YRes();
        
        for(int i = 0; i < numPixels; i++, depth++) {
            if(*depth < depth_thresholds[maskNumber].farThreshold && *depth > depth_thresholds[maskNumber].nearThreshold) {
                mask_pixels[maskNumber][i] = 255;
            } else {
                mask_pixels[maskNumber][i] = 0;
            }
            
        }
    }
    //----------------------------------------------
    XnDepthPixel* changeToRealDistance(const XnDepthPixel * dep_pixels) {
        XnDepthPixel * p = new XnUInt16[sizeof(dep_pixels) / sizeof(dep_pixels[0])];
        //XnDepthPixel newPixels[sizeof(dep_pixels) / sizeof(dep_pixels[0])];
        for (int i = 0; i < sizeof(dep_pixels) / sizeof(dep_pixels[0]); i++) {
            p[i] = (unsigned short)(0.1236 * tan(dep_pixels[i] / 2842.5 + 1.1863));
        }
        //p = newPixels;
        return p;
    } 
    //-----------------------------------------------
	
	
};

#endif
