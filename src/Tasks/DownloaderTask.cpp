//
//  DownloaderTask.cpp
//  edgertronicControl
//
//  Created by Dan Moore on 8/18/14.
//
//

#include "DownloaderTask.h"
DownloaderTask::DownloaderTask(){
    
}

DownloaderTask::~DownloaderTask(){
    
}

void DownloaderTask::setup(){
    ofAddListener(ofEvents().update, this, &DownloaderTask::update);
    downloadURL = "http://10.11.12.13";
    progress = 0;
    ofRegisterURLNotification(this);
    
}

void DownloaderTask::update(ofEventArgs& args){
    if(downloadFiles.size() > 0){
        ofNotifyEvent(downloadFinished, downloadFiles.front(), this);
        downloadFiles.pop_front();
    }
}

void DownloaderTask::triggerDownload(){
    ofLog(OF_LOG_NOTICE)<<"Download Triggered"<<endl;
    ofNotifyEvent(downloadStarted, downloadFiles.front(), this); //alert UI that download started...
    ofHttpResponse response = ofLoadURL("http://10.11.12.13/get_last_saved_filename");
    ofLog(OF_LOG_NOTICE)<<response.data<<endl;
    
    string newFile = ofToString(response.data);
    if(ofIsStringInString(newFile, "\"")){
        ofStringReplace(newFile, "\"", "");
    }
    if(newFile != lastFileName){
        lastFileName = newFile;
        
        ofLog(OF_LOG_NOTICE)<<"Download url "<<downloadURL+"/static/asattachment"+lastFileName<<endl;
        ofLoadURLAsync(downloadURL+"/static/asattachment"+lastFileName);
    }
    
    
}


void DownloaderTask::exit(){
    ofRemoveListener(ofEvents().update, this, &DownloaderTask::update);
}


void DownloaderTask::urlResponse(ofHttpResponse & response){
    if(response.status==200){
        //string filename = ofGetTimestampString()+".mov";
        string filename = "/Users/Shared/video/"+ofGetTimestampString()+".mov";
        ofFile file(filename, ofFile::WriteOnly);
        file.writeFromBuffer(response.data);
        file.close();
        downloadFiles.push_back(filename);
    }
    
}



