#pragma once

#include "ofMain.h"
#include "ofxEdgerControl.h"


class ofApp : public ofBaseApp{
    
public:
    ~ofApp();
    void setup();
    void update();
    void draw();
    
    void downloadStart(float & i);
    void downloadFinish(string & file);
    void downloadProgress(float & i);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
private:
    
    ofxEdgerControl edgertronic;
    ofVideoPlayer video;
    
    bool loaded;
};
