#include <chrono>
#include <thread>

#include "curlrequest.h"
#include <iostream>

namespace ssmn {
namespace CurlRequest {

// libcurl helper function to read result value
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  auto length = size * nmemb;
  auto str = reinterpret_cast<std::string*>(userp);
  if (str == nullptr) {
    return 0;
  }

  str->append((char*)contents, length);
  return length;
}

void init()
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void deinit()
{
  curl_global_cleanup();
}

std::string DoRequest(
  CURL *handle,
  CURLcode *retValue,
  int max_attempts,
  unsigned long timeout)
{
  std::string result;

  // set request timeout
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, timeout);

  // configure output
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result);

  // TODO: investigate how to deal with certificates properly
  curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);

  int attempt = 0;
  CURLcode ret = CURLE_OK;
  do {
    result.clear();
    ret = curl_easy_perform(handle); /* post away! */
    if (retValue != nullptr) {
      *retValue = ret;
    }

    if (ret != CURLE_OK && ret != CURLE_OPERATION_TIMEDOUT) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else if (ret != CURLE_OK) {
      std::cout << "curl_easy_strerror(ret ): " << curl_easy_strerror(ret) << std::endl;
    }

    attempt++;
  } while (ret != CURLE_OK && max_attempts > 0 && attempt < max_attempts);

  return result;
}

std::string GetUrl(
  const std::string &url,
  CURLcode *retValue,
  int max_attempts,
  unsigned long timeout)
{
  if (url.empty()) {
    return std::string();
  }

  auto handle = curl_easy_init();
  if (handle == nullptr) {
    return std::string();
  }

  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle, CURLOPT_POST, 0L);

  // fill headers
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Accept: */*");
  curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

  std::string result = DoRequest(handle, retValue, max_attempts, timeout /* milliseconds */);

  // cleanup
  curl_slist_free_all(headers); /* free the header list */
  curl_easy_cleanup(handle);

  return result;
}

std::string PostUrl(
  const std::string &url,
  const std::string &data,
  const std::vector<std::string> &headersList,
  CURLcode *retValue,
  int max_attempts,
  unsigned long timeout)
{
  if (url.empty()) {
    return std::string();
  }

  auto handle = curl_easy_init();
  if (handle == nullptr) {
    return std::string();
  }

  curl_easy_setopt(handle, CURLOPT_POST, 1L);
  curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data.size());

  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

  // fill headers
  struct curl_slist *headers = nullptr;
  for(const auto& str: headersList) {
    headers = curl_slist_append(headers, str.c_str());
  }
  curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

  std::string result = DoRequest(handle, retValue, max_attempts, timeout);

  // cleanup
  curl_slist_free_all(headers); /* free the header list */
  curl_easy_cleanup(handle);

  return result;
}

std::string PostMimeUrl(
  const std::string &url,
  const std::vector<std::pair<std::string, std::string>> &mime,
  CURLcode *retValue,
  int max_attempts,
  unsigned long timeout)
{
  if (url.empty()) {
    return std::string();
  }

  auto handle = curl_easy_init();
  if (handle == nullptr) {
    return std::string();
  }

  curl_mime *form = curl_mime_init(handle);

  for(const auto& pair: mime) {
    curl_mimepart *field = curl_mime_addpart(form);
    curl_mime_name(field, pair.first.c_str());
    curl_mime_data(field, pair.second.c_str(), CURL_ZERO_TERMINATED);
  }

  curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

  curl_easy_setopt(handle, CURLOPT_MIMEPOST, form);

  // fill headers
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Accept: */*");
  curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

  std::string result = DoRequest(handle, retValue, max_attempts, timeout);

  // cleanup
  curl_slist_free_all(headers); /* free the header list */
  curl_mime_free(form);
  curl_easy_cleanup(handle);

  return result;
}

} // namesmace ssmn
} // namespace CurlRequest
