#include "ssmnbackendapi.h"
#include "curlrequest.h"
#include <nlohmann/json.hpp>

#ifdef __MOONLIGHT__ /* for moonlight */
#include <QDebug>
#define LOG_DEBUG qDebug()
#define LOG_ERROR qWarning()
#else /* for sunshine */
#include "logging.h"
#define LOG_DEBUG BOOST_LOG(debug)
#define LOG_ERROR BOOST_LOG(error)
#endif

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

std::list<std::string> SsmnBackendApi::getServerList() {
    arg_map args;
    std::string response = doPostRequest(kApiServerList, args);
    std::list<std::string> result;

    if (!response.empty()) {
        try {
            nlohmann::json j =  nlohmann::json::parse(response.c_str());
            auto result_array { j.at("result").get<nlohmann::json::array_t>() };

            for (const auto& arr : result_array) {
                for (const auto& item : arr) {
                    std::string str;
                    item.at("ip_address").get_to(str);
                    result.emplace_back(std::move(str));
                }
            }
        } catch (std::exception& e) {
            LOG_ERROR << "wrong response from server\n"
                      << response.c_str() << "\n"
                      << "what: " << e.what();
        }

    } else {
        LOG_DEBUG << "Remote address is empty!";
    }

    return result;
}

bool SsmnBackendApi::setSessionPin(const std::string& server_name, const std::string& pin)
{
    arg_map args =  { {"pc_name", server_name }, {"pin", pin } };
    std::string response = doPostRequest(kApiSetPin, args);
    std::string message;

    try {
        nlohmann::json j =  nlohmann::json::parse(response.c_str());
        message = j["result"]["message"];
    } catch (...) {
        LOG_ERROR << "wrong response from server";
    }

    LOG_DEBUG << "message " << message.c_str();
    return message == "ok";
}

std::string SsmnBackendApi::getSessionPin(const std::string& server_name)
{
    arg_map args =  { {"pc_name", server_name.empty() ? mComputerName : server_name } };
    std::string response = doPostRequest(kApiGetPin, args);
    std::string pin;

    if (!response.empty()) {
        try {
            nlohmann::json j =  nlohmann::json::parse(response.c_str());
            pin = j["result"]["pin"];
        } catch (...) {
            LOG_ERROR << "wrong response from server";
        }
    }
    LOG_DEBUG << "pin " << pin.c_str();
    return pin;
}

std::string SsmnBackendApi::doPostRequest(const std::string &api, arg_map &args)
{
    std::string readBuffer;

    if (mRemotePort > 0) {
        if (!mRemoteAddress.empty()) {
            std::string address = mRemoteAddress + api;
            LOG_DEBUG << "Sending request to " << address.c_str();
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

            LOG_DEBUG << "postData: ['" << postData.c_str() << "']";

            readBuffer = CurlRequest::PostUrl(address, postData, headersList, &retValue,
                                              3, 20000L);

            LOG_DEBUG << "result: " << readBuffer.c_str();
        } else {
            LOG_DEBUG << "Remote address is empty!";
        }
    } else {
        LOG_DEBUG << "Remote port is 0!";
    }

    return readBuffer;
}
