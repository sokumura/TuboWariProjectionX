//
//  TuboWariProjection
//
//  Created by Shuya Okumura on 2012/10/21.
//
//

#include "myDepthGenerator.h"

myDepthGenerator::myDepthGenerator(){
    counter = counter2 = counter3 = 0;
    printf("myDepthGenerator No.%u コンストラクタが呼ばれました。 counter : %u\n", number, counter);
    bUpdateMasks = false;
    // make new map mode -> default to 640 x 480 @ 30fps
    out_put_modes.nXRes = XN_VGA_X_RES;
    out_put_modes.nYRes = XN_VGA_Y_RES;
    out_put_modes.nFPS  = 30;
    
    totalPixel = out_put_modes.nXRes * out_put_modes.nYRes;
    
    bBgDepth = bRunningCapture = false;
    bgCaptureCount = captureTimes = 0;
    freeBgDepth();
    capturePlay = 0;
}

myDepthGenerator::~myDepthGenerator(){

    
}
//----------------------------------------------
bool myDepthGenerator::setup(xn::NodeInfo const& node, int num){
    printf("myDepthGenerator No.%u setup()が呼ばれました。 counter : %u\n", number, counter);
    depthMIN = 10000;
    depthMAX = 0;
    number = num;
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
    if (status == XN_STATUS_OK) printf("DepthGenerator %i Generating Start!\n", number);
    else ofLogError("myDepthGenerator/startGenerating ",xnGetStatusString(status));
}

//----------------------------------------------
void myDepthGenerator::update(soDepthThresholds thresholds){
    
    if (counter < COUNTER_MAX) {
        printf("myDepthGenerator No.%u update()が呼ばれました\n" , number);
        counter++;
    }
    if (depth_generator.IsNewDataAvailable()) {
        depth_generator.WaitAndUpdateData();
        depth_generator.GetMetaData(dmd);
        
        privThresholds = thresholds;
        
        generateTexture();
        generateMonoTexture();
        
        if (bRunningCapture) {
            if (captureTimes > CAPTURE_TIMES) {
                captureTimes = 0;
                bRunningCapture = false;
            } else {
                captureBgDepth();
                captureTimes++;
            }
        }
        
    }
    
}

//----------------------------------------------
XnMapOutputMode& myDepthGenerator::getMapMode() {
    return out_put_modes;
}
//--------------------
void myDepthGenerator::runCapture(){
    bRunningCapture = true;
    bgCaptureCount++;
}

//----------------------------------------------
void myDepthGenerator::captureBgDepth(){
    const XnDepthPixel * bg = dmd.Data();
    XN_ASSERT(bg);
    if (bgCaptureCount != 0) {
        for (int i = 0; i < totalPixel; i++) {
            if (bg[i] != 0 && bgDepth[i] > bg[i]) {
                    bgDepth[i] = bg[i];
            }
        }
    } else {
        for (int i = 0; i < totalPixel; i++){
            bgDepth[i] = bg[i];
        }
    }
}
//--------------
void myDepthGenerator::freeBgDepth(){
    for (int i = 0; i < out_put_modes.nXRes * out_put_modes.nYRes; i++) {
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

#ifdef HAVE_TEXTURE

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

#endif

void myDepthGenerator::console(bool bOut){
    printf("\n--No.%u--privThresholds\nnear:%u\nfar:%u\nmin:%u\nmax:%u\n\n", number, privThresholds.near, privThresholds.far, privThresholds.min, privThresholds.max);
    
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
















