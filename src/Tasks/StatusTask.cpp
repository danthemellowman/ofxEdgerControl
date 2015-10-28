//
//  StatusTask.cpp
//  edgertronicControl
//
//  Created by Dan Moore on 8/18/14.
//
//

#include "StatusTask.h"
StatusTask::StatusTask(){
    
}

StatusTask::~StatusTask(){
    
}

void StatusTask::setup(){
    statusRefresh.registerAllEvents(this);
    ofAddListener(ofEvents().update, this, &StatusTask::update);
    statusURL = "http://10.11.12.13/get_status";
    string uuid = statusRefresh.get(statusURL);

}

void StatusTask::update(ofEventArgs& args){
    
    if(jsonData.swapFront()){
        ofxJSONElement& front = jsonData.getFront();
        pState = state;
        pFlags = flags;
        pLevel = level;
        state = front["state"].asInt();
        flags = front["flags"].asInt();
        level = front["level"].asInt();
        
        
        
        if(pState != state){
            ofNotifyEvent(newStateEvent,state,this);
        }
        if(pFlags != flags){
            ofNotifyEvent(newFlagEvent,flags,this);
        }
        if(pLevel != level){
            ofNotifyEvent(newLevelEvent, level, this);
        }
    }
    
    if(ofGetFrameNum()%16==0){
        string uuid = statusRefresh.get(statusURL);
    }
    
    StatusMap::iterator iter = statusMap.begin();
    
    while (iter != statusMap.end())
    {
        Status& t = iter->second;
        
        if (t.state == Status::SUCCESS)
        {
            statusMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

void StatusTask::exit(){
    ofRemoveListener(ofEvents().update, this, &StatusTask::update);
}

void StatusTask::onTaskQueued(const ofx::TaskQueueEventArgs& args)
{
    // Make a record of the task so we can keep track of its progress.
    Status newQuery;
    newQuery.name = args.getTaskName();
    newQuery.uuid = args.getTaskId();
    statusMap[newQuery.uuid] = newQuery;
    //ofLog(OF_LOG_NOTICE)<< "TASK QUEUED!! " <<" " <<args.getTaskId().toString() << "  "<<args.getState()<< ofGetTimestampString() <<endl;
    //cout<<"Status Map Size: " <<statusMap.size()<<endl;
}

int StatusTask::getStatusQueueSize(){
    return statusMap.size();
}


void StatusTask::onTaskStarted(const ofx::TaskQueueEventArgs& args)
{
    //ofLog(OF_LOG_NOTICE)<< "TASK Started!! " <<" " <<args.getTaskId().toString() << "  "<<args.getState()<< ofGetTimestampString() <<endl;
    

}


void StatusTask::onTaskCancelled(const ofx::TaskQueueEventArgs& args)
{
    //ofLog(OF_LOG_NOTICE)<< "TASK Cancelled!! " <<" " <<args.getTaskId().toString() << "  "<<args.getState()<< ofGetTimestampString() <<endl;
}


void StatusTask::onTaskFinished(const ofx::TaskQueueEventArgs& args)
{
    if (statusMap[args.getTaskId()].state == Status::PENDING)
    {
        //ofLog(OF_LOG_NOTICE)<< "TASK Finished!! " <<" " <<args.getTaskId().toString() << "  "<<args.getState()<< ofGetTimestampString() <<endl;
        statusMap[args.getTaskId()].progress = 1;
        statusMap[args.getTaskId()].state = Status::SUCCESS;
        //cout<<"Status Map Size: " <<statusMap.size()<<endl;
    }
}


void StatusTask::onTaskFailed(const ofx::TaskFailedEventArgs& args)
{
    //ofLog(OF_LOG_NOTICE)<< "TASK FAILED!! " <<" " <<args.getTaskId().toString() << "  "<<args.getState()<< ofGetTimestampString() <<endl;

}


void StatusTask::onTaskProgress(const ofx::TaskProgressEventArgs& args)
{
    //ofLog(OF_LOG_NOTICE)<< "TASK Progress!! " <<" " <<args.getTaskId().toString() << "  "<<args.getState()<< ofGetTimestampString() <<endl;
}


void StatusTask::onClientBuffer(const ofx::HTTP::ClientBufferEventArgs& args)
{
    const ofx::IO::ByteBuffer& buffer = args.getData().getByteBuffer();
    try
    {
        ofxJSONElement& back = jsonData.getBack();
        back.parse(buffer.toString());
        jsonData.swapBack();
    }
    catch (const Poco::Exception& exc)
    {
        ofLogError("StatusTask::onTaskData") << exc.displayText();
    }
}
