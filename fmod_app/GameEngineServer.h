#ifndef GAMEENGINESERVER_H
#define GAMEENGINESERVER_H

#include <juce/juce.h>
#include "ConnectionServer.h"

// this is to allow Vector3 to be predefined e.g., to an FMOD_VECTOR to avoid having to have
// casts in user code and to avoid making this file dependent on <a href=http://www.fmod.org/>FMOD</a> or any other system
#if !defined(VECTOR3) || defined(DOXYGEN)
/** Simple 3D vector.
 This has the same structure as an <a href=http://www.fmod.org/>FMOD</a> vector and an OpenGL Vector3 to make 
 things easier to typedef and inetegrate as needed. 
 
 Just do:
 @code
 typedef FMOD_VECTOR Vector3;
 #define VECTOR3
 @endcode
 ..before including GameEngineServer.h and Gamecon's own Vector3 won't be defined.
 */ 
class Vector3
{
public:
	Vector3()
	:	x(vec[0]),
		y(vec[1]),
		z(vec[2])
	{
	}
	
	
	float vec[3]; ///< { x, y, z }
	float& x; ///<  x axis
	float& y; ///<  y axis
	float& z; ///<  z axis
};
#endif

/** A simple class to hold information about a collision. */
class Collision
{
public:
	/** Simple constructor to fill the data members. */
	Collision(String const& otherName, float velocity = 0.f);
	
	/** The name of the other object which was hit. */
	String otherName;
	
	/** The relative velocity of the two objects which collided. */
	float velocity;
};


/** The GameEngineServer!.
 This does most of the work. Subclass this and implement the virtual functions as required.
 Uncaught messages through not implementing a particular function will be posted to the console. */
class GameEngineServer : public ConnectionServer
{	
public:
	/** Constructor for the GameEngineServer.
	 This also starts the ConnectionServer thread.
	 @param port The TCP/IP port on which to communicate */
	GameEngineServer(int port = 60000);	
	~GameEngineServer();
	
	/** Simple utility function to concatate a string with an int. 
	 @param name The string.
	 @param gameObjectInstanceID The int. If this is zero then only the name will be returned. */
	static String getGameInstanceString(String const& name, int gameObjectInstanceID);
	
	/** A message to indicate a game object was added to the game context.
	 @param name The name of the object.
	 @param gameObjectInstanceID The unique id of the object (or 0 if no id has been provided). */
	virtual void handleCreate(String const& name, int gameObjectInstanceID);
	
	/** A message to indicate a game object was removed from the game context.
	 @param name The name of the object.
	 @param gameObjectInstanceID The unique id of the object (or 0 if no id has been provided). */
	virtual void handleDestroy(String const& name, int gameObjectInstanceID);
	
	/** A message to indicate the position, orientation or movement of a game object within the game context.
	 @param name					The name of the object.
	 @param gameObjectInstanceID	The unique id of the object (or 0 if no id has been provided). 
	 @param param					Which type of information from is contained in the vector (e.g., pos, vel, dir).
	 @param vector	A pointer to the vector. 
					If using <a href=http://www.fmod.org/>FMOD</a> you can safely typedef the Vector3 type to an FMOD_VECTOR as the structure is the same. */	
	virtual void handleVector(String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector);
	
	/** A message to indicate that a collision occurred within the game.
	 @param name		The name of the object which collided with another object.
	 @param gameObjectInstanceID The unique id of the object (or 0 if no id has been provided). 
	 @param collision	Other information about the collision inlcuding the other object and the 
						relative velocity of the collision. */
	virtual void handleHit(String const& name, int gameObjectInstanceID, Collision const& collision);
	
	/** A boolean message from the game. 
	 A common boolean message would be "grounded" to indicate if the player is on the 
	 ground or in the air.
	 @param name					The name of the object.
	 @param gameObjectInstanceID	The unique id of the object (or 0 if no id has been provided). 
	 @param param					The boolean parameter's name (e.g., "grounded").
	 @param flag					True or false */
	virtual void handleBool(String const& name, int gameObjectInstanceID, String const& param, bool flag);
	
	/** An int message from the game. 
	 @param name					The name of the object.
	 @param gameObjectInstanceID	The unique id of the object (or 0 if no id has been provided). 
	 @param param					The integer parameter's name (e.g., treasure count or number of goals scored).
	 @param value					The integer. */	
	virtual void handleInt(String const& name, int gameObjectInstanceID, String const& param, int value);
	
