///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/queue.cpp
// Purpose:     Unit test for wxMessageQueue
// Author:      Evgeniy Tarassov
// Created:     31/10/2007
// Copyright:   (c) 2007 Evgeniy Tarassov
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "doctest.h"

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/thread.h"
#endif // WX_PRECOMP

#include "wx/msgqueue.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

typedef wxMessageQueue<int> Queue;

enum WaitTestType
{
    WaitWithTimeout = 0,
    WaitInfinitlyLong
};


// This class represents a thread that waits (following WaitTestType type)
// for exactly maxMsgCount messages from its message queue and if another
// MyThread is specified, then every message received is posted
// to that next thread.
class MyThread : public wxThread
{
public:
    MyThread(WaitTestType type, MyThread* next, int maxMsgCount)
        : wxThread(wxTHREAD_JOINABLE),
        m_type(type), m_nextThread(next), m_maxMsgCount(maxMsgCount)
    {}

    // thread execution starts here
    void* Entry() override;

    // Thread message queue
    Queue& GetQueue()
    {
        return m_queue;
    }

private:
    WaitTestType m_type;
    MyThread* m_nextThread;
    int          m_maxMsgCount;
    Queue        m_queue;
};

WX_DEFINE_ARRAY_PTR(MyThread*, ArrayThread);

// this function creates the given number of threads and posts msgCount
// messages to the last created thread which, in turn, posts all the messages
// it receives to the previously created thread which does the same and so on
// in cascade -- at the end, each thread will have received all msgCount
// messages directly or indirectly
TEST_CASE("TestReceive")
{
    const int msgCount = 100;
    const int threadCount = 10;

    ArrayThread threads;

    int i;
    for ( i = 0; i < threadCount; ++i )
    {
        MyThread *previousThread = i == 0 ? nullptr : threads[i-1];
        MyThread *thread =
            new MyThread(WaitInfinitlyLong, previousThread, msgCount);

        CHECK_EQ ( thread->Create(), wxTHREAD_NO_ERROR );
        threads.Add(thread);
    }

    for ( i = 0; i < threadCount; ++i )
    {
        threads[i]->Run();
    }

    MyThread* lastThread = threads[threadCount - 1];

    for ( i = 0; i < msgCount; ++i )
    {
        lastThread->GetQueue().Post(i);
    }

    for ( i = 0; i < threadCount; ++i )
    {
        // each thread should return the number of messages received.
        // if it returns a negative, then it detected some problem.
        wxThread::ExitCode code = threads[i]->Wait();
        CHECK_EQ( code, (wxThread::ExitCode)wxMSGQUEUE_NO_ERROR );
        delete threads[i];
    }
}

// this function creates two threads, each one waiting (with a timeout) for
// exactly two messages.
// Exactly to messages are posted into first thread queue, but
// only one message is posted to the second thread queue.
// Therefore first thread should return with wxMSGQUEUE_NO_ERROR, but the second
// should return wxMSGQUEUUE_TIMEOUT.
TEST_CASE("TestReceiveTimeout")
{
    MyThread* thread1 = new MyThread(WaitWithTimeout, nullptr, 2);
    MyThread* thread2 = new MyThread(WaitWithTimeout, nullptr, 2);

    CHECK_EQ ( thread1->Create(), wxTHREAD_NO_ERROR );
    CHECK_EQ ( thread2->Create(), wxTHREAD_NO_ERROR );

    thread1->Run();
    thread2->Run();

    // Post two messages to the first thread
    CHECK_EQ( thread1->GetQueue().Post(0), wxMSGQUEUE_NO_ERROR );
    CHECK_EQ( thread1->GetQueue().Post(1), wxMSGQUEUE_NO_ERROR );

    // ...but only one message to the second
    CHECK_EQ( thread2->GetQueue().Post(0), wxMSGQUEUE_NO_ERROR );

    wxThread::ExitCode code1 = thread1->Wait();
    wxThread::ExitCode code2 = thread2->Wait();

    CHECK_EQ( code1, (wxThread::ExitCode)wxMSGQUEUE_NO_ERROR );
    CHECK_EQ( code2, (wxThread::ExitCode)wxMSGQUEUE_TIMEOUT );
    delete thread2;
    delete thread1;
}

// every thread tries to read exactly m_maxMsgCount messages from its queue
// following the waiting strategy specified in m_type. If it succeeds then it
// returns 0. Otherwise it returns the error code - one of wxMessageQueueError.
void* MyThread::Entry()
{
    int messagesReceived = 0;
    while ( messagesReceived < m_maxMsgCount )
    {
        wxMessageQueueError result;
        int msg = -1; // just to suppress "possibly uninitialized" warnings

        if ( m_type == WaitWithTimeout )
            result = m_queue.ReceiveTimeout(1000, msg);
        else
            result = m_queue.Receive(msg);

        if ( result == wxMSGQUEUE_MISC_ERROR )
            return (wxThread::ExitCode)wxMSGQUEUE_MISC_ERROR;

        if ( result == wxMSGQUEUE_NO_ERROR )
        {
            if ( m_nextThread != nullptr )
            {
                wxMessageQueueError res = m_nextThread->GetQueue().Post(msg);

                if ( res == wxMSGQUEUE_MISC_ERROR )
                    return (wxThread::ExitCode)wxMSGQUEUE_MISC_ERROR;

                // We can't use Catch asserts outside of the main thread
                // currently, unfortunately.
                // TODO: Should we really be throwing asserts here?
                //wxASSERT( res == wxMSGQUEUE_NO_ERROR );
            }
            ++messagesReceived;
            continue;
        }

        wxASSERT( result == wxMSGQUEUE_TIMEOUT );

        break;
    }

    if ( messagesReceived != m_maxMsgCount )
    {
        // TODO: Should we really be throwing asserts here?
        //wxASSERT( m_type == WaitWithTimeout );

        return (wxThread::ExitCode)wxMSGQUEUE_TIMEOUT;
    }

    return (wxThread::ExitCode)wxMSGQUEUE_NO_ERROR;
}
