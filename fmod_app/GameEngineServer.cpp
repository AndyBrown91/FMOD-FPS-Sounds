#include "headers.h"
#include "GameEngineServer.h"


Collision::Collision(String const& name, float vel)
:	otherName(name),
	velocity(vel) 
{
}


GameEngineServer::GameEngineServer(int port)
:	ConnectionServer(port)
{
	//startThread();
}

GameEngineServer::~GameEngineServer()
{
	//stopThread(4000);
}

String GameEngineServer::getGameInstanceString(String const& name, int gameObjectInstanceID)
{
	if(!gameObjectInstanceID)
	{
		return name;
	}
	else
	{ 
		return name + String(gameObjectInstanceID);
	}
}

void GameEngineServer::handleConnectionMessage(String const& name, String const& t, String const& message)
{
	static const juce_wchar typeBool			= 'b';
	static const juce_wchar typeInt				= 'i';
	static const juce_wchar typeReal			= 'r';
	static const juce_wchar typeString			= 's';
	static const juce_wchar typeVector			= 'v';
	static const juce_wchar typeCollision		= 'c';
	
	static const String actionCreate		= "create";
	static const String actionDestroy		= "destroy";
	static const String actionHit			= "hit";
	
	juce_wchar type = t[0];
	
	StringArray messageItems;
	messageItems.addTokens(message.unquoted(), true);
	
	int messageIndex = 0;
	int gameObjectInstanceID = 0;
	
	if(CharacterFunctions::isUpperCase(type))
	{
		gameObjectInstanceID = messageItems[messageIndex++].getIntValue();
		type = CharacterFunctions::toLowerCase(type);
	}
	
	String object;
	String param;
	
	if(name.containsChar('.'))
	{
		object = name.upToLastOccurrenceOf(".", false, false);
		param = name.fromLastOccurrenceOf(".", false, false);
	}
	else
	{
		object = param = name;
	}
	
	switch (type) 
	{
		case typeBool: {
			bool flag = messageItems[messageIndex].getIntValue() != 0;
			
			handleBool(object, gameObjectInstanceID, param, flag);
			return;
		} break;
			
		case typeInt: {
			int data = messageItems[messageIndex].getIntValue();
			
			if(param == actionCreate)
			{
				gameObjectInstanceID = data;
				handleCreate(object, gameObjectInstanceID);
				return;
			}
			else if(param == actionDestroy)
			{
				gameObjectInstanceID = data;
				handleDestroy(object, gameObjectInstanceID);
				return;
			}
			else
			{
				handleInt(object, gameObjectInstanceID, param, data);
				return;
			}
			
		} break;
			
		case typeReal: {
			double data = messageItems[messageIndex].getDoubleValue(); 
			handleReal(object, gameObjectInstanceID, param, data);
			return;
		} break;
			
		case typeString: {
			const String& data = messageItems[messageIndex];
			handleString(object, gameObjectInstanceID, param, data);
			return;
		} break;
			
		case typeVector: {
			Vector3 vector;
			float* floatVector = (float*)&vector;
			
			// This seemingly pointless complexity is to allow it to work with an
			// OpenGL Vector3 which contains an array rather than x, y, z members.
			floatVector[0] = messageItems[messageIndex++].getFloatValue();
			floatVector[1] = messageItems[messageIndex++].getFloatValue();
			floatVector[2] = messageItems[messageIndex++].getFloatValue();
			
			handleVector(object, gameObjectInstanceID, param, &vector);
			return;
		} break;
			
		case typeCollision: {
			String otherName = messageItems[messageIndex++];
			float velocity = messageItems[messageIndex++].getFloatValue();
			handleHit(object, gameObjectInstanceID, Collision(otherName, velocity));
			return;
		}
	}
	
	handleOther(name, t, message);
}

void GameEngineServer::handleCreate(String const& name, int gameObjectInstanceID)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleCreate: %s %d",
		    (const char*)name.toUTF8(), 
             gameObjectInstanceID);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleDestroy(String const& name, int gameObjectInstanceID)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleDestroy: %s %d",
		   (const char*)name.toUTF8(),
		   gameObjectInstanceID);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleVector(String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleVector: %s %d %s %f %f %f",
		   (const char*)name.toUTF8(), 
		   gameObjectInstanceID, 
		   (const char*)param.toUTF8(),
		   vector->x, vector->y, vector->z);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleHit(String const& name, int gameObjectInstanceID, Collision const& collision)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleHit: %s %d %s %f",
		   (const char*)name.toUTF8(), 
		   gameObjectInstanceID,
		   (const char*)collision.otherName.toUTF8(),
		   collision.velocity);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleBool(String const& name, int gameObjectInstanceID, String const& param, bool flag)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleBool: %s %d %s %d",
		   (const char*)name.toUTF8(),
		   gameObjectInstanceID,
		   (const char*)param.toUTF8(),
		   flag);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleInt(String const& name, int gameObjectInstanceID, String const& param, int value)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleInt: %s %d %s %d",
		   (const char*)name.toUTF8(),
		   gameObjectInstanceID,
		   (const char*)param.toUTF8(),
		   value);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleReal(String const& name, int gameObjectInstanceID, String const& param, double value)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleReal: %s %d %s %f",
		   (const char*)name.toUTF8(), 
		   gameObjectInstanceID,
		   (const char*)param.toUTF8(),
		   value);
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleString(String const& name, int gameObjectInstanceID, String const& param, String const& content)
{
    char buf[1024];
	snprintf(buf, 1024, "Unhandled GameEngineServer::handleString: %s %d %s \"%s\"",
		   (const char*)name.toUTF8(), 
		   gameObjectInstanceID,
		   (const char*)param.toUTF8(),
		   (const char*)content.toUTF8());
    Logger::outputDebugString(buf);
}

void GameEngineServer::handleOther(String const& name, String const& t, String const& value) 
{
	// empty
}