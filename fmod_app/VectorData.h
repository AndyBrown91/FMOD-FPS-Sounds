//
//  VectorData.h
//  shooter
//
//  Created by Andy on 05/02/2013.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef shooter_VectorData_h
#define shooter_VectorData_h

/** Keep track of the vector data for a game object.
 This also keeps an array of Events that are already playing at
 this object's position and automatically updates the 3D attributes
 when the object moves. It also removes the Event from the array
 when the Event has finished playing. Functions are provided to start,
 stop and apply "key-off" for a given parameter for all current events.
 You can add more if you need to modify all events.
 */
class VectorData
{
public:
	VectorData()
	{
		pos.x = pos.y = pos.z = 0;
		vel.x = vel.y = vel.z = 0;
		dir.x = dir.y = dir.z = 0;
	}
    
    ~VectorData()
    {
        stopEvents();
    }
	
    bool eventIsLive(Event* event)
    {
        FMOD_EVENT_STATE state;
        FMOD_RESULT error = event->getState(&state);
        
        if((error == FMOD_OK) && (state & FMOD_EVENT_STATE_PLAYING))
        {
            return true;
        }
        else
        {
            // if you are using Juce modules 2.x this needs to be: events.removeFirstMatchingValue(event)
            events.removeValue(event); // event has finished playing
            return false;
        }
    }
    
	/** Update one or more of the vectors. */
	void setVectors(const Vector3 *newPos,
					const Vector3 *newVel,
					const Vector3 *newDir)
	{
		if(newPos) pos = *newPos;
		if(newVel) vel = *newVel;
		if(newDir) dir = *newDir;
		
		for(int i = events.size()-1; i >= 0; i--)
		{
			Event* event = events[i];
			
			if(eventIsLive(event))
			{
				ERRCHECK(event->set3DAttributes(newPos,
												newVel,
												newDir));
			}
		}
	}
	
	const Vector3* getPos() const { return &pos; }
	const Vector3* getVel() const { return &vel; }
	const Vector3* getDir() const { return &dir; }
	
	/** Add an Event playing at this object position. */
	void addEvent(Event* event)
	{
		events.add(event);
		ERRCHECK(event->set3DAttributes(&pos, &vel, &dir));
	}
	
	/** Remove an Event manually. */
	void removeEvent(Event* event)
	{
		events.removeValue(event);
	}
	
    void startEvents()
    {
        for(int i = events.size()-1; i >= 0; i--)
		{
            Event* event = events[i];
            ERRCHECK(event->start());
        }
    }
    
    void stopEvents()
    {
        for(int i = events.size()-1; i >= 0; i--)
		{
            Event* event = events[i];
            
            if(eventIsLive(event))
                ERRCHECK(event->stop());
        }
    }
    
    void setParameter(String const& param, const float value)
    {
        const char* paramString = (const char*)param.toUTF8();
        
        for(int i = events.size()-1; i >= 0; i--)
		{
            Event* event = events[i];
            
            if(eventIsLive(event))
            {
                EventParameter* param;
                ERRCHECK(event->getParameter(paramString, &param));
                ERRCHECK(param->setValue(value));
            }
        }
    }
    
    void parameterKeyOff(String const& param)
    {
        const char* paramString = (const char*)param.toUTF8();
        
        for(int i = events.size()-1; i >= 0; i--)
		{
            Event* event = events[i];
            
            if(eventIsLive(event))
            {
                EventParameter* param;
                ERRCHECK(event->getParameter(paramString, &param));
                ERRCHECK(param->keyOff());
            }
        }
    }
    
private:
	Vector3 pos, vel, dir;
	Array<Event*> events;
};


#endif
