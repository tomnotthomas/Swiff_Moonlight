#ifndef SSMN_BACKEND_API
#define SSMN_BACKEND_API

#include <string>
#include <cstdint>
#include <list>
#include <map>

namespace ssmn {
class SsmnBackendApi {
public:
    static constexpr const char* kApiServerList = "/api/server_list";
    static constexpr const char *kApiSetPin = "/api/pin";
    static constexpr const char *kApiRegister = "/api/register";
    static constexpr const char *kApiUnRegister = "/api/unregister";
    static constexpr const char *kApiGetPin = "/api/get_pin";

    using arg_map = std::map<std::string, std::string>;

    static SsmnBackendApi* instance() {
        static SsmnBackendApi api;
        return &api;
    }

    void setRemoteAddress(const std::string& address, std::uint16_t port)
    {
        mRemoteAddress = address;
        mRemotePort = port;
    }

    void setLocalAddress(const std::string& address) {
        mLocalAddress = address;
    }

    void setComputerName(const std::string& name) {
        mComputerName = name;
    }

    void remoteRegister();
    void remoteUnregister();
    std::list<std::string> getServerList();
    bool setSessionPin(const std::string& server_name, const std::string& pin);
    std::string getSessionPin(const std::string& server_name = "");
private:
    explicit SsmnBackendApi();
    ~SsmnBackendApi();
    SsmnBackendApi(const SsmnBackendApi&) = delete;
    SsmnBackendApi(SsmnBackendApi&&) noexcept = delete;
    SsmnBackendApi& operator = (const SsmnBackendApi&) = delete;
    SsmnBackendApi& operator = (SsmnBackendApi&&) noexcept = delete;

    std::string doPostRequest(const std::string& api, arg_map& args);

    std::string mRemoteAddress;
    std::string mLocalAddress;
    std::string mComputerName;
    std::uint16_t mRemotePort = 0;
};
} // namespace ssmn

#endif /* SSMN_BACKEND_API */
