#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <dynamic_libs/socket_functions.h>
#include <dynamic_libs/os_functions.h>
#include <system/CThread.h>

class TCPServer {
public:
    TCPServer(s32 port, s32 priority);
    virtual ~TCPServer();

    bool isConnected() {
        return connected;
    }
protected:
    bool shouldExit() {
        return (exitThread == 1);
    }

    s32 getClientFD() {
        return clientfd;
    }

    s32 getSocketFD() {
        return sockfd;
    }

    struct sockaddr_in getSockAddr() {
        return sock_addr;
    }
private:
    virtual void CloseSockets();
    virtual void ErrorHandling();

    static void DoTCPThread(CThread *thread, void *arg);
    virtual void DoTCPThreadInternal();

    virtual bool acceptConnection() = 0;

    virtual void onConnectionClosed(){
        DEBUG_FUNCTION_LINE("Default onConnectionClosed \n");
    }

    /**
        Called when a connection has be accepted.
    **/
    virtual bool whileLoop() = 0;

    struct sockaddr_in sock_addr;
    volatile s32 sockfd = -1;
    volatile s32 clientfd = -1;

    s32 port = 0;
    volatile bool connected = false;

    volatile s32 exitThread = 0;
    CThread *pThread = NULL;
};

#endif //_TCPSERVER_H_
