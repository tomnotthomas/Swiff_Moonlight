#pragma once

#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H

#include <string>
#include <curl/curl.h>
#include <vector>

namespace ssmn {
namespace CurlRequest
{
  void init();
  void deinit();

  std::string GetUrl(const std::string &url, CURLcode *retValue = nullptr, int max_attempts = 1, unsigned long timeout = 5000);
  std::string PostUrl(const std::string &url, const std::string &data, const std::vector<std::string> &headersList,
	  				          CURLcode *retValue = nullptr, int max_attempts = 1, unsigned long timeout = 5000);
  std::string PostMimeUrl(const std::string &url, const std::vector<std::pair<std::string, std::string>> &mime,
	  					            CURLcode *retValue = nullptr, int max_attempts = 1, unsigned long timeout = 5000);
} // namespace CurlRequest
} // namespace ssmn
#endif // CURL_REQUEST_H
