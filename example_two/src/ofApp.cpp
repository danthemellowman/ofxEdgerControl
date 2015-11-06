#include "ofApp.h"

//---------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_NOTICE);
    edgertronic.setup();
    edgertronic.addDownloadFinishedListener(this, &ofApp::downloadFinish);
}

//---------
void ofApp::update(){
    edgertronic.update();
    if (loaded) {
        video.update();
    }
}

//---------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    edgertronic.draw();
    if (loaded) {
        float width = 640;
        float height = video.getHeight()/video.getWidth()*width;
        video.draw(640, 0, width, height);
    }
}


//--------
void ofApp::downloadFinish(string & file){
    loaded = true;
    video.loadMovie(file);
    video.play();
}


//---------
void ofApp::keyPressed(int key){
    if      (key=='t')  edgertronic.toggleCapture();
    else if (key=='g')  edgertronic.toggleGuiVisible();
}

//---------
void ofApp::keyReleased(int key){
    
}

//---------
void ofApp::mouseMoved(int x, int y ){
    
}

//---------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//---------
void ofApp::mousePressed(int x, int y, int button){
    
}

//---------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//---------
void ofApp::windowResized(int w, int h){
    
}

//---------
void ofApp::gotMessage(ofMessage msg){
    
}

//---------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//---------
ofApp::~ofApp(){
    edgertronic.exit();
}

