#include "stdafx.h"
#include "MainApp/LightWaitMessage.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Core/Dbg/dbgLog.hpp"

#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"


namespace LightWaitMessaging {
	extern Thread *l_ConsumerThread ;
	extern Thread *l_ProducerThread ;

	extern LightWaitMessageConsumer *l_Consumer ;
	extern LightWaitMessageProducer *l_Producer ;

	extern CRITICAL_SECTION remoteMessagingCriticalSection; 
} ;

bool g_bRemoteRenderingCommand = true;		//default to remote messaging (use /local on command line for local mode)
bool g_bRemoteCompressResponses = false;	//default to uncompressed response data
//========================================================================
//========================================================================

LightWaitMessageProducer::~LightWaitMessageProducer(){
    cleanup();
}

int LightWaitMessageProducer::SendMessage(unsigned char *text, long int textLength) {
	BytesMessage* message = session->createBytesMessage(text,textLength) ; 
	producer->send( message );
	delete message;

return 1 ;
}

int LightWaitMessageProducer::SendMessage(const string &text) {
    TextMessage* message = session->createTextMessage(text) ;
	producer->send( message );
	delete message;

return 1 ;
}


void LightWaitMessageProducer::SendResponse(LightWaitResponse *response) {
	std::string xmlResponse = response->toXML() ;
	if( g_bRemoteCompressResponses )
	{
		orthoRemoteCommandMgr::CompressGZIPString( xmlResponse );
		xmlResponse.insert( 0, "GZIP" );
	}
	BytesMessage* message = session->createBytesMessage((const unsigned char *) xmlResponse.c_str(),xmlResponse.size()) ;
	producer->send( message );
	delete message;
}



void LightWaitMessageProducer::run() {
   	PrefsData& data = PrefsMgr::Data();
	
	DBG_WARNING0("Message Producer: Opening messaging subsystem.");


	try {
        // Create a ConnectionFactory
        ConnectionFactory* connectionFactory =
            ConnectionFactory::createCMSConnectionFactory( brokerURI );

		DBG_WARNING0("Message Producer: Created connection factory.");

        // Create a Connection
        connection = connectionFactory->createConnection();

		DBG_WARNING0("Message Producer: Created connection.");

		connection->start();

		DBG_WARNING0("Message Producer: Started connection.");

        // free the factory, we are done with it.
        delete connectionFactory;

		DBG_WARNING0("Message Producer: Deleted connection factory.");

        // Create a Session
		session = connection->createSession( Session::AUTO_ACKNOWLEDGE );

		DBG_WARNING0("Message Producer: Created session.");

        // Create the destination (Topic or Queue)
        if( useTopic ) {
            destination = session->createTopic(data.m_Lightwait_MessagingOutputQueue.GetValue());

			DBG_WARNING1("Message Producer: Created topic %s",data.m_Lightwait_MessagingOutputQueue.GetValue().c_str());
        } else {
            destination = session->createQueue(data.m_Lightwait_MessagingOutputQueue.GetValue());

			DBG_WARNING1("Message Producer: Created queue %s",data.m_Lightwait_MessagingOutputQueue.GetValue().c_str());
		}

        // Create a MessageProducer from the Session to the Topic or Queue
        producer = session->createProducer( destination );

		DBG_WARNING0("Message Producer: Created session producer.");

		producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

		DBG_WARNING0("Message Producer: Set production delivery mode.");

		while (!pithed) {
			Sleep (200) ;
		}

#if 0
		producer->close() ;
		
        connection->stop();
        connection->setExceptionListener(NULL);
		session->close() ;
		session = NULL ;
		connection = NULL ;
#endif

	}catch ( CMSException& e ) {
		DBG_ERROR1("Message Producer: Received exception starting up messaging %s.",e.getMessage()) ;

        e.printStackTrace();
    }
}


void LightWaitMessageProducer::stop() {
	pithed = true ;

    // cleanup();
}


