#include "Application.hpp"

#include <nlohmann/json.hpp>

using namespace rvceERMS;
using namespace web;
namespace {  // unnamed namespace

  const std::vector<std::pair<std::string, std::string>> PERSONNELS {
    { "WAP532", "Michael Reilly" },
    { "WAP3290", "Lawson Blake" },
    { "WAP3029", "Michael Sandears" }
   };
}

int Application::ID = 0;

Application::
Application()
{
  // Initialize all the personnels
  for (const auto& elem: PERSONNELS)
  {
    Personnel p;

    p.id = elem.first;
    p.name = elem.second;

    mPersonnelMap.insert(std::make_pair(elem.first, p));
  }
}

void
Application::
HandleGetMessage(web::http::http_request& request) 
{
  std::cout << "Message Received:" << request.extract_string().get() << std::endl;

  std::cout << "URI:" << request.absolute_uri().to_string() << std::endl;

  const web::uri& rxd_uri = request.absolute_uri();
  const std::vector<utility::string_t>& path_params = web::uri::split_path(rxd_uri.path());

  if (path_params.at(1) == "personnels")
  {
    // /emergency-request/personnels
    std::string personnels = handleGetPersonnels();
    if (personnels.empty())
    {
      request.reply(http::status_codes::BadRequest, U("Bad Request"));
    }
    else
    {
      // Response to the request
      std::cout << "Response:" << personnels << std::endl;
      request.reply(http::status_codes::OK, std::move(personnels), "application/json; charset=utf-8");
    }
  }
  else if (path_params.at(1) == "emergencies") 
  {
    std::string emergency_list = handleGetEmergencies(path_params.at(2));
    if (emergency_list.empty()) 
    {
      request.reply(http::status_codes::BadRequest, U("Bad Request"));
    }
    else
    {
      // Response to the request
      std::cout << "Response:" << emergency_list << std::endl;
      request.reply(http::status_codes::OK, std::move(emergency_list), "application/json; charset=utf-8");
    }
  }
}

void
Application::
HandlePostMessage(web::http::http_request& request)
{
  const std::string& body = request.extract_string().get();

  std::cout << "URI:" << request.absolute_uri().to_string() << std::endl;

  const web::uri& rxd_uri = request.absolute_uri();
  const std::vector<utility::string_t>& path_params = web::uri::split_path(rxd_uri.path());

  std::string emergency_request;
  if (path_params.at(1) == "raise")
  {
    // /emergency-request/raise
    emergency_request = handleRaiseRequest(body);
    if (emergency_request.empty())
    {
      request.reply(http::status_codes::BadRequest, U("Bad Request"));
    }
    else
    {
      // Response to the request
      std::cout << "Response:" << emergency_request << std::endl;
      request.reply(http::status_codes::OK, std::move(emergency_request), "application/json; charset=utf-8");
    }
  }
  else if (path_params.at(1) == "assign")
  {
    // /emergency-request/{id}/assign/
    bool status = handleAssignRequest(body);

    // Response to the request
    request.reply(status ? http::status_codes::OK : http::status_codes::BadRequest);
  }
  else if (path_params.at(1) == "resolve")
  {
    // /emergency-request/{id}/resolve/
    bool status = handleResolveRequest(body);

    // Response to the request
    request.reply(status ? http::status_codes::OK : http::status_codes::BadRequest);
  }
  else
  {
    std::cout << "Invalid URI received" << std::endl;
    request.reply(http::status_codes::BadRequest, U("Bad Request"));
    return;
  }
}


std::string
Application::
handleRaiseRequest(const std::string& body)
{
  std::cout << "Message Received(Raise):" << body << std::endl;

  // Parse the rx'd message body
  nlohmann::json j =  nlohmann::json::parse(body);

  auto it_desc = j.find("description");
  auto it_type = j.find("type");
  auto it_created_time = j.find("created-time");

  if (it_desc == j.end() || it_type == j.end() || it_created_time == j.end())
  {
    std::cout << "Bad Request(raise)" << std::endl;
    return std::string();
  }

  // All's well, if we have reached here.

  EmergencyRequest emergency_request;
  emergency_request.id = ++ID;
  emergency_request.description = *it_desc;
  emergency_request.type = *it_type;
  emergency_request.status = "OPEN";
  emergency_request.created_time = *it_created_time;

  auto p = mEmergencyMap.insert(std::make_pair(emergency_request.id, emergency_request));
  if (!p.second)
  {
    std::cout << "Insert failed. Duplicate ID found:"
              << emergency_request.id << std::endl;
    return std::string();
  }

  // Convert the emergency_request to JSON.
  nlohmann::json j_emergency;
  j_emergency["id"] = emergency_request.id;
  j_emergency["description"] = emergency_request.description;
  j_emergency["type"] = emergency_request.type;
  j_emergency["status"] = emergency_request.status;
  j_emergency["created-time"] = emergency_request.created_time;


  return j_emergency.dump();
}

