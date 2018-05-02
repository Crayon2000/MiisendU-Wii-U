#include "TCPServer.hpp"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <dynamic_libs/nn_act_functions.h>

#include "logger.h"
#include "net.h"

#define wiiu_errno (*__gh_errno_ptr())

TCPServer::TCPServer(s32 port,s32 priority) {
    this->port = port;
    this->sockfd = -1;
    this->clientfd = -1;
    memset(&(this->sock_addr),0,sizeof(this->sock_addr));

    pThread = CThread::create(TCPServer::DoTCPThread, (void*)this, CThread::eAttributeAffCore2,priority);
    pThread->resumeThread();
}

TCPServer::~TCPServer() {
    CloseSockets();
    //DEBUG_FUNCTION_LINE("Thread will be closed\n");
    exitThread = 1;

    ICInvalidateRange((void*)&exitThread, 4);
    DCFlushRange((void*)&exitThread, 4);

    if(pThread != NULL) {
        //DEBUG_FUNCTION_LINE("Deleting it!\n");
        delete pThread;
    }
    //DEBUG_FUNCTION_LINE("Thread done\n");
    pThread = NULL;
}

void TCPServer::CloseSockets() {
    if (this->sockfd != -1) {
        socketclose(this->sockfd);
    }
    if (this->clientfd != -1) {
        socketclose(this->clientfd);
    }
    this->sockfd = -1;
    this->clientfd = -1;
}

void TCPServer::ErrorHandling() {
    CloseSockets();
    os_usleep(1000*1000*2);
}

void TCPServer::DoTCPThreadInternal() {
    s32 ret;
    s32 len;
    connected = false;
    while (1) {
        if(exitThread) {
            break;
        }
        memset(&(this->sock_addr),0,sizeof(sock_addr));
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_port = this->port;
        sock_addr.sin_addr.s_addr = 0;

        this->sockfd = ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(ret == -1) {
            ErrorHandling();
            continue;
        }
        s32 enable = 1;

        setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

        ret = bind(this->sockfd, (sockaddr *)&sock_addr, 16);
        if(ret < 0) {
            ErrorHandling();
            continue;
        }
        ret = listen(this->sockfd, 1);
        if(ret < 0) {
            ErrorHandling();
            continue;
        }

        do {
            DEBUG_FUNCTION_LINE("Waiting for a connection\n");
            if(exitThread) {
                break;
            }
            len = 16;
            clientfd = ret = accept(sockfd, (sockaddr *)&(sock_addr), &len);

            if(ret == -1) {
                ErrorHandling();
                break;
            }

            if(!acceptConnection()) {
                ErrorHandling();
                break;
            }

            connected = true;

            DEBUG_FUNCTION_LINE("Connection accepted\n");

            whileLoop();

            DEBUG_FUNCTION_LINE("Client disconnected\n");

            if(clientfd != -1) {
                socketclose(clientfd);
            }
            clientfd = -1;
        } while(0);
        DEBUG_FUNCTION_LINE("Closing TCPServer\n");
        connected = false;
        onConnectionClosed();
        CloseSockets();
        continue;
    }
    DEBUG_FUNCTION_LINE("Ending DoTCPThreadInternal\n");
}

void TCPServer::DoTCPThread(CThread *thread, void *arg) {
    TCPServer * args = (TCPServer * )arg;
    return args->DoTCPThreadInternal();
}
