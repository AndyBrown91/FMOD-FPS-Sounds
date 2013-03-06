/** @mainpage
 
 Shooter game project documentation.
 
 @image html shooter.png "The Shooter Game"
 
 @section Overview
 See MainComponent documentation for the specific messages sent by the @c shootergame.app (Mac) or
 @c shootergame.exe (Windows). This project is designed as a shell project into which sounds can be
 impemented (e.g., in FMOD). Messages are sent from the game which can be used to trigger
 and control sounds.
 
 @section GameControl Game control
 Commands are:
 - <b>arrow keys</b>: move around (or a/d/s/w)
 - <b>shift key</b>: run (in conjunction with the arrow keys)
 - <b>space bar</b>: jump
 - <b>mouse pointer</b>: change viewing angle (look around)
 - <b>mouse button (left)</b>: shoot (reload is automatic when the gun needs reloading)
 - <b>1</b>: Choose gun
 - <b>2</b>: Choose grenade launcher
 - <b>alt key</b>: zoom (for shooting)
 - <b>control key</b>: toggle crouching / standing position
 - <b>esc, m or p</b>: menu (pauses game)
 
 @section ToImplement Sounds to implement
 The main sounds to implement are:
 - soldier footsteps for different surfaces
 - weapon firing and reloading sounds for both gun and grenades
 - bullet strikes for different surfaces
 - grenade explosions
 - various water sounds
 - reverbs for certain locations like buildings and above/below the bridge
 - overall ambience 
 */

#ifndef _MAINCOMPONENT_H_
#define _MAINCOMPONENT_H_

// include the FMOD and Juce headers at set up some other bits and pieces
#include "headers.h"

#include "GameEngineServer.h"
#include "PointerDictionary.h"
#include "VectorData.h"

/** Designed to work with the @c shootergame.app or @c shootergame.exe provided.
 
 Here you recevie messages from the game about objects within the game. The
 messages you will receive are documented in the individual functions.
 Certain functions document value ranges for some of the game parameters. Do
 not assume that the game will only send these values - make sure your sound
 engine behaves correctly even if values out of this range are received. You can
 either clip the value to the desired range or ensure large or smaller values
 can be employed. 
*/
namespace Strings
{
//Unique items
    static const String Bullet = "bullet";
    static const String Grenade = "grenade";
    static const String Soldier = "soldier";
    static const String Camera = "camera";
    static const String ElectricBox = "generator";
    
//Handle String identifiers    
    static const String Water = "water";
    static const String WaterImpact = "impact";
    static const String WaterJump = "jump";
    
    static const String Gun = "gun";
    static const String GunFire = "fire";
    static const String GunReload = "reload";
    static const String GunEmpty = "empty";
    static const String GrenadeExplode = "explode";
    
//Objects
    static const String ObjectRiver = "river";
    static const String ObjectWaterfall = "waterfall";
    static const String ObjectSmallWaterfall = "smallwaterfall";
    static const String ObjectSmallHouse = "smallhouse";
    static const String ObjectLargeHouse = "largehouse";
    static const String ObjectOverBridge = "overbridge";
    static const String ObjectUnderBridge = "underbridge";
    static const String ObjectBrick = "brick";
    static const String ObjectInkCan = "inkcan";
    static const String ObjectBarrel = "barrel";
    static const String ObjectChair = "chair";
    static const String ObjectNoticeboard = "noticeboard";
    static const String ObjectCabinet = "cabinet";
    static const String ObjectTyre = "tyre";
    
//Handle Vectors
    static const String VectorPosition = "pos";
    static const String VectorVelocity = "vel";
    static const String VectorDirection = "dir";
    static const String VectorUp = "up";
    
//Sound locations
    static const String  GunsLocation = "shooter/guns/";
    static const String  FootstepLocation = "shooter/footsteps/";
    static const String  WaterLocation = "shooter/water/";
    static const String  AtmosLocation = "shooter/atmosphere/";
    static const String  CollisionsLocation = "shooter/collisions/";
    
// C Strings
    static const char* FEVFile       = "shooter.fev";
    static const char* BirdsFlying = "shooter/atmosphere/birdsFlying";
    static const char* RunningBreath = "shooter/atmosphere/breathing";
    
//FMOD Parameters
    static const char* Velocity = "velocity";
    static const char* ExplodeDistance = "explodeDistance";
    static const char* RunningParam = "runningCounter";
    static const char* BirdCounter = "birdCounter";
}

