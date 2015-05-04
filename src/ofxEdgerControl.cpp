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
    
    didTriggerProperly = true;
    
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
            ofHttpResponse response = ofLoadURL("http://10.11.12.13/trigger");
            if(response.status < 300){
                ofLog(OF_LOG_NOTICE)<<"Capture Triggered"<<endl;
                ofLog(OF_LOG_NOTICE)<<"Time Triggered - capture gate: "<< ofGetTimestampString() <<endl;
                ofLog(OF_LOG_NOTICE)<<"Trigger Returned: " << response.status<<" status code"<<endl;
                saveQue.push_back(ofGetTimestampString()); //say we have one file waiting to be saved
                triggerTimer = ofGetElapsedTimef();
                //Check below if the trigger actually changed camera state - currently getting an issue where camera will return 200 status, but won't actually change state
                didTriggerProperly = false;
                
            }else{
                ofLog(OF_LOG_NOTICE)<<"Trigger Returned: " << response.status<<" status code at "<< ofGetTimestampString()<<endl;
            }
        }else{
            triggerQue.push_back(ofGetTimestampString()); //if camera isn't ready, then put the trigger in the queue to be fired off immediately after it is ready
        }
    }

    
    //if its between 0 and 15 seconds after a trigger and the camera is not in a triggering state or a saving state, then the capture failed. even though the camera returned a 200
    //
    if (!didTriggerProperly && (camState=="TRIGGERED" || camState=="SAVING")) {
        ofLog(OF_LOG_NOTICE) <<"Camera Triggered properly. Current state: " << camState <<endl;
        didTriggerProperly = true;
    }else{
        //if time since trigger is >10 &&
        if(triggerTimer+5>ofGetElapsedTimef() && camState=="READY" && !didTriggerProperly){ //if it has been 10 seconds since a trigger was fired, and camera is in ready state, then something is likely wrong. Entirely Dependent on capture size and download time
            if(ofGetFrameNum()%15==0){
                ofLog(OF_LOG_NOTICE) <<"May be an issue with camera triggering. CURRENT STATE: " << camState<<endl;
            }
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
    
    //Deprecated??
    //This is never typically hit unless the camera is told to trigger when it is not ready yet - in a run of 100 captures, this was never hit
    if(camState == "READY"){
        if(triggerQue.size() > 0){
            ofHttpResponse response = ofLoadURL("http://10.11.12.13/trigger");
            if(response.status < 300){
                ofLog(OF_LOG_NOTICE)<<"Capture Triggered"<<endl;
                ofLog(OF_LOG_NOTICE)<<"Time Triggered - trigque gate: "<< ofGetTimestampString() <<endl;
                ofLog(OF_LOG_NOTICE)<<"Trigger Returned: " << response.status<<" status code"<<endl;
                ofLog(OF_LOG_VERBOSE)<<"trigger returend "<<response.status<<" status code"<<endl;
            }else{
                ofLog(OF_LOG_NOTICE)<<"Trigger Returned: " << response.status<<" status code at "<< ofGetTimestampString()<<endl;
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
        ofLog(OF_LOG_NOTICE)<<"CAMAPI_STATE_TRIGGERED==============================="<<endl;
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


