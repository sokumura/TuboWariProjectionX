//
//  TuboWariProjection
//
//  Created by Shuya Okumura on 2012/10/21.
//
//

#include "myDepthGenerator.h"

myDepthGenerator::myDepthGenerator(){
    bUpdateMasks = false;
    // make new map mode -> default to 640 x 480 @ 30fps
    out_put_modes.nXRes = XN_VGA_X_RES;
    out_put_modes.nYRes = XN_VGA_Y_RES;
    out_put_modes.nFPS  = 30;
    
    totalPixel = out_put_modes.nXRes * out_put_modes.nYRes;
    
    bBgDepth = false;
    bgCaptureCount = 0;
    freeBgDepth();
    capturePlay = 0;//testAppで初期設定せよ
}

myDepthGenerator::~myDepthGenerator(){

    
}
//----------------------------------------------
bool myDepthGenerator::setup(xn::NodeInfo const& node, int num){
    depthMIN = 10000;
    depthMAX = 0;
    xtionNum = num;
    XnStatus result = XN_STATUS_OK;
    result = node.GetInstance(depth_generator);
    if (result != XN_STATUS_OK) {
        ofLogError("myDepthGenerator.setup()", xnGetStatusString(result));
        return false;
    } else {
        depth_generator.SetMapOutputMode(out_put_modes);
        return true;
    }
    
    
}
//----------------------------------------------
void myDepthGenerator::startGenerating(){
    XnStatus status = depth_generator.StartGenerating();
    if (status == XN_STATUS_OK) printf("DepthGenerator %i Generating Start!\n", xtionNum);
    else ofLogError("myDepthGenerator/startGenerating ",xnGetStatusString(status));
}

//----------------------------------------------
void myDepthGenerator::update(soDepthThresholds thresholds){
    
    if (depth_generator.IsNewDataAvailable()) {
        depth_generator.WaitAndUpdateData();
        depth_generator.GetMetaData(dmd);
        
        privThresholds = thresholds;
        
        generateTexture();
        generateMonoTexture();
        
    }
    
}

//----------------------------------------------
XnMapOutputMode& myDepthGenerator::getMapMode() {
    return out_put_modes;
}
//--------------------
void myDepthGenerator::runCapture(){
    captureBgDepth();
    bgCaptureCount++;
}

//----------------------------------------------
void myDepthGenerator::captureBgDepth(){
    const XnDepthPixel * bg = dmd.Data();
    unsigned int X = out_put_modes.nXRes;
    unsigned int Y = out_put_modes.nYRes;
    unsigned short addition = bgCaptureCount;
    int counter6[9] = {0,0,0,
                        0,0,0,
                        0,0,0};
    XN_ASSERT(bg);
    for (int i = 0; i < totalPixel; i++) {
            if (i == 0) {//左上角
                bgDepth[i] =
                (bg[i] + bg[i+1] +
                 bg[i + X] + bg[i + X +1]) / 4
                - addition;
                counter6[0]++;
            } else if (i == X - 1) {//右上角
                bgDepth[i] =
                (bg[i] + bg[i-1] +
                 bg[i + X] + bg[i + X -1]) / 4
                - addition;
                counter6[1]++;
            } else if (i == X * Y - X) {//左下角
                bgDepth[i] =
                (bg[i] + bg[i+1] +
                 bg[i - X] + bg[i - X +1]) / 4
                - addition;
                counter6[2]++;
            } else if (i == X * Y - 1) {//右下角
                bgDepth[i] =
                (bg[i] + bg[i - 1] +
                 bg[i - X] + bg[i - X -1]) / 4
                - addition;
                counter6[3]++;
            }else if (i % X == 0 && i >= X && i < X * Y - X) {// 左端
                bgDepth[i] =
                (bg[i] + bg[i+1] +
                 bg[i - X] + bg[i - X +1] +
                 bg[i + X] + bg[i + X +1]) / 6
                - addition;
                counter6[4]++;
            } else if (i % X == X - 1 && i >= X && i < X * Y - X) {//右端
                bgDepth[i] = 
                (bg[i] + bg[i - 1] +
                 bg[i - X] + bg[i - X -1] +
                 bg[i + X] + bg[i + X -1]) / 6
                - addition;
                counter6[5]++;
            } else if (i < X) {//上端
                bgDepth[i] =
                (bg[i] + bg[i+1] + bg[i-1] +
                 bg[i + X] + bg[i + X - 1] + bg[i + X +1]) / 6
                - addition;
                counter6[6]++;
            } else if (i >= X * Y - X) {//下端
                bgDepth[i] =
                (bg[i] + bg[i+1] + bg[i-1] +
                 bg[i - X] + bg[i - X - 1] + bg[i - X +1] )/ 6
                - addition;
                 counter6[7]++;
            } else {//その他
                bgDepth[i] =
                (bg[i] + bg[i+1] + bg[i-1] +
                bg[i - X] + bg[i - X - 1] + bg[i - X +1] +
                 bg[i + X] + bg[i + X - 1] + bg[i + X +1]) / 9
                - addition;
                counter6[8]++;
            }
    }
}
//--------------
void myDepthGenerator::freeBgDepth(){
    for (int i = 0; i < totalPixel; i++) {
        bgDepth[i] = privThresholds.max;
    }
    bgCaptureCount = 0;
}
//--------------
unsigned int myDepthGenerator::getCaptureCount() const{
    return bgCaptureCount;
}