	/** A real (i.e., floating point) message from the game.
	 This may be a float or a double but is passes as a double here.
	 @param name					The name of the object.
	 @param gameObjectInstanceID	The unique id of the object (or 0 if no id has been provided). 
	 @param param					The real parameter's name (e.g., health or engine rpm).
	 @param value					The real value as a double. */	
	virtual void handleReal(String const& name, int gameObjectInstanceID, String const& param, double value);
	
	/** A string message from the game.
	 @param name					The name of the object.
	 @param gameObjectInstanceID	The unique id of the object (or 0 if no id has been provided). 
	 @param param					The string parameter's name.
	 @param content					The string message. */	
	virtual void handleString(String const& name, int gameObjectInstanceID, String const& param, String const& content);

	/** Other messages which haven't been parsed by the GameEngineServer class. */
	virtual void handleOther(String const& name, String const& t, String const& value);
	
private:
	void handleConnectionMessage(String const& name, String const& type, String const& message);
};

/** @mainpage
 
 @section Introduction Introduction
 Gamecon is a simple network system for communicating between a game and another application. 
 
 Gamecon was really designed to allow a game to send data over the localhost network to 
 another application running the sound engine. In particular this system was built around
 <a href=http://unity3d.com//>Unity</a> to <a href=http://www.fmod.org/>FMOD</a> communication, 
 although there's nothing specific to <a href=http://unity3d.com//>Unity</a> or 
 <a href=http://www.fmod.org/>FMOD</a> in the code. The original purpose was for
 sound design and music technology students to be able to test their sound design and music 
 within the context of a game-like environment.
 
 Gamecon includes a set of <a href=http://www.rawmaterialsoftware.com/juce.php>Juce</a>-based 
 classes so should work on Mac, Windows, Linux and iOS.
 
 @section GettingStarted Getting started
 The main class of interest is the GameEngineServer class, this is where you can receive the
 position of objects within the game world, collisions that occur and so on. GameEngineServer 
 inherits from the ConnectionServer class which deals with the lower level reception of messages
 over the network, GameEngineServer parses these and turns them into more useful messages.
 
 Typically you would implement a class which inherits from GameEngineServer, you should ensure 
 that only one of these objects will be instantiated within your app. There are three pure virtual functions you 
 must implement from ConnectionServer. The first two are ConnectionServer::handleConnect() and
 ConnectionServer::handleDisconnect() which repsond to the network connection being made and lost respectively. The handleConnect()
 function is a good place to initialise your sound engine, in particular it is an ideal place to initialise
 the <a href=http://www.fmod.org/>FMOD</a> event system since it helps ensure that calls to 
 <a href=http://www.fmod.org/>FMOD</a> are all made on the same "network" thread. Similarly
 handleDisconnect() is a good place to shutdown and clear up the sound engine. The third pure virtual
 function from ConnectionServer is ConnectionServer::tick() which is called regularly on the network thread. This
 should/could be used to send "update" (or similar) messages to the sound engine. Commonly this would be assumed to
 be every "frame" from the game. It is the ideal place to call update() messages for <a href=http://www.fmod.org/>FMOD</a>.
 
 Then you have the option to implement various virtual functions from GameEngineServer, these are:
 - GameEngineServer::handleCreate()
 - GameEngineServer::handleDestroy()
 - GameEngineServer::handleVector()
 - GameEngineServer::handleHit()
 - GameEngineServer::handleBool()
 - GameEngineServer::handleInt()
 - GameEngineServer::handleReal()
 - GameEngineServer::handleString()
 - GameEngineServer::handleOther()
 
 These can respond to various actions and changes from the game engine in terms of object creation and deletion
 (which can then be used to create and delete sound events for example), object position and movement within the game, 
 and collisions between objects in the game. Again these are all called on the network thread so are safe to use
 with <a href=http://www.fmod.org/>FMOD</a> without using critical sections etc. as prescribed in the <a href=http://www.fmod.org/>FMOD</a> documentation.
 
 @section FurtherInfo Further information
 - @subpage DataFormat "Gamecon data format and common usage"
 */

