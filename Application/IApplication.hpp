#pragma once

#include <cpprest/http_msg.h>

namespace rvceERMS
{
  /**
   *  This class implements the business logic of Emergency Response Management
   *  System.
   *  The implementation sits behind the HTTP server, and handles the GET
   *  POST and DELETE requests from the client.
   *
   *  @ref https://microsoft.github.io/cpprestsdk/classweb_1_1http_1_1http__request.html
   */
  class IApplication
  {
  public:
    ~IApplication() = default;

  public:
    /// @brief This method handles GET request from the peer.
    ///
    /// @param request HTTP Request encapsulating the GET operation.
    virtual void HandleGetMessage(web::http::http_request& request) = 0;

    /// @brief This method handles POST request from the peer.
    ///
    /// @param request HTTP Request encapsulating the POST operation.
    virtual void HandlePostMessage(web::http::http_request& request) = 0;

    /// @brief This method handles DELETE request from the peer.
    ///
    /// @param request HTTP Request encapsulating the DELETE operation.
    virtual void HandleDeleteMessage(web::http::http_request& request) = 0;
  };
}
