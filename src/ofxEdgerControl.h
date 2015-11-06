#pragma once

#define CAMIP "http://10.11.12.13"

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"
#include "FrameTask.h"
#include "StatusTask.h"
#include "DownloaderTask.h"


class ofxEdgerControl
{
public:
    void setup();
    void setupDownloader();
    void exit();
    void update();
    
    void updateAll();
    void updateCaptureApp(); //used in older versions when the apps were seperated
    void updateRenderApp(); //used in older versions when the apps were seperated
    
    void updateAndDownload();
    
    void draw();
    
    void setCapture(bool capture);
    void toggleCapture();
    
    string getCamState() {return camState;}
    bool getDidTriggerProperly(){return didTriggerProperly;}
    
    void newState(int & i);
    void newFlag(int & i);
    void newLevel(int & i);
    
    template <typename ListenerClass, typename ListenerMethod>
    void addDownloadFinishedListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(captureDownloader.downloadFinished, listener, method);
    }
    template <typename ListenerClass, typename ListenerMethod>
    void addDownloadStartedListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(captureDownloader.downloadStarted, listener, method);
    }
    template <typename ListenerClass, typename ListenerMethod>
    void addCameraStateListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(cameraStatus.newStateEvent, listener, method);
    }
    template <typename ListenerClass, typename ListenerMethod>
    void addCameraLevelListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(cameraStatus.newLevelEvent, listener, method);
    }
    template <typename ListenerClass, typename ListenerMethod>
    void addCameraFlagListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(cameraStatus.newFlagEvent, listener, method);
    }
    
    void downloadStart(string & i);
    void downloadFinish(string & file);
    
    
    void onGuiEvent(ofxUIEventArgs& e);
    
    void setGuiVisible(bool visible);
    void setReadyToDownload(bool _readyToDownload){readyToDownload=_readyToDownload;}
    bool getReadyToDownload(){return readyToDownload;}
    void setCameraReady(bool _bCameraReady){bCameraReady=_bCameraReady;}
    bool getCameraReady(){return bCameraReady;}
    
    void toggleGuiVisible();
    
    void toggleDownloadReady();
    
    int getTrigQueSize(){return triggerQue.size();};
    int getSaveQueSize(){return saveQue.size();};\
    int getStatusTaskSize();
    
    
private:
    
    bool bCameraReady;
    FrameTask frameTask;
    StatusTask cameraStatus;
    DownloaderTask captureDownloader;
    ofVideoPlayer videoPreview;
    bool download;
    bool loaded;
    void setupUI();
    ofxUISuperCanvas* ui;
    ofxUITextArea* uiStatus;
    
    deque<string> triggerQue;
    deque<string> saveQue;
    string camState, pCamState;
    float saveProgress;
    float loadProgress;
    
    float triggerTimer;
    bool didTriggerProperly;
    
    float desiredFramerate, desiredWidth, desiredHeight;
    float desiredISO, desiredShutter, desiredDuration;
    float preTrigger;
    
    bool capture, configure, triggerDownload, readyToDownload;
    
    bool visible;
};

