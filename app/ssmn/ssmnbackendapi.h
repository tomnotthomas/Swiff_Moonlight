#ifndef SSMN_BACKEND_API
#define SSMN_BACKEND_API

#include <string>
#include <cstdint>
#include <map>

namespace ssmn {
    class SsmnBackendApi {
        public:
            static constexpr const char* kApiServerList = "/api/server_list";
            static constexpr const char *kApiSessions = "/api/sessions";
            static constexpr const char *kApiRegister = "/api/register";
            static constexpr const char *kApiUnRegister = "/api/unregister";
            static constexpr const char *kApiValidateSessionId = "/api/validate_session";

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

            void setAddress(const std::string& address) {
                mLocalAddress = address;
            }

            void setComputerName(const std::string& name) {
                mComputerName = name;
            }

            void remoteRegister();
            void remoteUnregister();
            void serverList();
            std::string getSessionId();
            bool validateSessionId(const std::string& session_id);
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