//-----------------------------
void myDepthGenerator::generateMonoTexture() {
    const XnDepthPixel * depth = dmd.Data();
    //XN_ASSERT(depth);
    if (dmd.FrameID() == 0) return;
    
    int i = 0;
    float max = 255;
    for (XnUInt16 y = 0; y < dmd.YRes(); y++) {
        unsigned char * texture = mono_texture + y * dmd.XRes();
		for (XnUInt16 x = 0; x < dmd.XRes(); x++, i++, depth++, texture++) {
            if (bBgDepth && bgDepth[i] - capturePlay <= *depth) {
                *texture = 0;
                continue;
            }
            if (*depth > privThresholds.near && *depth < privThresholds.far)
                *texture = 255;
            else
                *texture = 0;
        }
    }
    
}

const unsigned char * myDepthGenerator::getMonoTexture() const{
    return mono_texture;
}

//---------------
void myDepthGenerator::generateTexture() {
    const XnDepthPixel * depth = dmd.Data();
    XN_ASSERT(depth);
    if (dmd.FrameID() == 0) return;
    
    float max = 255;
    int i = 0;
    for (XnUInt16 y = 0; y < dmd.YRes(); y++) {
        unsigned char * texture = monitor_texture + y * dmd.XRes() * 4;
		for (XnUInt16 x = 0; x < dmd.XRes(); x++, i++, depth++, texture += 4) {
            if (bBgDepth && bgDepth[i] - capturePlay <= *depth) {
                texture[0] = 0;
                texture[1] = 0;
                texture[2] = 0;
                texture[3] = 0;
                continue;
            }
            XnUInt8 a = (XnUInt8)(((*depth) / (1 - privThresholds.max / max)));
            if (*depth > privThresholds.near && *depth < privThresholds.far) {
                texture[0] = 255;
            } else texture[0] = a;
            
            texture[1] = a;
            texture[2] = a;
            
            if (*depth == 0)
                texture[3] = 0;
            else
                texture[3] = 255;
            
            if (*depth < depthMIN && *depth != 0) depthMIN = *depth;
            if (*depth > depthMAX) depthMAX = *depth;
        }
    }
}

const unsigned char * myDepthGenerator::getMonitorTexture() const{
    return monitor_texture;
}

void myDepthGenerator::console(bool bOut){
    printf("\n--No.%u--privThresholds\nnear:%u\nfar:%u\nmin:%u\nmax:%u\n\n", xtionNum, privThresholds.near, privThresholds.far, privThresholds.min, privThresholds.max);
    
    cout << "\n\n---------------dmd-------------------\n" <<
    "dmd.DataSize() : " << dmd.DataSize() <<
    "\ndmd.FPS() : " << dmd.FPS() <<
    "\ndmd.FrameID() : " << dmd.FrameID() <<
    "\ndmd.FullXRes() : " << dmd.FullXRes() <<
    "\ndmd.FullYRes() : " << dmd.FullYRes() <<
    "\ndmd.XRes() : " << dmd.XRes() <<
    "\ndmd.YRes() : " << dmd.YRes() <<
    "\ndmd.ZRes() : " << dmd.ZRes() <<
    "\ndmd.XOffset() : " << dmd.XOffset() <<
    "\ndmd.YOffset() : " << dmd.YOffset() << endl;
}
















