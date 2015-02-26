#include "ofxEdgerControl.h"

//--------
void ofxEdgerControl::setup() {
    capture = false;
    configure = false;
    visible = true;
    bCameraReady = false;
    setupUI();
    
    //frameTask.setup();
    cameraStatus.setup();
//    captureDownloader.setup();
    

   // addDownloadFinishedListener(this, &ofxEdgerControl::downloadFinish);
    addCameraStateListener(this, &ofxEdgerControl::newState);
    addCameraLevelListener(this, &ofxEdgerControl::newLevel);
    addCameraFlagListener(this, &ofxEdgerControl::newFlag);
}

void ofxEdgerControl::setupDownloader(){
    capture = false;
    configure = false;
    visible = true;
    triggerDownload = false;

    //frameTask.setup();
    captureDownloader.setup();
    
    addDownloadFinishedListener(this, &ofxEdgerControl::downloadFinish);
    
}

//--------
void ofxEdgerControl::exit() {
    //frameTask.exit();
    cameraStatus.exit();
    captureDownloader.exit();
    ofRemoveListener(cameraStatus.newStateEvent, this, &ofxEdgerControl::newState);
    ofRemoveListener(cameraStatus.newLevelEvent, this, &ofxEdgerControl::newLevel);
    ofRemoveListener(cameraStatus.newFlagEvent, this, &ofxEdgerControl::newFlag);
}

//--------
void ofxEdgerControl::update(){

}

void ofxEdgerControl::updateCaptureApp(){
    if(capture){
        capture = false;
        if(camState == "READY"){
            ofHttpResponse reponse = ofLoadURL("http://10.11.12.13/trigger");
            if(reponse.status < 300){
                ofLog(OF_LOG_NOTICE)<<"Capture Triggered"<<endl;
                ofLog(OF_LOG_VERBOSE)<<"trigger returend "<<reponse.status<<" status code"<<endl;
                saveQue.push_back(ofGetTimestampString());
            }else{
                ofLog(OF_LOG_ERROR)<<"trigger returend "<<reponse.status<<" status code"<<endl;
            }
        }else{
            triggerQue.push_back(ofGetTimestampString());
        }
    }
    
    // for capture app -- make sure these conditions are met before sending 'ready' over OSC
    if(camState == "READY" && camState != pCamState && saveProgress == 100){
        if(saveQue.size() > 0){
            saveQue.pop_front();
            download = true;
            // in capture app, if edgertronic.download = true, signal OSC to render app for download
        }
    }
    
    if(configure){
        configure = false;
    }
    
    if(camState == "READY"){
        if(triggerQue.size() > 0){
            ofHttpResponse response = ofLoadURL("http://10.11.12.13/trigger");
            if(response.status < 300){
                ofLog(OF_LOG_NOTICE)<<"Capture Triggered"<<endl;
                ofLog(OF_LOG_VERBOSE)<<"trigger returend "<<response.status<<" status code"<<endl;
            }else{
                ofLog(OF_LOG_ERROR)<<"trigger returend "<<response.status<<" status code"<<endl;
            }
            triggerQue.pop_front();
        }
    }
    
    pCamState = camState;
}

void ofxEdgerControl::updateRenderApp(){
    
    if(download){
        download = false;
        captureDownloader.triggerDownload();
        
    }
    
}

//--------
void ofxEdgerControl::draw() {
//    if (visible) {
//        frameTask.getCurrentFrame()->draw(0, 0, 640, 360);
//    }
}

//--------
void ofxEdgerControl::setupUI(){
    ui = new ofxUISuperCanvas("Edgertronic Control");
    ui->setPosition(10, 10);
    ui->addFPS();
    ui->addSpacer();
    ui->addSlider("Level", 0.0, 100.0, &saveProgress);
    ui->addSpacer();
    ui->addToggle("Trigger", &capture);
    ui->addSpacer();
    uiStatus = ui->addTextArea("Camera Status: ", camState);
    ui->ofxUICanvas::autoSizeToFitWidgets();
    
    ofAddListener(ui->newGUIEvent,this,&ofxEdgerControl::onGuiEvent);
}

//--------
void ofxEdgerControl::setCapture(bool capture) {
    this->capture = capture;
}

//--------
void ofxEdgerControl::toggleCapture() {
    setCapture(!capture);
}

//--------
void ofxEdgerControl::onGuiEvent(ofxUIEventArgs& e){
    string name = e.getName();
    int kind = e.getKind();
}

//--------
void ofxEdgerControl::newState(int & i){
    pCamState = camState;
    if(i == StatusTask::CAMAPI_STATE_CALIBRATING){
        ofLog(OF_LOG_NOTICE)<<"STATE_CALIBRATING"<<endl;
        camState = "CALIBRATING";
    }
    if(i == StatusTask::CAMAPI_STATE_RUNNING){
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_RUNNING"<<endl;
        camState = "RUNNING";
    }
    if(i == StatusTask::CAMAPI_STATE_TRIGGERED){
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_TRIGGERED"<<endl;
        camState = "TRIGGERED";
    }
    if(i == StatusTask::CAMAPI_STATE_SAVING){
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_SAVING"<<endl;
        camState = "SAVING";
    }
    if(i == StatusTask::CAMAPI_STATE_RUNNING_PRETRIGGER_FULL){
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_RUNNING_PRETRIGGER_FULL"<<endl;
        camState = "READY";
        bCameraReady = true;
    }
    if(i == StatusTask::CAMAPI_STATE_TRIGGER_CANCELED){
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_TRIGGER_CANCELED"<<endl;
    }
    if(i == StatusTask::CAMAPI_STATE_SAVE_CANCELED){
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_SAVE_CANCELED"<<endl;
    }
    if(uiStatus != NULL){
        uiStatus->setTextString(camState);
    }
}

//--------
void ofxEdgerControl::newFlag(int & i){
    
}

//--------
void ofxEdgerControl::newLevel(int & i){
    saveProgress = i;
}

//--------
void ofxEdgerControl::downloadStart(float & i){

}

//--------
void ofxEdgerControl::downloadFinish(string & file){

}

void ofxEdgerControl::toggleDownloadReady(){
    download = !download;
}

//--------
void ofxEdgerControl::setGuiVisible(bool visible){
    this->visible = visible;
    ui->setVisible(visible);
}

//--------
void ofxEdgerControl::toggleGuiVisible(){
    setGuiVisible(!visible);
}


