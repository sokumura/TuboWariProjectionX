//
//  TuboWariProjection
//
//  Created by Shuya Okumura on 2012/10/21.
//
//

#include "myDepthGenerator.h"

myDepthGenerator::myDepthGenerator(){
    counter = counter2 = 0;
    printf("myDepthGenerator No.%u コンストラクタが呼ばれました。 counter : %u\n", number, counter);
    bUpdateMasks = false;
    // make new map mode -> default to 640 x 480 @ 30fps
    out_put_modes.nXRes = XN_VGA_X_RES;
    out_put_modes.nYRes = XN_VGA_Y_RES;
    out_put_modes.nFPS  = 30;
    
    backCaptured_map = new unsigned short[out_put_modes.nXRes * out_put_modes.nYRes];
    depth_map = new unsigned short[out_put_modes.nXRes * out_put_modes.nYRes];
    memset(backCaptured_map, 0, out_put_modes.nXRes * out_put_modes.nYRes * sizeof(unsigned short));
    memset(depth_map, 0, out_put_modes.nXRes * out_put_modes.nYRes * sizeof(unsigned short));
#ifdef HAVE_TEXTURE
    depth_texture = new unsigned char[out_put_modes.nXRes * out_put_modes.nYRes * 4];
    memset(depth_texture, 0, out_put_modes.nXRes * out_put_modes.nYRes * 4 * sizeof(unsigned char));
#endif
    
}

myDepthGenerator::~myDepthGenerator(){
    printf("myDepthGenerator No.%u デストラクタが呼ばれました。\n", number);
    delete [] backCaptured_map;
    delete [] depth_map;
#ifdef HAVE_TEXTURE
    delete [] depth_texture;
#endif
}
//----------------------------------------------
bool myDepthGenerator::setup(xn::NodeInfo const& node, int num){
    printf("myDepthGenerator No.%u setup()が呼ばれました。 counter : %u\n", number, counter);
    number = num;
    XnStatus result = XN_STATUS_OK;

    result = node.GetInstance(depth_generator);
    if (result != XN_STATUS_OK) {
        ofLogError("myDepthGenerator.setup()", xnGetStatusString(result));
        return false;
    } else {
        depth_generator.SetMapOutputMode(out_put_modes);
        
        // setup mask pixelskk
        thresholds.max = thresholds.far = depth_generator.GetDeviceMaxDepth();
        
        cout << "\n\n/////myDepthGenerator/setup()///////" <<
        "\ndepth_generator.GetDeviceMaxDepth() : " << depth_generator.GetDeviceMaxDepth() <<
        "\nthresholds.max : " << thresholds.max <<
        "\nthresholds.far : " << thresholds.far << "\n\n" << endl;
        
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

//----------------------------------------------
void myDepthGenerator::update(){
    
    if (counter < COUNTER_MAX) {
        printf("myDepthGenerator No.%u update()が呼ばれました\n" , number);
        counter++;
    }
    if (depth_generator.IsNewDataAvailable()) {
        depth_generator.WaitAndUpdateData();
        depth_generator.GetMetaData(dmd);
        
        counter2++;
        if (counter2 == 50 || counter2 == 60) {
            cout << "\n\n--------thresholds----------\n" <<
            "this dpthGenerator's NO is " << number <<
            "\nnear : " << thresholds.near <<
            "\nfar : " << thresholds.far <<
            "\nmin : " << thresholds.min <<
            "\nmax : " << thresholds.max << endl;
            
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
//        const XnDepthPixel * depth = changeToRealDistance(dmd.Data());
//        memcpy(depth_map, depth, out_put_modes.nXRes * out_put_modes.nYRes * sizeof(XnDepthPixel));//depth_mapの更新
#ifdef HAVE_TEXTURE
        generateTexture();
#endif
    }
}

//----------------------------------------------
void myDepthGenerator::setThreshold(XnUInt16 const& _near, XnUInt16 const& _far){
    thresholds.near = _near;
    thresholds.far = _far;
}

//----------------------------------------------
XnMapOutputMode& myDepthGenerator::getMapMode() {
    return out_put_modes;
}
//----------------------------------------------
XnDepthPixel * myDepthGenerator::getDepthPixels(){//soDepthThresholds const& thre){
    XnDepthPixel * depth = new XnDepthPixel[sizeof(XnDepthPixel) * out_put_modes.nXRes * out_put_modes.nYRes];
//    for (int i = 0; i < out_put_modes.nXRes * out_put_modes.nYRes; i++) {
//        if (depth_map[i] < thre.near || depth_map[i] > thre.far) {
//            depth[i] = 0;
//        } else depth[i] = depth_map[i];
//    }
    return depth;
}
//----------------------------------------------
//inline
//----------------------------------------------
inline XnDepthPixel* myDepthGenerator::changeToRealDistance(const XnDepthPixel * dep_pixels) {
    XnDepthPixel * p = new XnUInt16[sizeof(dep_pixels) / sizeof(dep_pixels[0])];
    //XnDepthPixel newPixels[sizeof(dep_pixels) / sizeof(dep_pixels[0])];
    for (int i = 0; i < sizeof(dep_pixels) / sizeof(dep_pixels[0]); i++) {
        p[i] = (unsigned short)(0.1236 * tan(dep_pixels[i] / 2842.5 + 1.1863));
    }
    //p = newPixels;
    return p;
}

inline XnUInt16 myDepthGenerator::changeToRealDistance(XnUInt16 const depthValue){
    return (unsigned short)(0.1236 * tan(depthValue / 2842.5 + 1.1863));
}

#ifdef HAVE_TEXTURE
//----------------------------------------------
unsigned char * myDepthGenerator::mapDepthToChar(XnDepthPixel * const depthMap){
    unsigned char * pixels = new unsigned char[out_put_modes.nXRes * out_put_modes.nYRes];
    //TODO: どの領域に移すか
    for (int i = 0; i < out_put_modes.nXRes * out_put_modes.nYRes; i++) {
        pixels[i] = ofMap((float)depthMap[i], (float)thresholds.min, (float)thresholds.max, 0.0, 255.0);
    }
    return pixels;
}
unsigned char * myDepthGenerator::generateTexture() {
    const XnDepthPixel * depth = dmd.Data();
    //XN_ASSERT(depth);
    if (dmd.FrameID() == 0) return;
    
    float max = 255;
	for (XnUInt16 y = dmd.YOffset(); y < dmd.YRes() + dmd.YOffset(); y++) {//上のオフセット位置から一番下まで
		unsigned char * texture = (unsigned char*)depth_texture + y * dmd.XRes() * 4 + dmd.XOffset() * 4;
		for (XnUInt16 x = 0; x < dmd.XRes(); x++, depth++, texture += 4) {
            
            XnUInt8 a = (XnUInt8)(((*depth) / ( thresholds.max / max)));
            texture[0] = a;
            texture[1] = a;
            texture[2] = a;
            
            if (*depth == 0)
                texture[3] = 0;
            else
                texture[3] = 255;
            
        }
    }
    return depth_texture;
}

#endif

