/** @page DataFormat Gamecon data format and common usage
 
 @section Introduction Introduction
 
 In this version all network traffic is ASCII encoded, including floats and doubles (so there may be
 some issues with rounding to take care of).
 
 The general format of a message is a space-separated message:
 @code <message-name> <message-type> <message-content> @endcode
 
 Where:
 - <b><em><tt><message-name></tt></em></b>	is the message name (a string); 
 - <b><em><tt><message-type></tt></em></b>   is the message type (a single character); and 
 - <b><em><tt><message-content></tt></em></b>   is the content, value or arguments of the message (as a string). 
 
 If any of these three parts contain spaces then these strings will be quoated (i.e., surrounded by double quotes).
 This applies primarily to the <b><em><tt><message-content></tt></em></b> part as outlined below. By convention 
 <b><em><tt><message-name></tt></em></b> will commonly be in two parts separated by a dot:
 @code <object-name>.<parameter-name> @endcode
 
 - <b><em><tt><object-name></tt></em></b>   is the object name to which the message relates; and 
 - <b><em><tt><parameter-name></tt></em></b>   is the parameter acting on the object. 
 
 
 Possible values of <b><em><tt><message-type></tt></em></b> are:
 - @c b : boolean (sent as an integer 0 or 1);
 - @c i : integer;
 - @c r : real (float/double);
 - @c s : string;
 - @c v : three element vector (e.g., x, y, z for 3D info); and
 - @c c : collision.
 
 If the character is upper case (i.e., @c B, @c I, @c R, @c S, @c V or @c C)
 then the value has an additional "id" integer prefixed to its normal data.
 This is to allow identification of multiple objects of the same variety
 (e.g., doors, boxes, trees) as their IDs from the game should be unique.
 
 NB the quotation marks, where applicable, in the following <em>Examples</em> are part of 
 the message and are included verbatim.
 
 @section Boolean Boolean
 
 The boolean message formats are:
 @code <message-name> b <value> @endcode and
 @code <message-name> B "<object-id> <value>" @endcode

 Where
 - <b><em><tt><message-name></tt></em></b>	is the message name (a string); 
 - @c b or @c B identifies the type of message;
 - <b><em><tt>\<value></tt></em></b> is the flag for the boolean 0 or 1; and
 - <b><em><tt><object-id></tt></em></b> is the unique object id from the game (an integer).
 

 <em>Examples</em>
 <br><hr><br>
 @code char.grounded b 1 @endcode
 <ul><li>
 This would call your GameEngineServer::handleBool() function:
 @code 
 void handleBool(String const& name,       // would be "char"
                 int gameObjectInstanceID, // would be 0
                 String const& param,      // would be "grounded"
                 bool flag)                // would be true
 {
     //...
 } @endcode
 </li></ul>
 
 <br><hr><br>
 @code light.on B "-9294 0" @endcode 
 <ul><li>
 This would call your GameEngineServer::handleBool() function:
 @code 
 void handleBool(String const& name,       // would be "light"
                 int gameObjectInstanceID, // would be -9294
                 String const& param,      // would be "on"
                 bool flag)                // would be false
 {
     //...
 } @endcode
 </li></ul>
 
 @section Integer Integer
 
 The integer message formats are:
 @code <message-name> i <value> @endcode and
 @code <message-name> I "<object-id> <value>" @endcode
 
 Where
 - <b><em><tt><message-name></tt></em></b>	is the message name (a string); 
 - @c i or @c I identifies the type of message;
 - <b><em><tt>\<value></tt></em></b> is the integer value itself; and
 - <b><em><tt><<object-id></tt></em></b> is the unique object id from the game (an integer).
  
 <em>Examples</em>
 <br><hr><br>
 @code teamA.score i 3 @endcode
 <ul><li>
 This would call your GameEngineServer::handleInt() function:
 @code 
 void handleInt(String const& name,       // would be "teamA"
                int gameObjectInstanceID, // would be 0
                String const& param,      // would be "score"
                int value)                // would be 3
 {
     //...
 } @endcode
 </li></ul>
 
 <br><hr><br>
 @code team.score I "4535 5" @endcode 
 <ul><li>
 This would call your GameEngineServer::handleInt() function:
 @code 
 void handleInt(String const& name,       // would be "team"
                int gameObjectInstanceID, // would be 4535
                String const& param,      // would be "score"
                int value)                // would be 5
 {
     //...
 } @endcode
 </li></ul>
 
 
 @section Real Real
 
 The real (float/double) message formats are:
 @code <message-name> r <value> @endcode and
 @code <message-name> R "<object-id> <value>" @endcode
 
 Where
 - <b><em><tt><message-name></tt></em></b>	is the message name (a string); 
 - @c r or @c R identifies the type of message;
 - <b><em><tt>\<value></tt></em></b> is the real (float or double) value itself; and
 - <b><em><tt><object-id></tt></em></b> is the unique object id from the game (an integer).
 
 <em>Examples</em>
 <br><hr><br>
 @code car.rpm r 0.76688 @endcode
 <ul><li>
 This would call your GameEngineServer::handleReal() function:
 @code 
 void handleReal(String const& name,       // would be "car"
                 int gameObjectInstanceID, // would be 0
                 String const& param,      // would be "rpm"
                 double value)             // would be 0.76688
 {
     //...
 } @endcode
 </li></ul>
 
 <br><hr><br>
 @code enemy.health R "-31494 85.5" @endcode 
 <ul><li>
 This would call your GameEngineServer::handleReal() function:
 @code 
 void handleReal(String const& name,       // would be "enemy"
                 int gameObjectInstanceID, // would be -31494
                 String const& param,      // would be "health"
                 double value)             // would be 85.5
 {
     //...
 } @endcode
 </li></ul>
 
 @section String String
 
 The string message formats are:
 @code <message-name> s <value> @endcode and
 @code <message-name> S "<object-id> <value>" @endcode
 
 Where
 - <b><em><tt><message-name></tt></em></b>	is the message name (a string); 
 - @c s or @c S identifies the type of message;
 - <b><em><tt>\<value></tt></em></b> is the string itself; and
 - <b><em><tt><object-id></tt></em></b> is the unique object id from the game (an integer).
 
 <em>Examples</em>
 <br><hr><br>
 @code char.surface s water @endcode
 <ul><li>
 This would call your GameEngineServer::handleString() function:
 @code 
 void handleString(String const& name,       // would be "char"
                   int gameObjectInstanceID, // would be 0
                   String const& param,      // would be "surface"
                   String const& value)      // would be "water"
 {
     //...
 } @endcode
 </li></ul>
 
 <br><hr><br>
 @code enemy.surface S "98483 gravel" @endcode 
 <ul><li>
 This would call your GameEngineServer::handleString() function:
 @code 
 void handleString(String const& name,       // would be "enemy"
                   int gameObjectInstanceID, // would be 98483
                   String const& param,      // would be "surface"
                   String const& value)      // would be "gravel"
 {
     //...
 } @endcode
 </li></ul>
 
 @section Vector Vector
 
 The vector message formats are:
 @code <message-name> v "<x-value> <y-value> <z-value>" @endcode and
 @code <message-name> V "<object-id> <x-value> <y-value> <z-value>" @endcode
 
 Where
 - <b><em><tt><message-name></tt></em></b>	is the message name (a string); 
 - @c v or @c V identifies the type of message;
 - <b><em><tt><x-value></tt></em></b>, <b><em><tt><y-value></tt></em></b> and <b><em><tt><z-value></tt></em></b> are the vector values; and
 - <b><em><tt><object-id></tt></em></b> is the unique object id from the game (an integer).
 
 <em>Examples</em>
 <br><hr><br>
 @code char.pos v "0.002 45.5235 -55.352" @endcode
 <ul><li>
 This would call your GameEngineServer::handleVector() function:
 @code 
 void handleVector(String const& name,       // would be "char"
                   int gameObjectInstanceID, // would be 0
                   String const& param,      // would be "pos"
                   const Vector3* vector)    // would be { 0.002 45.5235 -55.352 }
 {
     //...
 } @endcode
 </li></ul>
 
 <br><hr><br>
 @code char.vel V "-4882 0.003 -0.342 1.125" @endcode 
 <ul><li>
 This would call your GameEngineServer::handleVector() function:
 @code 
 void handleVector(String const& name,       // would be "char"
                   int gameObjectInstanceID, // would be -4882
                   String const& param,      // would be "vel"
                   const Vector3* vector)    // would be { 0.003 -0.342 1.125 }
 {
     //...
 } @endcode
 </li></ul>
 
 @section Collision Collision
 
 tba
  
 */

#endif // GAMEENGINESERVER_H