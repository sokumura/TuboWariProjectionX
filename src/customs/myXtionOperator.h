//
//  myXtionOperator.h
//  xtionExample
//
//  Created by 奥村 周也 on 12/10/05.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef xtionExample_myXtionOperator_h
#define xtionExample_myXtionOperator_h

#include "myDepthGenerator.h"
#include "ofxOpenNIHeaders.h"
#include "XnCppWrapper.h"

class myXtionOperator {
public:
    
    ofxOpenNIContext context;
    myDepthGenerator depth_container;
    
    void setup(){
        cout << "OpenNI ココカラ☆" << endl;
        XnStatus nRetVal = XN_STATUS_OK;
        nRetVal = context.setup();
        if (nRetVal != XN_STATUS_OK) {
            logErrors(errors);
            ofLogError("OpenNI error : context.Init();", xnGetStatusString(nRetVal));
        }
        
        customEnumerateProductionTrees(context.getXnContext(), XN_NODE_TYPE_DEVICE);
        customEnumerateProductionTrees(context.getXnContext(), XN_NODE_TYPE_DEPTH);
        
        xn::NodeInfoList nodeList;
        nRetVal = context.getXnContext().EnumerateExistingNodes(nodeList);
        if (nRetVal != XN_STATUS_OK) {
            ofLogError("OpenNI error : context.EnumerateExistingNodes();", xnGetStatusString(nRetVal));
            logErrors(errors);
        } else cout << "//////////////nodeExistings Successed!!!!///////////////" << endl;
        int geneNum = 0;
        for (xn::NodeInfoList::Iterator it = nodeList.Begin(); it != nodeList.End(); ++it, geneNum++) {
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
                xn::NodeInfo info = *it;
                bool result = depth_container.setup(info);
                if (!result) {
                    logErrors(errors);
                    ofLogError("OpenNI error: generatorSet : ", xnGetStatusString(nRetVal));
                }
                std::cout << "sensor[" << no << "]のdepthGeneratorを作成!!" << std::endl;
            }
            //logErrors(errors);
        }
        
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!\n%i 個のジェネレーターを作りました\n!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", depth_container.generatorNum);
        depth_container.startGeneratingAll();
    }
    //--------------------------------------
    void update(){
        for (int i = 0; i < depth_container.generatorNum; i++) {
            depth_container.update();
        }
    
    }
    //--------------------------------------
    void draw(){
        for (int i = 0; i < depth_container.generatorNum; i++) {
            depth_container.testDraw(0.0, 0.0, 640.0, 480.0);
        }
    }
    //--------------------------------------
    void customEnumerateProductionTrees(xn::Context& con, XnProductionNodeType type){
        xn::NodeInfoList nodes;
            XnStatus rc = XN_STATUS_OK;
            rc = con.EnumerateProductionTrees(type, NULL, nodes);
            if (rc != XN_STATUS_OK) {
                ofLogError("MY TASK NO.1", xnGetStatusString(rc));
            } else if (nodes.Begin() == nodes.End()) {
                ofLogError("MY TASK NO.1", "No device found.");
            }
            
        for (xn::NodeInfoList::Iterator it = nodes.Begin(); it != nodes.End(); ++it) {
                cout << "MY TASK NO.2 " <<
                xnProductionNodeTypeToString((*it).GetDescription().Type ) << ", " <<
                (*it).GetCreationInfo() << ", " <<
                (*it).GetInstanceName() << ", " <<
                (*it).GetDescription().strName << ", " <<
                (*it).GetDescription().strVendor << ", " << endl;
                
                xn::NodeInfo info = *it;
                con.CreateProductionTree(info);
            }
    }
    
    
    
private:
    xn::EnumerationErrors errors;
        
    //--------------------------------------
    void logErrors(xn::EnumerationErrors& rErrors){
        for(xn::EnumerationErrors::Iterator it = rErrors.Begin(); it != rErrors.End(); ++it) {
            XnChar desc[512];
            xnProductionNodeDescriptionToString(&it.Description(), desc,512);
            printf("%s failed: %s\n", desc, xnGetStatusString(it.Error()));
        }
    }

};

#endif