bool
Application::
handleAssignRequest(const std::string& body)
{
  std::cout << "Message Received(Assign):" << body << std::endl;

  // Parse the rx'd message body
  nlohmann::json j =  nlohmann::json::parse(body);

  auto it_id = j.find("id");
  auto it_personnel_assigned = j.find("personnel-assigned");

  if (it_personnel_assigned == j.end() || it_id == j.end())
  {
    std::cout << "Bad Request(assign)" << std::endl;
    return false;
  }

  int id = *it_id;
  std::string personnel_assigned = *it_personnel_assigned;

  auto it = mEmergencyMap.find(id);
  if (it == mEmergencyMap.end())
  {
    std::cout << "Failed to find the Emergency ID:" << id << std::endl;
    return false;
  }

  auto it2 = mPersonnelMap.find(personnel_assigned);
  if (it2 == mPersonnelMap.end())
  {
    std::cout << "Failed to find the Personnel:" << personnel_assigned << std::endl;
    return false;
  }

  // All's well, if we have reached here.

  // Check for re-assignment
  if (!(it->second.personnel_assigned.empty()))
  {
    auto it3 = mPersonnelMap.find(it->second.personnel_assigned);
    --(it3->second.numActiveRequests);
  }

  // Assign the personnel to the Emergency Request
  it->second.personnel_assigned = *it_personnel_assigned;
  it->second.status = "ASSIGNED";

  // Update the personnel. New assignment.
  ++(it2->second.numActiveRequests);
  
  std::cout << "Assigned Emergency-ID:" << it->second.id << "to " << it->second.personnel_assigned << std::endl;

  return true;
}

bool
Application::
handleResolveRequest(const std::string& body)
{
  std::cout << "Message Received(Resolve):" << body << std::endl;

  // Parse the rx'd message body
  nlohmann::json j =  nlohmann::json::parse(body);

  auto it_id = j.find("id");
  if (it_id == j.end())
  {
    std::cout << "Bad Request(assign)" << std::endl;
    return false;
  }

  int id = *it_id;
  auto it = mEmergencyMap.find(id);
  if (it == mEmergencyMap.end())
  {
    std::cout << "Failed to find the Emergency ID:" << id << std::endl;
    return false;
  }

  // All's well, if we have reached here.

  // Mark the Emergency Request as RESOLVED
  it->second.status = "RESOLVED";

  if (!(it->second.personnel_assigned.empty()))
  {
    auto it2 = mPersonnelMap.find(it->second.personnel_assigned);
    // Update the personnel
    --(it2->second.numActiveRequests);
  }
  
  std::cout << "Marked the Emergency-ID:" << it->second.id << " as RESOLVED." << std::endl;

  return true;
}

std::string
Application::
handleGetPersonnels() const
{
  nlohmann::json jsonArray;

  for (const auto& elem : mPersonnelMap) {
    const Personnel& p = elem.second;
    nlohmann::json personJson = {
        {"id", p.id},
        {"name", p.name},
        {"numActiveRequests", p.numActiveRequests},
        {"status", p.status}
    };
    jsonArray.push_back(personJson);
  }

  std::cout << jsonArray.dump() << std::endl;

  return jsonArray.dump();
}

std::string
Application::
handleGetEmergencies(const std::string& status) const
{
  if(status != "all") {
    return std::string();
  }
  nlohmann::json jsonArray;
  for (const auto& elem : mEmergencyMap) {
    const EmergencyRequest& e = elem.second;
    nlohmann::json emergency_json = {
        { "id", e.id },
        { "desc", e.description },
        { "type", e.type },
        { "status", e.status },
        { "created-time", e.created_time },
        { "resolved-time", e.resolved_time },
        { "personnel-assigned", e.personnel_assigned }
    };
  
    jsonArray.push_back(emergency_json);
  }

  std::cout << jsonArray.dump() << std::endl;

  return jsonArray.dump();
}

void
Application::
HandleDeleteMessage(web::http::http_request& request)
{
  std::cout << "Message Received:" << request.extract_string().get() << std::endl;
  std::cout << "URI:" << request.absolute_uri().to_string() << std::endl;
  std::cout << "URI:" << request.relative_uri().to_string() << std::endl;

  const web::uri& rxd_uri = request.absolute_uri();
  const std::vector<utility::string_t>& path_params = web::uri::split_path(rxd_uri.path());

  if (path_params.size() == 2)
  {
    handleDeleteRequest(std::stoi(path_params.at(1)));
    request.reply(http::status_codes::OK, U("Resource deleted"));
  }
  else
  {
    request.reply(http::status_codes::BadRequest, U("Bad Request"));
  }
}

void Application::handleDeleteRequest(int emergencyId)
{
  auto it = mEmergencyMap.find(emergencyId);
  std::string personnel_assigned;
  if (it != mEmergencyMap.end())
  {
    personnel_assigned = it->second.personnel_assigned;
    mEmergencyMap.erase(it);
  }

  auto it2 = mPersonnelMap.find(personnel_assigned);
  if (it2 != mPersonnelMap.end())
  {
    --(it2->second.numActiveRequests);
  }
}
