#include "server.h"
#include <grpc++/server_builder.h>
#include "log.h"
#include "calldata.h"

ServerImpl::ServerImpl()
{}

ServerImpl::~ServerImpl()
{
    m_server->Shutdown();
    m_cq->Shutdown();
}

void ServerImpl::Run(const std::string& address)
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&m_service);
    m_cq = builder.AddCompletionQueue();
    m_server = builder.BuildAndStart();
    LOG_INFO(std::string("服务器监听:") + address);
    // 启动初始的 CallData 实例，等待第一个流
    new CallDataChat(&m_service, m_cq.get(), this);

    // 事件循环
    void* tag;
    bool ok;
    while (true) {
        // 等待下一个完成事件
        if (!m_cq->Next(&tag, &ok)) {
            // CompletionQueue 关闭
            break;
        }
        // 将 tag 转换回 CallData 对象，并调用 Proceed()
        auto callData = static_cast<CallData*>(tag);
        if(callData)
            static_cast<CallData*>(tag)->Proceed(ok);
    }
}
