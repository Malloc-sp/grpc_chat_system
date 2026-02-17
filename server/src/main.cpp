#include "server.h"
#include "log.h"

int main()
{
    const std::string address = "0.0.0.0:6666";
    ServerImpl server;
    server.Run(address);
    return 0;
}
