#pragma once

#include <string>
#include <memory>
#include <grpc++/server.h>
#include <grpc++/completion_queue.h>
#include "proto.grpc.pb.h"
#include <unordered_map>
#include <atomic>

using grpc::ServerCompletionQueue;
using namespace Malloc::chatsystem::net::v1;

class CallData;
class CallDataChat;

class ServerImpl final
{
    struct user
    {
        CallData* user_callDataPtr = nullptr;
        std::mutex user_mutex;
        std::atomic<bool> user_bWriting{false};
    };
public:
    ServerImpl();
    ~ServerImpl();
    void Run(const std::string& address);
    bool addUser(std::string name, CallData* callData);
    void deleteUser(std::string name);

private:
    std::unique_ptr<grpc::Server> m_server;
    std::unique_ptr<ServerCompletionQueue> m_cq;
    GrpcData::AsyncService m_service;

private:
    std::mutex m_writeMutex;
    std::unordered_map<std::string, user*> m_onlineStream;

    CallDataChat* callDataChat;

    friend class CallData;
    friend class CallDataChat;
};
