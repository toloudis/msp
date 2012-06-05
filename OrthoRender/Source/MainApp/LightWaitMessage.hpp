#ifdef LIGHTWAIT_MESSAGE_HPP
#error LightWaitMessage.hpp multiply included
#endif
#define LIGHTWAIT_MESSAGE_HPP


//#define ACTIVEMQ_VER 21				// this is the ActiveMQ version we're using
#define ACTIVEMQ_VER 22				// this is the ActiveMQ version we're using


#include "MainApp/LightWaitResponse.hpp"


#if ACTIVEMQ_VER == 21
#include <activemq/concurrent/Thread.h>
#include <activemq/concurrent/Runnable.h>
#include <activemq/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Integer.h>
#include <activemq/util/Config.h>
#include <activemq/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <iostream>
#include <queue>


//========================================================================
//========================================================================
using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::concurrent;
using namespace cms;
using namespace std;
#endif

#if ACTIVEMQ_VER == 22
#include <windows.h>

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <iostream>
#include <queue>


//========================================================================
//========================================================================
using namespace activemq::core;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;
#endif


//========================================================================
//========================================================================
class LightWaitMessage 
{
public:
	unsigned char keyPressed;
	unsigned long timePressed;
	string text;
};

extern bool g_bRemoteRenderingCommand;
extern bool g_bRemoteCompressResponses;
//========================================================================
//========================================================================

class LightWaitMessageProducer : public Runnable {
private:

    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    int numMessages;
    bool useTopic;
    std::string brokerURI;
	bool pithed ;

public:

    LightWaitMessageProducer( const std::string& brokerURI,
                        int numMessages, bool useTopic = true ){
        connection = NULL;
        session = NULL;
        destination = NULL;
        producer = NULL;
        this->numMessages = numMessages;
        this->useTopic = useTopic;
        this->brokerURI = brokerURI;
		pithed = false ;
    }

    virtual ~LightWaitMessageProducer() ;

	virtual int SendMessage(unsigned char *message, long int textLength) ;

	virtual int SendMessage(const string &message) ;

	virtual void SendResponse(LightWaitResponse *response) ;

    virtual void run() ;

	virtual void stop() ;

private:
    void cleanup();
};



// 4-1-08 PSM
// This is a thread which runs in the background to consume remote messages.
//
class LightWaitMessageConsumer : public ExceptionListener,
									   public MessageListener,
									   public Runnable 
{
private:

    CountDownLatch latch;
    CountDownLatch doneLatch;
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    long waitMillis;
    bool useTopic;
    std::string brokerURI;

	queue<LightWaitMessage *> *remoteMessageQ;

	bool pithed ;


public:

    LightWaitMessageConsumer( const std::string& brokerURI,
                        long numMessages,
                        bool useTopic = false,
                        long waitMillis = 30000 ) ;
	
	virtual ~LightWaitMessageConsumer() ;

    void waitUntilReady() ;

    virtual void run() ;

	virtual void stop() ;

    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage( const Message* message ) ;
	
    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException( const CMSException& ex AMQCPP_UNUSED) ;

	queue<LightWaitMessage *> *getQueue(void) ;

private:
    void cleanup() ;
};