void LightWaitMessageProducer::cleanup(){

    try{
		if( destination != NULL ) {
			delete destination;
			destination = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
		if( producer != NULL ) {
			producer->close() ;
			delete producer;
			producer = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
		if( session != NULL ) {
			session->close();
			delete session;
			session = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
		if( connection != NULL ) {
			connection->stop() ;
			connection->close() ;
			delete connection;
			connection = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }
}


//========================================================================
//========================================================================
// 4-1-08 PSM
// This is a thread which runs in the background to consume remote messages.
//


LightWaitMessageConsumer::LightWaitMessageConsumer( const std::string& brokerURI,
                    long numMessages,
                    bool useTopic,
                    long waitMillis )
    : latch(1), doneLatch(numMessages) 
{
    connection = NULL;
    session = NULL;
    destination = NULL;
    consumer = NULL;
    this->waitMillis = waitMillis;
    this->useTopic = useTopic;
    this->brokerURI = brokerURI;

	remoteMessageQ = new queue<LightWaitMessage *> ;

	pithed = false ;
}


LightWaitMessageConsumer::~LightWaitMessageConsumer()
{
    cleanup();
}

void LightWaitMessageConsumer::waitUntilReady() 
{
    latch.await();
}

void LightWaitMessageConsumer::stop() {
	pithed = true ;

    // cleanup();
}


void LightWaitMessageConsumer::run() 
{
   	PrefsData& data = PrefsMgr::Data();

	DBG_WARNING0("Message Consumer: Created message consumer.");

    try 
	{
		EnterCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection) ;
		
        // Create a ConnectionFactory
        ConnectionFactory* connectionFactory =
            ConnectionFactory::createCMSConnectionFactory( brokerURI );

		DBG_WARNING0("Message Consumer: Created connection factory.");

        // Create a Connection
        connection = connectionFactory->createConnection();

		DBG_WARNING0("Message Consumer: Created connection.");

        delete connectionFactory;

		DBG_WARNING0("Message Consumer: Deleted connection factory.");

        connection->start();

		DBG_WARNING0("Message Consumer: Started connection.");

        connection->setExceptionListener(this);

        // Create a Session
        session = connection->createSession( Session::AUTO_ACKNOWLEDGE );

		DBG_WARNING0("Message Consumer: Created connection session.");

        // Create the destination (Topic or Queue)
        if( useTopic ) {
            destination = session->createTopic(data.m_Lightwait_MessagingInputQueue.GetValue());

			DBG_WARNING1("Message Consumer: Created destination topic %s.",data.m_Lightwait_MessagingInputQueue.GetValue().c_str());
        } else {
            destination = session->createQueue(data.m_Lightwait_MessagingInputQueue.GetValue());

			DBG_WARNING1("Message Consumer: Created destination queue %s.",data.m_Lightwait_MessagingInputQueue.GetValue().c_str());
        }

        // Create a MessageConsumer from the Session to the Topic or Queue
        consumer = session->createConsumer( destination );

		DBG_WARNING0("Message Consumer: Created session consumer.");

        consumer->setMessageListener( this );

		DBG_WARNING0("Message Consumer: Created consumer listener.");

#if 0
		std::cout.flush();
        std::cerr.flush();
#endif

        // Indicate we are ready for messages.
        latch.countDown();

		LeaveCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection) ;

        // Wait while asynchronous messages come in.
//			doneLatch.await( );	
		while (!pithed) {
//				doneLatch.await(waitMillis);		// 

			Sleep(200) ;
		} 

		consumer->setMessageListener( NULL );
		consumer->close() ;

        connection->stop();
        connection->setExceptionListener(NULL);
		session->close() ;

    } catch (CMSException& e) {
		DBG_ERROR1("Message Consumer: Received exception starting up messaging %s.",e.getMessage()) ;

		e.printStackTrace();

		LeaveCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection) ;
    }
}

// Called from the consumer since this class is a registered MessageListener.
void LightWaitMessageConsumer::onMessage( const Message* message )
{
    static int count = 0;

    try
    {
        count++;
        string text ;
		// unsigned char *messageBody = NULL ;

		EnterCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection) ;
		
		const BytesMessage *bytesMessage = dynamic_cast< const BytesMessage* >( message );
        if( bytesMessage != NULL ) {
			int bodyLength = bytesMessage->getBodyLength() ;
			// unsigned char *bodyBytes = NULL ;

			if (bodyLength == 0) {
			    doneLatch.countDown();
				LeaveCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection) ;
				return ;
			}

			// bodyBytes = (unsigned char *) malloc(bodyLength + 1) ; // new unsigned char(bodyLength+1) ;
	
			// bytesMessage->readBytes(messageBody,bodyLength) ;
			// memcpy(bodyBytes,bytesMessage->getBodyBytes(),bodyLength) ;
			// bodyBytes[bodyLength] = '\0' ;
			text.clear() ;
			text.insert(0,(char *) bytesMessage->getBodyBytes(),bodyLength) ;
			// free(bodyBytes) ;
        } else {
			const TextMessage* textMessage =
				dynamic_cast< const TextMessage* >( message );

			if( textMessage != NULL ) {
				text = textMessage->getText();
	        } else {
		        text = "NOT A READABLE MESSAGE!";
			}

			// strcpy((char *) messageBody,text.c_str()) ;
        }

		if (text.size() == 0) {
		    doneLatch.countDown();
			return ;
		}

		// THIS IS WHERE THE AVATAR ASSEMBLY MESSAGES COME IN
		// 4-1-08 PSM
		LightWaitMessage *message = new(LightWaitMessage) ;

		message->keyPressed = text.at(0) ;		// ONLY the first character counts 4-1-08 PSM
		message->timePressed = 0L ;
		message->text.clear() ;
		message->text.append(text) ;

		remoteMessageQ->push(message);

		LeaveCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection) ;

		DBG_WARNING0("Message Consumer: Received message via messaging.");

#if 0
		printf( "Message #%d Received: %s\n", count, text.c_str() );
#endif
	} catch (CMSException& e) {
		DBG_ERROR1("Message Consumer: Received exception starting up messaging %s.",e.getMessage()) ;
		e.printStackTrace();
    }

    // No matter what, tag the count down latch until done.
    doneLatch.countDown();
}

// If something bad happens you see it here as this class is also been
// registered as an ExceptionListener with the connection.
void LightWaitMessageConsumer::onException( const CMSException& ex AMQCPP_UNUSED) 
{
    printf("Message Consumer: CMS Exception occured.  Shutting down client.\n");
}

void LightWaitMessageConsumer::cleanup()
{

    //*************************************************
    // Always close destination, consumers and producers before
    // you destroy their sessions and connection.
    //*************************************************

    try{
		if( destination != NULL ) {
			delete destination;
			destination = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
		if( consumer != NULL ) {
			consumer->close() ;
			delete consumer;
			consumer = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
		if( session != NULL ) {
			session->close();
			delete session;
			session = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
		if( connection != NULL ) {
			connection->stop() ;
			connection->close() ;
			delete connection;
			connection = NULL ;
		}
    }catch ( CMSException& e ) { e.printStackTrace(); }
}


queue<LightWaitMessage *> *LightWaitMessageConsumer::getQueue(void) {
	return remoteMessageQ ;
}
