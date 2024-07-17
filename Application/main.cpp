#include "HttpServer.hpp"
#include "Application.hpp"

#include <memory>

using namespace rvceERMS;

int main(int argc, char ** argv)
{
  std::shared_ptr<IApplication> pApplication = std::make_shared<Application>();

  HttpServer http_server;
  http_server.SetApplication(pApplication);

  http_server.Start();

  return 0;
}
