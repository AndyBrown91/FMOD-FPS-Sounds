#include "headers.h"
#include "ConnectionServer.h"

ConnectionServer::ConnectionServer (int port)
:	Thread("ConnectionServer"),
	connection(0)
{
	listener.createListener(port);
	startThread();
}

ConnectionServer::~ConnectionServer ()
{
	stopThread(4000);
	listener.close();
}

void ConnectionServer::run()
{
	const int bufferSize = 32768;
	char buffer[bufferSize];
	
	int lastTime = 0;
	int tickRate = 15;
	
	while(threadShouldExit() == false)
	{
		int time = Time::getMillisecondCounter();
		
		if(connection)
		{
			if(connection->isConnected())
			{
				int ready = connection->waitUntilReady (true, tickRate);
				
				if(ready < 0)
				{
					disconnect();
				}
				else if(ready > 0)
				{					
					int numBytes = connection->read(buffer, bufferSize, false);
					
					if(numBytes > 0)
					{										
						StringArray array;
						String msg(buffer, numBytes);
						array.addLines(msg);
						
						int num = array.size();					
						for(int i = 0; i < num; i++)
						{
							const String& line = array[i];
							StringArray data;
							data.addTokens(line, true);
							
							handleConnectionMessage(data[0], data[1], data[2]);
						}
					}
					else if(numBytes < 0)
					{
						disconnect();
					}
				}
				else
				{					
					Thread::sleep(tickRate);
				}
			}
			else
			{
				disconnect();
			}
		}
		else
		{
			if (listener.waitUntilReady(true, 200))
			{
				connection = listener.waitForNextConnection();
				
				if(connection)
				{
                    char buf[1024];
					snprintf(buf, 1024, "Connected to %s:%d",
						   (const char*)connection->getHostName().toUTF8(), 
						   connection->getPort());
                    Logger::outputDebugString(buf);
					handleConnect();
				}
			}
		}
		
		if(threadShouldExit())
			break;
		
		if((time-lastTime) >= tickRate)
		{
			tick();
			lastTime = time;	
		}	
	}
	
	deleteAndZero(connection);	// can't call virtual handleDisconnect() perhaps this thread 'joins' another on exit..?
}

void ConnectionServer::disconnect()
{
    char buf[1024];
	snprintf(buf, 1024, "Disconnected from %s:%d",
		   (const char*)connection->getHostName().toUTF8(), 
		   connection->getPort());
    Logger::outputDebugString(buf);
	
	deleteAndZero(connection);
	
	handleDisconnect();
}

