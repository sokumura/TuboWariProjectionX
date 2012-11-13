#include "testApp.h"
//////////////////
//////GLOBAL//////
//xtions
myXtionOperator xtions;
ofTexture tex[XTION_NUM];
soDepthThresholds thresholds[XTION_NUM];
ofTexture monoTexture[XTION_NUM];
int a,b,c,d;
//-xtions
//////////////////
//////////////////

void uiWindow::setup(){
    printf("uiWindow setup()が呼ばれました。\n");
    
    gui.setup("first page");
    for (int i = 0; i < XTION_NUM; i++) {
        
        tex[i].allocate(640, 480, GL_RGBA);
        if(i > 0) gui.addTitle("Xtion No." + ofToString(i + 1)).setNewColumn(true);
        else gui.addTitle("Xtion No." + ofToString(i + 1));
        gui.addContent("depth_map", tex[i]);
        gui.addRangeSlider("thresholds:near&far", thresholds[i].near, thresholds[i].far, thresholds[i].min, thresholds[i].max);
        
    }
    
    gui.loadFromXML();
    
}

void uiWindow::update(){
    if (counter < COUNTER_MAX) {
        printf("uiWindow update()が呼ばれました。\n");
        counter++;
    }
}

void uiWindow::keyPressed(int key){
    if(key>='0' && key<='9') {
        gui.setPage(key - '0');
        gui.show();
    } else {
        switch(key) {
            case OF_KEY_ESC: exitApp(); break;
            case ' ': gui.toggleDraw(); break;
            case '[': gui.prevPage(); break;
            case ']': gui.nextPage(); break;
            case 'p': gui.nextPageWithBlank(); break;
        }
    }
}
void uiWindow::draw(){
    if (counter < COUNTER_MAX) {
        printf("uiWindow draw()が呼ばれました。\n");
    }
    ofBackgroundGradient(ofColor::white, ofColor::gray);
    gui.draw();
    
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::setup(){
    printf("testApp setup() が呼ばれました。\n");
//fenster
	ofxFenster * win = ofxFensterManager::get()->createFenster(400 , 0, 1300, 900, OF_WINDOW);
	win -> addListener(new uiWindow());
	//win -> setBackgroundColor(0,0,0);
    ui.setup();
    ofxFensterManager::get()->getPrimaryWindow()->setWindowPosition(-1279, 182);
    ofxFensterManager::get()->getPrimaryWindow()->toggleFullscreen();
//--fenster
//xtion--
    xtions.setup();
//--xtion
    for (int i = 0; i < XTION_NUM; i++) {
        monoTexture[i].allocate(640, 480, GL_LUMINANCE);
    }
}

//--------------------------------------------------------------
void testApp::update(){
    if (counter < COUNTER_MAX) {
        printf("testApp update()が呼ばれました。\n");
        counter++;
    }
	//xtion--
    xtions.update();
    for (int i = 0; i < XTION_NUM; i++) {
        const unsigned char * p = xtions.getDepthGenerator(i).getMonitorTexture();
        tex[i].loadData(p, 640, 480, GL_RGBA);
    }
    
    //--xtion
    
    //gui
    
    //--gui
    
    for (int i = 0; i < XTION_NUM; i++) {
        const unsigned char * p = xtions.getDepthGenerator(i).getMonoTexture();
        monoTexture[i].loadData(p, 640, 480, GL_LUMINANCE);
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if (counter < COUNTER_MAX) {
        printf("testApp draw()が呼ばれました。\n");
    }
//    tex[0].draw(0.0f, 0.0f);
//    for (int i = 0; i < XTION_NUM; i++) {
//        monoTexture[i].draw(0.0f + i * 650, 500.0f);
//    }
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