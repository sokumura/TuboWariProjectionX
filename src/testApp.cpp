#include "testApp.h"
//////////////////
//////GLOBAL//////
bool td = false;
float myFloat1 = 0.56;
int myInt1 = 10;
int box1 = 3;
bool randomizeButton = true;
float * value;
float myFloat2 = 0.5;
int myInt2 = 6;
bool myBool2 = false;
int box2 = 1;
float myFloat5 = 3.0;
float myFloat6 = 0.3;
float myFloat9 = 0.0054;
bool myBool9 = false;

int nearThreshold = 50;
int farThreshold = 200;
float near = 0.001f;
float far = 0.8f;

ofFloatColor fc = ofFloatColor(0.5,0.5,0.5,0.5);

ofPoint p[4];
ofTexture dTexture;

myXtionOperator xtions;


//////////////////
//////////////////

void uiWindow::setup(){
    dTexture.allocate(640, 480, GL_RGBA);
    p[0].set(ofVec3f(0.0f, 0.0f, 0.0f));
    p[1].set(ofVec3f(200.0f, 0.0f, 0.0f));
    p[2].set(ofVec3f(200.0f, 200.0f, 0.0f));
    p[3].set(ofVec3f(0.0f, 200.0f, 0.0f));
    
    gui.setup("first page");
    
    gui.addTitle("Control Window");
    gui.addRangeSlider("testRenge", nearThreshold, farThreshold, 0, 500).setSmoothing(1.0f);
    gui.addRangeSlider("testFloatRenge", near, far, 0.0f, 1.0f);
    gui.addToggle("Full Screen", td);
    gui.addSlider("myFloat1: noise", myFloat1, 0.0, 1); 
    gui.addSlider("myInt1", myInt1, 100, 200); 
    gui.addComboBox("box1", box1, 12, NULL);
    gui.addButton("Randomize Background", randomizeButton);
    //gui.addColorPicker("BG Color", value);
    
    //gui.addPage();
    gui.addTitle("Another group").setNewColumn(true);
    gui.addColorPicker("colorPickerExample", fc);
	gui.addSlider("myFloat2", myFloat2, 0.0, 1).setSmoothing(0.5);
	gui.addSlider("myInt2", myInt2, 3, 8);
	gui.addToggle("myBool2", myBool2);	
	string titleArray[] = {"Lame", "Alright", "Better", "Best"};
	gui.addComboBox("box2", box2, 4,  titleArray);
    
    gui.addSlider2d("2dSlider Test", p[1], 0.0f, 200.0f, 0.0f, 100.0f);
    
    gui.addPage("My 3rd page");
	gui.addSlider("myFloat5", myFloat5, 0.0, 5);
	gui.addSlider("myFloat6", myFloat6, 0.0, 1);
	gui.addSlider("myFloat9", myFloat9, 0.0, 0.01 ); 
	gui.addToggle("myBool9", myBool9);
    gui.addContent("depth", dTexture);
    gui.addQuadWarper("quadWarperTest", dTexture, p);
    
    gui.setPage(1);
    
}
void uiWindow::keyPressed(int key){
    if(key>='0' && key<='9') {
        gui.setPage(key - '0');
        gui.show();
    } else {
        switch(key) {
            case ' ': gui.toggleDraw(); break;
            case '[': gui.prevPage(); break;
            case ']': gui.nextPage(); break;
            case 'p': gui.nextPageWithBlank(); break;
        }
    }
}
void uiWindow::draw(){
    ofBackgroundGradient(ofColor::white, ofColor::gray);
    gui.draw();
    ofDrawBitmapString("nearThreshold:" + ofToString(nearThreshold) + " farThreshold:" + ofToString(farThreshold), ofGetWindowWidth() - 500, ofGetWindowHeight() - 300);
    
    ofDrawBitmapString("2d:x = " + ofToString(p[1].x) + ", y = " + ofToString(p[1].y), ofGetWindowWidth() - 500, ofGetWindowHeight() - 200);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::setup(){
	ofxFenster * win = ofxFensterManager::get()->createFenster(400 , 0, 1300, 900, OF_WINDOW);
	win -> addListener(new uiWindow());
	//win -> setBackgroundColor(0,0,0);
    ui.setup();
    ofxFensterManager::get()->getPrimaryWindow()->setWindowPosition(-1279, 182);
    ofxFensterManager::get()->getPrimaryWindow()->toggleFullscreen();
//xtion--
    xtions.setup();
//--xtion
}

//--------------------------------------------------------------
void testApp::update(){
	//xtion--
    xtions.update();
    dTexture = xtions.depth_container.depth_texture[0];
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //--xtion
    
    //simple gui
    if(randomizeButton) {
		randomizeButton = false;
		//*value = ofRandomuf();
	}
    //--simple gui
    
}

//--------------------------------------------------------------
void testApp::draw(){
    xtions.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key=='f') ofxFensterManager::get()->getPrimaryWindow()->toggleFullscreen();
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}