#include "HttpServer.hpp"

#include "IApplication.hpp"

#include <cpprest/http_listener.h>
#include <iostream>
#include <string>

using namespace rvceERMS;
using namespace web;

HttpServer::
HttpServer(int listenPort)
  : mListenPort { listenPort },
    mpApplication { nullptr }
{}

void
HttpServer::
SetApplication(std::shared_ptr<IApplication> pApplication)
{
  mpApplication = pApplication;
}

int
HttpServer::
Start()
{
  // Create a listener at http://localhost:<mListenPort>
  const std::string& http_server_endpoint =
  //  "http://127.0.0.1:" + std::to_string(mListenPort) + "/";
    "http://0.0.0.0:" + std::to_string(mListenPort) + "/";
  http::experimental::listener::http_listener listener(U(http_server_endpoint));

  // Add a handler for GET requests
  listener.support(
    http::methods::GET,
    [this](web::http::http_request request) {

      mpApplication->HandleGetMessage(request);
    });

  // Add a handler for DELETE requests
  listener.support(
    http::methods::DEL,
    [this](web::http::http_request request) {

      mpApplication->HandleDeleteMessage(request);
    });

  // Add a handler for POST requests
  listener.support(
    http::methods::POST,
    [this](web::http::http_request request) {

      mpApplication->HandlePostMessage(request);
    });

  // Open the listener and wait for a user input to close it
  std::cout << "Application listening for HTTP requests at:"
            << http_server_endpoint << std::endl;
  listener.open().wait();

  std::cout << "Press ENTER to exit." << std::endl;

  std::string line;
  std::getline(std::cin, line);
  listener.close().wait();

  std::cout << "Exiting..." << std::endl;
  return 0;
}
