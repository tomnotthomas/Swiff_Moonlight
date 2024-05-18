#include "ssmnbackendapi.h"
#include "curlrequest.h"
#include <nlohmann/json.hpp>
#include <QDebug>

using namespace ssmn;

SsmnBackendApi::SsmnBackendApi()
{
    ssmn::CurlRequest::init();
}

SsmnBackendApi::~SsmnBackendApi()
{
    ssmn::CurlRequest::deinit();
}

void SsmnBackendApi::remoteRegister() {
  arg_map args =  { {"title", mComputerName}, {"ip_address", mLocalAddress} };
  doPostRequest(kApiRegister, args);
}

void SsmnBackendApi::remoteUnregister() {
  arg_map args =  { {"title", mComputerName}, {"ip_address", mLocalAddress} };
  doPostRequest(kApiUnRegister, args);
}

void SsmnBackendApi::serverList() {
  arg_map args;
  doPostRequest(kApiServerList, args);
}

std::string SsmnBackendApi::getSessionId()
{
  arg_map args =  { {"pc_name", mComputerName} };
  std::string response = doPostRequest(kApiSessions, args);
  std::string session_id;

  if (!response.empty()) {
    try {
      nlohmann::json j =  nlohmann::json::parse(response.c_str());
      session_id = j["result"]["session_id"];
    } catch (std::exception& e) {
      qDebug() << "wrong response from server\n"
               << response.c_str() << "\n"
               << "what: " << e.what();
    }

    qDebug() << "session id " << session_id.c_str();
  } else {
      qDebug() << "Remote address is empty!";
  }

  return session_id;
}

bool SsmnBackendApi::validateSessionId(const std::string& session_id)
{
  arg_map args =  { {"session_id", session_id}, {"pc_name", mComputerName} };
  std::string response = doPostRequest(kApiValidateSessionId, args);
  std::string message;

  try {
    nlohmann::json j =  nlohmann::json::parse(response.c_str());
    message = j["result"]["message"];
  } catch (...) {
    qDebug() << "wrong response from server";
  }

  qDebug() << "message " << message.c_str();
  return message == "ok";
}

std::string SsmnBackendApi::doPostRequest(const std::string &api, arg_map &args)
{
  std::string readBuffer;

  if (mRemotePort > 0) {
    if (!mRemoteAddress.empty()) {
      std::string address = mRemoteAddress + api;
        qDebug() << "Sending request to " << address.c_str();
                                                             // fill headers
      std::vector<std::string> headersList = {
        //                "Authorization: " + sessionKey,
        "User-Agent: SSMNPCClient/V1.0",
        "Content-Type: application/x-www-form-urlencoded"
      };

      CURLcode retValue = CURLE_OK;
      std::string postData;

      for (const auto &i : args) {
        postData.append("&" + i.first);
        postData.append("=" + i.second);
      }

      qDebug() << "postData: ['" << postData.c_str() << "']";

      readBuffer = CurlRequest::PostUrl(address, postData, headersList, &retValue,
        3, 20000L);

      qDebug() << "result: " << readBuffer.c_str();
    } else {
      qDebug() << "Remote address is empty!";
    }
  } else {
    qDebug() << "Remote port is 0!";
  }

  return readBuffer;
}
