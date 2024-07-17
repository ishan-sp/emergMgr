#pragma once

#include <string>

namespace rvceERMS
{
  class IHttpClient
  {
  public:
    virtual ~IHttpClient() = default;

  public:
    ///
    /// @brief This method sends the message to the peer.
    ///
    /// @param peerEndpoint FQDN/IPv4 address of the peer.
    /// @param peerPort Port of the peer
    /// @param method HTTP method supported.
    ///               This HTTP-Client supports GET, POST and DELETE methods.
    /// @param uri URI of the HTTP request
    /// @param data Data to send
    ///
    virtual void SendHttpMsg(
      const std::string& peerEndpoint,
      int peerPort,
      const std::string& method,
      const std::string& uri,
      const std::string& data) = 0;
  };
}
