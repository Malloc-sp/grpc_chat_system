#include "calldata.h"
#include "log.h"

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

            const auto& client_metadata = ctx_.client_metadata();
            std::string name = "";
            for (const auto& [key, value] : client_metadata) 
            {
                if(std::string(key.data(), key.size()) == "name")
                {
                    name = std::string(value.data(), value.size());
                }
            }
            if(name == "" || !m_owner->addUser(name ,this))
            {
                m_status = CallDataStatus::FINISH;
                responder_.Finish(grpc::Status::OK, this);
                return;
            }

            m_name = name;
            m_status = CallDataStatus::READ;
            responder_.Read(&recvMsg,this);
            break;
        }
        case CallDataStatus::READ:
        {
            if(!ok)
            {
                m_owner->deleteUser(m_name);
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
                LOG_INFO(recvMsg.to());
                if(m_owner->m_onlineStream.find(recvMsg.to()) != m_owner->m_onlineStream.end())
                {
                    //if(!m_owner->m_onlineStream[recvMsg.to()]->user_bWriting.load())
                    {
                        LOG_INFO(recvMsg.from() + " " + recvMsg.to());
                        auto *cd = static_cast<CallDataChat*>(m_owner->m_onlineStream[recvMsg.to()]->user_callDataPtr);
                        auto op = new WriteOperation(cd);
                        cd->responder_.Write(resp,cd);
                        return;
                    }
                }
                LOG_INFO("发送失败");
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

