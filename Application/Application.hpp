#pragma once

#include "IApplication.hpp"

#include <map>

namespace rvceERMS
{
  class Application : public IApplication
  {
  public:
    static int ID;

  public:
    Application();
    ~Application() = default;

  public:
    void HandleGetMessage(web::http::http_request& request) override;
    void HandlePostMessage(web::http::http_request& request)  override;
    void HandleDeleteMessage(web::http::http_request& request)  override;

  private:
    class EmergencyRequest
    {
    public:
      int id;
      std::string description;
      std::string type;
      std::string status;
      std::string created_time;
      std::string resolved_time;
      std::string personnel_assigned;
    };

    class Personnel
    {
    public:
      std::string id;
      std::string name;
      int numActiveRequests { 0 };
      std::string status { "Standby" };
    };

    std::string handleRaiseRequest(const std::string& body);
    bool handleAssignRequest(const std::string& body);
    bool handleResolveRequest(const std::string& body);
    std::string handleGetPersonnels() const;
    std::string handleGetEmergencies(const std::string& status) const;
    void handleDeleteRequest(int emergencyId);


  private:
    // Key: emergency-id, Value: EmergencyRequest object
    std::map<int, EmergencyRequest> mEmergencyMap;

    // Key: id-of-the-personnel, Value: Personnel object
    std::map<std::string, Personnel> mPersonnelMap;
  };
}
