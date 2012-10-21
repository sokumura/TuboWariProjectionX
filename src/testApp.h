#pragma once

#include "ofMain.h"
#include "ofxFensterManager.h"
#include "ofxOpenCv.h"
#include "customs/myXtionOperator.h"
#include "ofxSimpleGuiToo.h"

class uiWindow : public ofxFensterListener {
public:
    
    void setup();
    
    void update(){
    
    }
    
    void draw();
    void keyPressed(int key);
    
};

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    

    //fenster
    uiWindow ui;
    
		
};
