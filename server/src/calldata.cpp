#include "calldata.h"

CallDataSignUp::CallDataSignUp(GrpcData::AsyncService* service, ServerCompletionQueue *cq)
    :service_(service), cq_(cq)
{

}

void CallDataSignUp::Proceed(bool ok)
{

}

CallDataChat::CallDataChat(GrpcData::AsyncService* service, ServerCompletionQueue *cq,
            ServerImpl* server)
    :service_(service), cq_(cq), responder_(&ctx_), m_owner(server)
{
    m_status = CallDataStatus::CREATE;
    Proceed(true);
}

void CallDataChat::Proceed(bool ok)
{
    switch(m_status)
    {
        case CallDataStatus::CREATE:
        {
            m_status = CallDataStatus::PROCESS;
            service_->RequestsendMessage(&ctx_, &responder_, cq_, cq_, this);
            break;
        }
        case CallDataStatus::PROCESS:
        {
            new CallDataChat(service_, cq_, m_owner);
            m_owner->callDataChat = this;
            m_status = CallDataStatus::READ;
            responder_.Read(&recvMsg,this);
            break;
        }
        case CallDataStatus::READ:
        {
            if(!ok)
            {
                m_status = CallDataStatus::FINISH;
                responder_.Finish(grpc::Status::OK, this);
            }
            if(recvMsg.to() != "")
            {
                //std::lock_guard<std::mutex> lck();
                SendMessageResponse resp;
                resp.set_from(recvMsg.from());
                resp.set_content(recvMsg.content());
                std::lock_guard<std::mutex> lck(m_owner->m_writeMutex);
                auto op = new WriteOperation(this);
                m_owner->callDataChat->responder_.Write(resp,op);
            }
            responder_.Read(&recvMsg,this);
            break;
        }
        case CallDataStatus::FINISH:
        {
            delete this;
            break;
        }
        default:
        {
            break;
        }
    }
}

WriteOperation::WriteOperation(CallData *callData) : parent_(callData)
{}


void WriteOperation::Proceed(bool ok)
{
}

