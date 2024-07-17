#pragma once

#include <memory>

namespace rvceERMS
{
  class IApplication;
  class HttpServer
  {
  public:
    HttpServer(int listenPort = 8080);
    ~HttpServer() = default;

  public:
    int Start();
    void SetApplication(std::shared_ptr<IApplication> pApplication);

  private:
    int mListenPort;
    std::shared_ptr<IApplication> mpApplication { nullptr };
  };
}
