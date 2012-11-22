//
//  myXtionOperator.cpp
//  TuboWariProjection
//
//  Created by Shuya Okumura on 2012/10/22.
//
//

#include "myXtionOperator.h"

myXtionOperator::myXtionOperator(){


}
myXtionOperator::~myXtionOperator(){

}

void myXtionOperator::setup(){
    cout << "OpenNI ココカラ☆" << endl;
    context.setup();
    
    //customEnumerateProductionTrees(context.getXnContext(), XN_NODE_TYPE_DEVICE);
    customEnumerateProductionTrees(context.getXnContext(), XN_NODE_TYPE_DEPTH);
    
    xn::NodeInfoList nodeList;
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = context.getXnContext().EnumerateExistingNodes(nodeList);
    if (nRetVal != XN_STATUS_OK) {
        ofLogError("OpenNI error : context.EnumerateExistingNodes();", xnGetStatusString(nRetVal));
        logErrors(errors);
    } else cout << "//////////////nodeExistings Successed!!!!///////////////" << endl;
    int geneNum = 0;
    for (xn::NodeInfoList::Iterator it = nodeList.Begin(); it != nodeList.End(); ++it) {
        //インスタンス名の最後が番号になってるから、
        std::string name = (*it).GetInstanceName();
        cout << "--------------nodeInfomation------------\nname is : " << name << endl;
        int no = *name.rbegin() - '1';
        cout << "no is : " << no << endl;
        
        std::cout <<
        "GetCreationInfo() : " <<
        (*it).GetCreationInfo() << "\n" <<
        "GetInstanceName() : " <<
        (*it).GetInstanceName() << "\n" <<
        "GetDescription().strName : " <<
        (*it).GetDescription().strName << "\n" <<
        "GetDescription().strVendor : " <<
        (*it).GetDescription().strVendor << "\n" <<
        "-------/nodeInfomation-----" << std::endl;
        
        if ((*it).GetDescription().Type == XN_NODE_TYPE_DEPTH) {
            printf("geneNum : %u\n", geneNum);
            xn::NodeInfo info = *it;
            depth_GRs[geneNum].setup(info, geneNum);
            std::cout << "sensor[" << no << "]のdepthGeneratorを作成!!" << std::endl;
            printf("depth_GRs[%u].getMapMode().nFPS : %u", geneNum, depth_GRs[geneNum].getMapMode().nFPS);
            depth_GRs[geneNum].startGenerating();
            geneNum++;
        }
    }
    generatorNum = geneNum;
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!\n%i 個のジェネレーターを作りました\n!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", generatorNum);
    
    //mesh
    vboMesh.setMode(OF_PRIMITIVE_POINTS);
    
}
//--------------------------------------
void myXtionOperator::update(){
    for (int i = 0; i < generatorNum; i++) {
        thresholds[i].near = thresholdNear[i];
        thresholds[i].far = thresholdFar[i];
        depth_GRs[i].update(thresholds[i]);
    }
    
}
//--------------------------------------
void myXtionOperator::testDraw(float x, float y, float w, float h){
    
}
//--------------------------------------
void myXtionOperator::customEnumerateProductionTrees(xn::Context& con, XnProductionNodeType type){
    xn::NodeInfoList nodes;
    XnStatus rc = XN_STATUS_OK;
    rc = con.EnumerateProductionTrees(type, NULL, nodes);
    if (rc != XN_STATUS_OK) {
        ofLogError("MY TASK NO.1", xnGetStatusString(rc));
    } else if (nodes.Begin() == nodes.End()) {
        ofLogError("MY TASK NO.1", "No device found.");
    }
    
    for (xn::NodeInfoList::Iterator it = nodes.Begin(); it != nodes.End(); ++it) {
        cout << "ProductionTreeの列挙\n" <<
        xnProductionNodeTypeToString((*it).GetDescription().Type ) << ", " <<
        (*it).GetCreationInfo() << ", " <<
        (*it).GetInstanceName() << ", " <<
        (*it).GetDescription().strName << ", " <<
        (*it).GetDescription().strVendor << ", " << endl;
        
        xn::NodeInfo info = *it;
        con.CreateProductionTree(info);
    }
}

//-----------------------------------------
myDepthGenerator& myXtionOperator::getDepthGenerator(int index) {
    return depth_GRs[index];
}
//--------------------------------------
void myXtionOperator::logErrors(xn::EnumerationErrors& rErrors){
    for(xn::EnumerationErrors::Iterator it = rErrors.Begin(); it != rErrors.End(); ++it) {
        XnChar desc[512];
        xnProductionNodeDescriptionToString(&it.Description(), desc,512);
        printf("%s failed: %s\n", desc, xnGetStatusString(it.Error()));
    }
}