namespace Globals {
    //Whether the soldier is in the water
    //Whether they're using the grenadelauncher or gun
    //Whether soldier is running
    bool inWater, grenadeLauncher, grenadeWater, running;
    //Number of rivers created
    //Keeps track of how long soldier has been running, for out of breath type sounds
    //A counter for playing the sounds of birds flying away once the gun hasn't been fired for a set amount of time
    int riverCounter, runningCounter, birdCounter;
}

//The amount of ticks that must have past since the last gun shot for the birds to fly away again
#define birdCounterTrigger 750

//typedef for storing a dictionary of Vector locations and FMOD events related to each object
typedef PointerDictionary<VectorData> VectorDictionary;

class MainComponent  :	public Component,
                        public GameEngineServer
{
private:
	// FMOD objects
    EventSystem* eventsystem;	
    
	Event* atmos;
    //Global so it can be turned off when birds fly away
    Event* birdEvent;
    //Loads sound event on start up to stop delay when gun is fired
    Event* birdsFlying;
    //Global to allow param control in footsteps function, but create in handleCreate
    Event* runningEvent;
    
	EventReverb* underBridgeReverb1;
    EventReverb* underBridgeReverb2;
    EventReverb* smallHouseReverb;
    EventReverb* largeHouseReverb;
    
    //Contains the vector data of all objects in the game
    VectorDictionary objects;
    
    enum Commands
	{
		Quit
	};
    
public:
	MainComponent ()
	:	eventsystem(0),
    atmos(0)
	{
		// launch the game app
		launchGame();
	}
	
	~MainComponent ()
	{
		deleteAllChildren();
	}
    
	void initFMODEvent()
	{				
		// setup FMOD and load an FEV file
		ERRCHECK(EventSystem_Create(&eventsystem));
        
		// initialise FMOD and its event system
		ERRCHECK(eventsystem->init(256, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL));
		
		// define our resources path (on the Mac this is within the app bundle)
		String resourcesPath = getResourcesPath();
		
		// set the media path for the FEV and FSB files to be found
		ERRCHECK(eventsystem->setMediaPath(resourcesPath.toUTF8())); 
		
		// load an event file
		ERRCHECK(eventsystem->load(Strings::FEVFile, 0, 0));
        
		// create reverb
		ERRCHECK(eventsystem->createReverb(&underBridgeReverb1));
        ERRCHECK(eventsystem->createReverb(&underBridgeReverb2));
        ERRCHECK(eventsystem->createReverb(&smallHouseReverb));
        ERRCHECK(eventsystem->createReverb(&largeHouseReverb));
        
        // get the reverb properties set up in FMOD designer
		FMOD_REVERB_PROPERTIES smallHouseProperties = FMOD_PRESET_LIVINGROOM;
        FMOD_REVERB_PROPERTIES largeHouseProperties = FMOD_PRESET_ROOM;

		// ..and apply them to our reverb
        ERRCHECK(smallHouseReverb->setProperties(&smallHouseProperties));
        ERRCHECK(largeHouseReverb->setProperties(&largeHouseProperties));
        
		// get the preset reverb property
		FMOD_REVERB_PROPERTIES underBridgeProperties = FMOD_PRESET_STONECORRIDOR;/*FMOD_PRESET_CAVE;*/
		// ..and apply them to our reverb
		ERRCHECK(underBridgeReverb1->setProperties(&underBridgeProperties));
        ERRCHECK(underBridgeReverb2->setProperties(&underBridgeProperties));
		
		// set the "ambient" reverb
		FMOD_REVERB_PROPERTIES ambientProperties = FMOD_PRESET_PLAIN;
		ERRCHECK(eventsystem->setReverbAmbientProperties(&ambientProperties));
        
        //Sets bird counter so it will go off for the first gun shot
        Globals::birdCounter = birdCounterTrigger;
        ERRCHECK(eventsystem->getEvent(Strings::BirdsFlying,
                                       FMOD_EVENT_DEFAULT, 
                                       &birdsFlying));
	}
	
	void shutdownFMODEvent()
	{
		ERRCHECK(underBridgeReverb1->release());
        ERRCHECK(underBridgeReverb2->release());
        ERRCHECK(smallHouseReverb->release());
        ERRCHECK(largeHouseReverb->release());
		ERRCHECK(eventsystem->release());
		eventsystem = 0;
	}	
	
	void tick()
	{
		// this is called by the ConnectionServer thread every few milliseconds
		
		if(eventsystem) // make sure we have an event system running
		{
			ERRCHECK(eventsystem->update()); // need to call this regularly, docs say once per "frame"
            
            //Checks if soldier is running, increases counter
            if (Globals::running)
            {
                if (Globals::runningCounter < 1200)
                    Globals::runningCounter++;
            }
            else
            {
                //Used for checking how long the soldier has been resting for, so the soldier can't stop for a second and suddenly be completely re-energised
                if(Globals::runningCounter > 0)
                    Globals::runningCounter = Globals::runningCounter - 2;
            }
            
            Globals::birdCounter++;
            
            if (Globals::birdCounter < birdCounterTrigger)
            {
                if (birdEvent != nullptr)
                {
                    //Makes the bird sounds fade in after they have flown away, but stops counting once the flying away sound is ready to be triggered again
                    EventParameter* param;
                    ERRCHECK(birdEvent->getParameter(Strings::BirdCounter, &param));
                    //DBG("Setting bird counter to " << Globals::birdCounter);
                    ERRCHECK(param->setValue(Globals::birdCounter));
                }
            }
            
            if (Globals::runningCounter < 1200)
            {
                //If soldier has been running for longer than 1200 ticks, no longer change the parameter as by that point the breathing is at max speed/volume
                
                
                if (runningEvent != nullptr)
                {
                    bool test;
                    runningEvent->getPaused(&test);
                    if (!test)
                    {
                        EventParameter* param;
                        ERRCHECK(runningEvent->getParameter(Strings::RunningParam, &param));
                        ERRCHECK(param->setValue(Globals::runningCounter));
                    }
                }
            }  
		}
	}
    
    //Used to give each instance of a barrel or brick etc. a unique name for storing in positions dictionary
    String makeUniqueString(String const& name, int gameObjectInstanceID)
	{
		return name + String(gameObjectInstanceID);
	}
	
	
	void handleConnect()
	{
		initFMODEvent();
		
        String atmosEvent = Strings::AtmosLocation+"atmos";
        
		ERRCHECK(eventsystem->getEvent(atmosEvent.toUTF8(), FMOD_EVENT_DEFAULT, &atmos));
		ERRCHECK(atmos->start());		
        
        //Create vector data pointers for bullet and grenade, No handleCreate is ever called for them, but their vectordata is important
        objects.add(Strings::Bullet, new VectorData());
        objects.add(Strings::Grenade, new VectorData());
        
        //Creates vector data for electricity pylon for hum
        objects.add(Strings::ElectricBox, new VectorData());
        //Position electric box
        VectorData* electricBox = objects.get(Strings::ElectricBox);
        Vector3* vector = new Vector3();
        vector->x = -63.6690102;
        vector->y = -2.22161102;
        vector->z = -123.804001;
        electricBox->setVectors (vector, nullptr, nullptr);
        String electricString = Strings::AtmosLocation+Strings::ElectricBox;
        Event* event;
        ERRCHECK(eventsystem->getEvent(electricString.toUTF8(),
                                       FMOD_EVENT_DEFAULT, 
                                       &event));
        
        electricBox->addEvent(event);
        ERRCHECK(event->start());
        
        //Sets the river counter, used to give different rivers a different sound
        Globals::riverCounter = 0;
	}
	
	void handleDisconnect()
	{
		// see what state the event is in now...
		FMOD_EVENT_STATE initialState, newState;
		ERRCHECK(atmos->getState(&initialState));
		
		// stop the event (it's a fading event though)
		ERRCHECK(atmos->stop());
        
		// wait for the event to fade out
		do
		{
			// need to manually do update() here to make sure the fade out actually happens
			ERRCHECK(eventsystem->update());
			
			// sleep a while (15ms shoudl be OK) so we don't hammer the system 
			Thread::sleep(15);
			
			// see what state the event is in now
			ERRCHECK(atmos->getState(&newState));
		}
		while(initialState == newState); // exit the loop if the state changes (i.e., it has stopped)

        objects.clearAndDelete();
        
        //Shuts down FMOD
		shutdownFMODEvent();
        
        // this calls handleCommandMessage with argument Quit but executes on the
		// message thread rather than the network thread..
		// close this Juce app when the game disconnects
		postCommandMessage(Quit);
	}
    
    void handleCommandMessage(int commandId)
	{
		if(commandId == Quit)
		{
			JUCEApplication::getInstance()->systemRequestedQuit();
		}
	}
	
	/** Messages to indicate game objects have been created in the game world.
	 Not all game objects are reported, only those which may emit/receive sounds 
	 in 3D positions or positions of reverb locations.
	 @param name	One of the following:
					 - @c camera:					the camera and listener
					 - @c soldier:					the player character
					 - @c river:					river objects
					 - @c waterfall:				large waterfall
					 - @c smallwaterfall:			small waterfall
					 - @c smallhouse:				the small house
					 - @c largehouse:				the large house
					 - @c overbridge:				ontop of the bridge
					 - @c underbridge:				under the bridge
					 - @c brick:					various concrete bricks
					 - @c inkcan:					the small cans in the small house
					 - @c barrel:					various barrels
					 - @c chair:					chair in the large house
					 - @c noticeboard:				in the large house
					 - @c cabinet:					cabinets in the large house
					 - @c tyre:						various tyres
					<br><br>
	 @param gameObjectInstanceID
					needed for many of these objects
	 */
	void handleCreate(String const& name, int gameObjectInstanceID)
	{
        
        DBG("Creating " << name);
        String uniqueString;
        if (name == Strings::Soldier || name == Strings::Camera)
        {
            uniqueString = name;
        }
        else
            uniqueString = makeUniqueString(name, gameObjectInstanceID);
        //Creates unique names for all repeated objects
        
        //Adds items to objects so they can be accessed
        
        objects.add(uniqueString, new VectorData());
        
        
        if (name == Strings::Soldier)
        {
            VectorData* soldier = objects.get(uniqueString);
            if (soldier)
            {
                //Adds randomly positioned bird sounds which follow the soldier but trigger at random distances/3d positions
                String birds = Strings::AtmosLocation + "birds";
                
                
                ERRCHECK(eventsystem->getEvent(birds.toUTF8(),
                                               FMOD_EVENT_DEFAULT,
                                               &birdEvent));
                EventParameter* birdParam;
                ERRCHECK(birdEvent->getParameter(Strings::BirdCounter, &birdParam));
                ERRCHECK(birdParam->setValue(Globals::birdCounter));
                
                soldier->addEvent(birdEvent);
                ERRCHECK(birdEvent->start());

                //Adds running sounds to soldier, constantly looping, param is set every tick                   
                ERRCHECK(eventsystem->getEvent(Strings::RunningBreath,
                                                   FMOD_EVENT_DEFAULT, 
                                                   &runningEvent));
                    
                EventParameter* param;
                ERRCHECK(runningEvent->getParameter(Strings::RunningParam, &param));
                ERRCHECK(param->setValue(Globals::runningCounter));
                
                soldier->addEvent(runningEvent);
                ERRCHECK(runningEvent->start());
                
            }
            
        }
	}
	

	/** Messages to indicate that objects created via handleCreate() should be
	 deleted.
	 
	 @param name	One of the following:
					 - @c camera:					the camera and listener
					 - @c soldier:					the player character
					 - @c river:					river objects
					 - @c waterfall:				large waterfall
					 - @c smallwaterfall:			small waterfall
					 - @c smallhouse:				the small house
					 - @c largehouse:				the large house
					 - @c overbridge:				ontop of the bridge
					 - @c underbridge:				under the bridge
					 - @c brick:					various concrete bricks
					 - @c inkcan:					the small cans in the small house
					 - @c barrel:					various barrels
					 - @c chair:					chair in the large house
					 - @c noticeboard:				in the large house
					 - @c cabinet:					cabinets in the large house
					 - @c tyre:						various tyres
					<br><br>
	 @param gameObjectInstanceID
					needed for many of these objects
	 */
	void handleDestroy(String const& name, int gameObjectInstanceID)
	{        
        VectorData* vecData = 0;

        //Creates unique names for all objects, only really required for rivers, cans, barrels, bricks, chairs, tyres, cabinets
        String uniqueString = makeUniqueString(name, gameObjectInstanceID);
        
        //Removes object from dictionary, VectorData calls stop events in the destructor, so no call needed
        vecData = objects.remove(uniqueString);

		if(vecData)
			delete vecData;
	}
		
	/** Vectors from the game for 3D positionable objects.
	 Some objects which don't move will only be reported at the start of the game.
	 Other objects (like the soldier and camera) will move. 
	 
	 @param name	One of the following:
					 - @c camera:					the camera and listener
					 - @c soldier:					the player character
					 - @c river:					river objects
					 - @c waterfall:				large waterfall
					 - @c smallwaterfall:			small waterfall
					 - @c smallhouse:				the small house
					 - @c largehouse:				the large house
					 - @c overbridge:				ontop of the bridge
					 - @c underbridge:				under the bridge
					 - @c brick:					various concrete bricks
					 - @c inkcan:					the small cans in the small house
					 - @c barrel:					various barrels
					 - @c chair:					chair in the large house
					 - @c noticeboard:				in the large house
					 - @c cabinet:					cabinets in the large house
					 - @c tyre:						various tyres
					 - @c bullet:					a bullet just before it hits the target (@e param @c pos only)
					 - @c grenade:					a grenade just before it explodes (@e param @c pos only)
					<br><br>
	 @param gameObjectInstanceID
					needed for many of these objects
					<br><br>
	 @param param	One of the following:
					- @c pos: position in m
					- @c vel: velocity in m/s
					- @c dir: direction facing
					- @c up:  where is the up direction (@e name @c camera only)
					<br><br>
	 @param vector	Stucture containing the vector data.
					- @c x: @e vector->x
					- @c y: @e vector->y
					- @c z: @e vector->z
	 */

	void handleVector(String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector)
	{
        String uniqueString;
        if (name == Strings::Camera)
        {
            handleCameraVector (param, vector);
        }
        else if (name == Strings::ObjectWaterfall || name == Strings::ObjectSmallWaterfall || name == Strings::ObjectSmallHouse || name == Strings::ObjectUnderBridge || name == Strings::ObjectOverBridge || name == Strings::ObjectLargeHouse ||  name == Strings::ObjectRiver)
        {
            //Sets vector data for objects which do not move
            handleStaticVector(name, gameObjectInstanceID, param, vector);
        }
        
        else
        {
            uniqueString = makeUniqueString(name, gameObjectInstanceID);
            
            if (name == Strings::Soldier || name == Strings::Bullet || name == Strings::Grenade)
            {
                uniqueString = name;
            }
        
            if (param == Strings::VectorPosition) {
                //Updates objects dictionary with new position for item
                objects.get(uniqueString)->setVectors(vector, nullptr, nullptr);
            }
            if (param == Strings::VectorVelocity) {
                //Updates objects dictionary with new velocity for item
                objects.get(uniqueString)->setVectors(nullptr, vector, nullptr);
            }
            if (param == Strings::VectorDirection) {
                //Updates objects dictionary with new direction for item
                objects.get(uniqueString)->setVectors(nullptr, nullptr, vector);
            }    

        }        
    }
    
    void handleCameraVector (String const& param, const Vector3* vector)
    {
        // the camera and listener
        
        if(param == Strings::VectorPosition) {
            ERRCHECK(eventsystem->set3DListenerAttributes(FMOD_MAIN_LISTENER,
                                                          vector, 0, 0, 0));
        }
        else if(param == Strings::VectorVelocity) {
            ERRCHECK(eventsystem->set3DListenerAttributes(FMOD_MAIN_LISTENER,
                                                          0, vector, 0, 0));
        }
        else if(param == Strings::VectorDirection) {
            ERRCHECK(eventsystem->set3DListenerAttributes(FMOD_MAIN_LISTENER,
                                                          0, 0, vector, 0));
        }
        else if(param == Strings::VectorUp) {
            ERRCHECK(eventsystem->set3DListenerAttributes(FMOD_MAIN_LISTENER,
                                                          0, 0, 0, vector));
        }
        
    }
    
    void handleStaticVector (String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector)
    {
        String uniqueString = makeUniqueString(name, gameObjectInstanceID);
        //Only sets position as objects do not move, therefore no velocity or direction
        if (param == Strings::VectorPosition)
        {
            if (name == Strings::ObjectRiver || name == Strings::ObjectSmallWaterfall || name == Strings::ObjectWaterfall)
            {
                VectorData* objectData = objects.get(uniqueString);
                if (objectData)
                {
                    objectData->setVectors(vector, nullptr, nullptr);
                }
                startLooping (name, gameObjectInstanceID);
            }
            
            if (name == Strings::ObjectSmallHouse)
            {
                ERRCHECK(smallHouseReverb->set3DAttributes(vector, 4, 6));
            }
            
            if (name == Strings::ObjectLargeHouse)
            {
                ERRCHECK(largeHouseReverb->set3DAttributes(vector, 9, 10.5));
            }
            
            if (name == Strings::ObjectUnderBridge)
            {
                
                //Checks to see if underBridge1 has been set, if it has, set underbridge 2 with incoming position
                float minCheck;
                ERRCHECK(underBridgeReverb1->get3DAttributes(0, &minCheck, 0));
                if(minCheck == 0)
                {
                    DBG("Settings " << name <<1<< " to position" << vector->x << " " << vector->y << " " << vector->z);
                    // set the position properties in game world units (metres here)                    
                    ERRCHECK(underBridgeReverb1->set3DAttributes(vector, 10, 16));
                }
                else
                {
                    DBG("Settings " << name <<2<< " to position" << vector->x << " " << vector->y << " " << vector->z);
                    ERRCHECK(underBridgeReverb2->set3DAttributes(vector, 10, 16));
                }
                
            }
        }
    }

    
    void startLooping (String const& name, int gameObjectInstanceID)
    {
        String uniqueString = makeUniqueString(name, gameObjectInstanceID);
        
        name == Strings::ObjectRiver ? (Globals::riverCounter++) : (Globals::riverCounter);
        
        if (name == Strings::ObjectWaterfall || name == Strings::ObjectSmallWaterfall || name == Strings::ObjectRiver)
        {
            VectorData* waterData = objects.get(uniqueString);
            if(waterData)
            {
                waterData->stopEvents();
                
                String waterString;
                //Allows different sounds to be used for each river section, stream at the top, bigger river under the bridge and by dam
                if (Globals::riverCounter < 2)
                    waterString = Strings::WaterLocation+name;
                else    
                    waterString = Strings::WaterLocation+name+String(2);
                
                DBG("Starting = " << waterString);
                Event* event;
                
                ERRCHECK(eventsystem->getEvent(waterString.toUTF8(),
                                               FMOD_EVENT_DEFAULT, 
                                               &event));
                
                waterData->addEvent(event);
                ERRCHECK(event->start());
            }
        }
    }
    
	/** String type messages from the game (various).
	 
	 @param name		One of the following:
						- @c soldier
						<br><br>
	 @param gameObjectInstanceID	Not really needed here.
						<br><br>
	 @param param		One of the following:
						- @c water: when name is "soldier"
						- @c gun: when name is "soldier"
						<br><br>
	 @param content		One of the following:
						<br><br>
						For @e soldier.water <br><br><ul>
						<li>@c impact:		the soldier hit the water</li>
						<li>@c jump:		the soldier jumped in the water</li>
						</ul>
						For @e soldier.gun <ul>
						<li>@c fire:		the gun was fired</li>
						<li>@c empty:		the gun is out of ammo</li>
						<li>@c reload:		the gun is being reloaded</li>
						</ul>
	 */
	void handleString(String const& name, int gameObjectInstanceID, String const& param, String const& content)
	{
        if (name == Strings::Soldier)
        {
            //If soldier
            if (param == Strings::Water)
            {
                String waterString = Strings::WaterLocation + content;
                    VectorData* soldierData = objects.get(Strings::Soldier);
                    if(soldierData)
                    {
                        //Soldier hits water/jumps while in water
                        Event* event;
                        
                        ERRCHECK(eventsystem->getEvent(waterString.toUTF8(),
                                                       FMOD_EVENT_DEFAULT, 
                                                       &event));                        
                        soldierData->addEvent(event);
                        ERRCHECK(event->start());
                    }
            
            }
            else if (param == Strings::Gun)
            {
                String gunString = Strings::GunsLocation;
                
                if (Globals::grenadeLauncher)
                    gunString = gunString + "grenade";
                else
                    gunString = gunString + "gun";
                
                gunString = gunString + content;
                
                VectorData* gunData = objects.get(Strings::Soldier);
                
                if(gunData)
                {
                    //Gun Shot
                    Event* event;
                    
                    
                    
                    ERRCHECK(eventsystem->getEvent(gunString.toUTF8(),
                                                   FMOD_EVENT_DEFAULT, 
                                                   &event));
                                      
                    gunData->addEvent(event);
                    ERRCHECK(event->start());
                    
                    if (!Globals::grenadeLauncher)
                    {
                        //Checks to make sure gun is in use, grenades have their own bird flying event
                        //If a certain amount of time has past since the last gun shot triggers the sound of birds flying away
                        if (Globals::birdCounter > birdCounterTrigger)
                        {
                            gunData->addEvent(birdsFlying);
                            ERRCHECK(birdsFlying->start());
                            
                            DBG("Bird sounds triggered");
                        }
                        //Sets the bird counter to 0 every time the gun is fired. Makes sure the birds only return when the gun hasn't been fired for a while
                        Globals::birdCounter = 0;
                    }
                }
                
                
                else if (content == Strings::GunEmpty)
                {
                    //Ammo is unlimited, should never be called, included for future
                }
            }
        }
	}
	
	/** Boolean (on/off) messages from the game.
	 
	 @param name		One of the following:
						- @c soldier
						<br><br>
	 @param gameObjectInstanceID	Not really needed here.
						<br><br>
	 @param param		One of the following:
						- @c water: when @e name is "soldier"
						<br><br>
	 @param flag		One of the following:
						- when @e param is @c "water" @c true means the soldier is moving in the water
						- when @e param is @c "water" @c false means the soldier stopped moving in the water
	 
	 */
	void handleBool(String const& name, int gameObjectInstanceID, String const& param, bool flag)
	{
        if (name == Strings::Soldier)
        {
            //If soldier
            if (param == Strings::Water)
            {
                //Soldier in water
                Globals::inWater = flag;
            }
        }
	}
	
	/** Integer messages from the game.
	 
	 @param name		One of the following:
						- @c soldier
						<br><br>
	 @param gameObjectInstanceID	
						Not really needed here.
						<br><br>
	 @param param		One of the following:
						- @c gun: when @e name is "soldier" the value is the weapon selected 0=gun, 1=grenade
						<br><br>
	 @param value		The values described under the @e param section above.
	 */
	void handleInt(String const& name, int gameObjectInstanceID, String const& param, int value)
	{   
        if (name == Strings::Soldier)
        {
            //If soldier
            if (param == Strings::Gun)
            {
                //True if using grenadeLauncher false if using the gun
                Globals::grenadeLauncher = value;
            }
        }
	}
	
	/** "Real" messages from the game i.e., continuous controls.
	 
	 @param name		One of the following:
						- @c grenade
						<br><br>
	 @param gameObjectInstanceID	
						Not really needed here.
						<br><br>
	 @param param		One of the following:
						- @c explode: when @e name is "grenade" the power of the grenade
									  that's just about to explode (always 320 in this version)
						<br><br>
	 @param value		The values described under the @e param section above.
	 */
	void handleReal(String const& name, int gameObjectInstanceID, String const& param, double value)
	{
        if (name == Strings::Grenade)
        {
            if (param == Strings::GrenadeExplode)
            {
                VectorData* grenadeData = objects.get(Strings::Grenade);
                //Play explosion sound
                if(grenadeData)
                {                    
                    Event* event;
                    Event* ring;
                    String grenadeString = Strings::GunsLocation+"explode";
                    
                    ERRCHECK(eventsystem->getEvent(grenadeString.toUTF8(),
                                                   FMOD_EVENT_DEFAULT, 
                                                   &event));
                    grenadeData->addEvent(event);
                    ERRCHECK(event->start());
                    
                    //If a certain amount of time has past since the last gun shot triggers the sound of birds flying away
                    if (Globals::birdCounter > birdCounterTrigger)
                    {
                        //Placed in the grenadeExplode event so the sound waits till the grenade has exploded instead of when it has been fired
                        //Position the birds flying sound on the soldier so it is always distant and away from the soldier. Positioning the sound on the grenade meant that the birds could be triggered too close to the listener
                        VectorData* soldierData = objects.get(Strings::Soldier);
                        
                        soldierData->addEvent(birdsFlying);
                        ERRCHECK(birdsFlying->start());
                        
                        DBG("Bird sounds triggered");
                    }
                    //Sets the bird counter to 0 every time the gun is fired. Makes sure the birds only return when the gun hasn't been fired for a while
                    Globals::birdCounter = 0;
                    
                    //Adds a loud ringing sound depending on how close the explosion was. Being able to trigger a global heavy low pass filter would complete this effect
                    grenadeString = grenadeString+"Ring";
                    
                    ERRCHECK(eventsystem->getEvent(grenadeString.toUTF8(),
                                                   FMOD_EVENT_DEFAULT, 
                                                   &ring));
                    
                    VectorData* soldierData = objects.get(Strings::Soldier);
                    
                    if (soldierData) {
                        soldierData->addEvent(ring);
                        
                        EventParameter* param;
                        ERRCHECK(ring->getParameter(Strings::ExplodeDistance, &param));
                        float distance = grenadeData->getPos()->z - soldierData->getPos()->z;
                        DBG(distance);
                        ERRCHECK(param->setValue(distance));
                        ERRCHECK(ring->start());
                        
                    }
                  
                    
                }
                
            }
        }
	}
		
	/** Collisions or triggers from the game.
	
	 @param name		One of the following:
						 - @c soldier
						 - @c bullet
						 - @c grenade
						 - @c brick
						 - @c inkcan
						 - @c barrel
						 - @c chair
						 - @c noticeboard
						 - @c cabinet
						 - @c tyre
						<br><br>
	 @param gameObjectInstanceID	Not really needed here.
						<br><br>
	 @param collision	One of the following:
						<br>
						When name is <code>brick, inkcan, barrel, chair, noticeboard, cabinet</code> or @c tyre 
						these object may strike other objects after being disturbed by bullets or grenades.
						You could take account of the types of surface that these objects strike by using the 
						@e collision.otherName field but the object itself is more likely to be of importance
						along with an indication of the strength of the impact using the @e collision.velocity 
						field.
						<br><br>
						When name is @c "soldier" @e collision.otherName will be one of: <ul>
						<li>@c dirt</li>
						<li>@c wood</li>
						<li>@c metal</li>
						<li>@c concrete</li>
						<li>@c sand</li>
						</ul>
						These will be for footstep sounds, the @e collision.velocity will
						be how hard the foot hit the object.
						<br><br>
						When name is "bullet" @e collision.otherName will be one of: <ul>
						<li>@c dirt</li>
						<li>@c wood</li>
						<li>@c metal</li>
						<li>@c concrete</li>
						<li>@c sand</li>
						<li>@c water</li>
						<li>@c glass</li>
						</ul>
						These will be for bullet impact sounds, the @e collision.velocity will
						be dependent on the distance from the target. This will be in the range 
						0.8-1.0 where 1.0 is close range and 0.8 is 50m away. The exception to this 
						rule is for @c glass sounds. Here a @e collision.velocity of 1.0 will be sent.
						<br><br>
						The impact positions for bullet and grenades will have been reported just before 
						the collision/explosion via the "bullet" or "grenade" position (@c pos) vector - 
						see handleVector().
	 */
	void handleHit(String const& name, int gameObjectInstanceID, Collision const& collision)
	{
        if (name == Strings::Soldier) {
            //0.4 is the general walking velocity, 1 is running
            if (collision.velocity == 1)
                Globals::running = true;
            else
                Globals::running = false;
            
            VectorData* soldierData = objects.get(Strings::Soldier);
            if(soldierData)
            {
                String footstepString;
                
                if (Globals::inWater)
                    footstepString = Strings::FootstepLocation+String("water");
                else
                    footstepString = Strings::FootstepLocation+collision.otherName;
                
                Event* event;
                
                ERRCHECK(eventsystem->getEvent(footstepString.toUTF8(),
                                               FMOD_EVENT_DEFAULT, 
                                               &event));
                
                EventParameter* param = nullptr;
                //Not error checked as some footsteps don't have a velocity parameter
                event->getParameter(Strings::Velocity, &param);
                
                if (param != nullptr)
                    ERRCHECK(param->setValue(collision.velocity));
                
                soldierData->addEvent(event);
                ERRCHECK(event->start());            
            }
        }
        
        else if (name == Strings::Bullet || name == Strings::Grenade)
        {
            String bulletString = Strings::GunsLocation + Strings::Bullet + "/" + collision.otherName;
            
            VectorData* bulletData = objects.get(Strings::Bullet);
            if(bulletData)
            {                    
                Event* event;
                
                ERRCHECK(eventsystem->getEvent(bulletString.toUTF8(),
                                               FMOD_EVENT_DEFAULT, 
                                               &event));
                
                bulletData->addEvent(event);
                ERRCHECK(event->start());
            }            
        }
        
        else
        {
            DBG(name << " Collided with = " << collision.otherName << " Velocity = " << collision.velocity);
            if (collision.velocity > 0)
            {
                String uniqueString = makeUniqueString(name, gameObjectInstanceID);
                
                String collisionString = Strings::CollisionsLocation + name;
                DBG(collisionString);
                VectorData* collisionObject = objects.get(uniqueString);
                
                if (collisionObject)
                {
                    Event* event;
                    
                    ERRCHECK(eventsystem->getEvent(collisionString.toUTF8(),
                                                   FMOD_EVENT_DEFAULT, 
                                                   &event));
                    
                    EventParameter* param;
                    ERRCHECK(event->getParameter(Strings::Velocity, &param));
                    
                    ERRCHECK(param->setValue(collision.velocity));
                    
                    collisionObject->addEvent(event);
                    ERRCHECK(event->start());
                }
            }
        }
        
	}
};


#endif //_MAINCOMPONENT_H_ 