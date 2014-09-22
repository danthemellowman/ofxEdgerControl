#include "ofxEdgerControl.h"

//--------
void ofxEdgerControl::setup() {
    capture = false;
    download = false;
    configure = false;
    visible = true;
    setupUI();
    
    //frameTask.setup();
    cameraStatus.setup();
    captureDownloader.setup();
    
    addDownloadStartedListener(this, &ofxEdgerControl::downloadStart);
    addDownloadProgressListener(this, &ofxEdgerControl::downloadProgress);
    addDownloadFinishedListener(this, &ofxEdgerControl::downloadFinish);
    addCameraStateListener(this, &ofxEdgerControl::newState);
    addCameraLevelListener(this, &ofxEdgerControl::newLevel);
    addCameraFlagListener(this, &ofxEdgerControl::newFlag);
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
    
    if(camState == "READY" && camState != pCamState && saveProgress == 100){
        if(saveQue.size() > 0){
            saveQue.pop_front();
            download = true;
        }
    }
    
    if(configure){
        configure = false;
    }
    
    if(download){
        download = false;
        captureDownloader.triggerDownload();
        
    }
    
    if(camState == "READY"){
        if(triggerQue.size() > 0){
            ofHttpResponse reponse = ofLoadURL("http://10.11.12.13/trigger");
            if(reponse.status < 300){
                ofLog(OF_LOG_NOTICE)<<"Capture Triggered"<<endl;
                ofLog(OF_LOG_VERBOSE)<<"trigger returend "<<reponse.status<<" status code"<<endl;
            }else{
                ofLog(OF_LOG_ERROR)<<"trigger returend "<<reponse.status<<" status code"<<endl;
            }
            triggerQue.pop_front();
        }
    }
    
    pCamState = camState;
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
    ui->addSlider("Download Progress", 0.0, 100.0, &loadProgress);
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
    loadProgress = i * 100.0;   // percent

}

//--------
void ofxEdgerControl::downloadFinish(string & file){

}

//--------
void ofxEdgerControl::downloadProgress(float & i){
    loadProgress = i * 100.0;   // percent
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


