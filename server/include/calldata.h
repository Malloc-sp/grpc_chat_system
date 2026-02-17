#ifndef CALLDATA_H
#define CALLDATA_H

#include "proto.grpc.pb.h"
#include <grpc++/server_context.h>
#include "server.h"

using namespace Malloc::chatsystem::net::v1;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerAsyncReader;

enum class CallDataStatus
{
    UNKOWN,
    CREATE,
    PROCESS,
    READ,
    FINISH
};

class CallData
{
public:
    virtual ~CallData() = default;
    virtual void Proceed(bool ok) = 0;

protected:
    CallDataStatus m_status = CallDataStatus::UNKOWN;
};

class CallDataSignUp : public CallData
{
public:
    CallDataSignUp(GrpcData::AsyncService* service, ServerCompletionQueue* cq);
    virtual void Proceed(bool ok) final;

private:
    GrpcData::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;

    //ServerAsyncReader<>
};

class CallDataChat : public CallData
{
public:
    CallDataChat(GrpcData::AsyncService* service_, ServerCompletionQueue* cq, ServerImpl* server);
    virtual void Proceed(bool ok) final;

private:
    GrpcData::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;

    ServerImpl* m_owner;
    grpc::ServerAsyncReaderWriter<SendMessageResponse, SendMessageRequest> responder_;
    SendMessageRequest recvMsg;
};

class WriteOperation : public CallData
{
public:
    WriteOperation(CallData* callData);
    void Proceed(bool ok) final;
private:
    CallData* parent_;
};

#endif // CALLDATA_H
