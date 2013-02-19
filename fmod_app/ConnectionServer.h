#ifndef CONNECTIONSERVER_H
#define CONNECTIONSERVER_H

#include <juce/juce.h>


/** A class which handles low level communication of the network.
 This has a very simple protocol which can be adapted flexibly. */
class ConnectionServer : public Thread
{		
public:
	/** Constructor for the ConnectionServer.
	 This also starts the network thread.
	 @param port The TCP/IP port on which to communicate */
	ConnectionServer (int port = 60000);
	virtual ~ConnectionServer ();
		
	/** The main messages from the connection.
	 This is called on the internal thread.
	 @param name		An arbitrary name and/or command for the payload.
	 @param type		A code to signify the format of the value payload parameter.
	 @param message		The value payload for the message. */
	virtual void handleConnectionMessage(String const& name, String const& type, String const& message) = 0;
	
	/** A message to indicate the connection was made. 
	 This is called on the internal thread. */
	virtual void handleConnect() = 0;
	
	/** A message to indicate the connection was terminated. 
	 This is called on the internal thread.*/
	virtual void handleDisconnect() = 0;
	
	/** A message called regularly on the network thread. */
	virtual void tick() = 0;
	
private:
	StreamingSocket listener;
	StreamingSocket *connection;
	
	void run();	
	void disconnect();
};

#endif // CONNECTIONSERVER_H
