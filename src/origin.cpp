#include "origin.hpp"

#include <curl/curl.h>

namespace yunying {

StaticFileOrigin::StaticFileOrigin() {
  root_dir_ = Conf::getInstance().get_root_dir();
}

StaticFileOrigin::~StaticFileOrigin() {
  // TODO
}

std::string StaticFileOrigin::handleDefault(std::string path) {
  if (path[path.length() - 1] == '/') {
    path += "index.html";
  }
  return path;
}

bool isDirectory(std::string path) {
  struct stat path_stat;
  stat(path.c_str(), &path_stat);
  return S_ISDIR(path_stat.st_mode);
}

std::string StaticFileOrigin::getKey(HttpRequest request) {
  return MethodString[request.get_method()] + handleDefault(request.get_path());
}

HttpResponse* StaticFileOrigin::get(HttpRequest request, int* max_age) {
  HttpResponse* response = new HttpResponse();
  *max_age = Conf::getInstance().get_cache_default_max_age();
  if (request.get_method() != HttpMethod::GET) {
    response->set_status(HttpStatus::METHOD_NOT_ALLOWED);
    return response;
  }
  std::string path = root_dir_ + handleDefault(request.get_path()) + "/";
  if (isDirectory(path)) {
    path = handleDefault(path);
  }
  FILE* file = fopen(path.c_str(), "rb");
  if (file == NULL) {
    response->set_status(HttpStatus::NOT_FOUND);
  } else {
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* file_content = (char*)malloc(file_size);
    int ret = fread(file_content, 1, file_size, file);
    if (ret < 0) {
      perror("fread");
      response->set_status(HttpStatus::INTERNAL_SERVER_ERROR);
      return response;
    }
    fclose(file);
    response->set_body(std::string(file_content, file_size));
    free(file_content);
    response->set_status(HttpStatus::OK);
  }
  return response;
}

UpstreamOrigin::UpstreamOrigin() {}

UpstreamOrigin::~UpstreamOrigin() {
  // TODO
}

std::string UpstreamOrigin::getKey(HttpRequest request) {
  return request.get_host() + MethodString[request.get_method()] +
         request.get_path();
}

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
  HttpResponse* response = (HttpResponse*)userdata;
  response->set_body(response->get_body() + std::string(ptr, size * nmemb));
  return size * nmemb;
}

size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  HttpResponse* response = (HttpResponse*)userdata;
  std::string header(buffer, nitems);
  std::vector<std::string> header_pair = split(header, ":", 2);
  if (header_pair.size() < 2) {
    return size * nitems;
  }
  std::string header_key = header_pair[0];
  std::string header_value = header_pair[1];
  trim(header_key);
  trim(header_value);
  for (auto& c : header_key) c = toupper(c);
  response->set_header(header_key, header_value);
  return size * nitems;
}

HttpResponse* UpstreamOrigin::get(HttpRequest request, int* max_age) {
  HttpResponse* response = new HttpResponse();
  CURL* curl = curl_easy_init();
  if (!curl) {
    response->set_status(HttpStatus::INTERNAL_SERVER_ERROR);
    return response;
  }

  HttpMethod upstream_method = request.get_method();
  std::string upstream_protocol = Conf::getInstance().get_upstream_protocol();
  std::string upstream_domain = Conf::getInstance().get_upstream_domain();
  int upstream_port = Conf::getInstance().get_upstream_port();
  std::string upstream_path = request.get_path();
  std::string upstream_host = Conf::getInstance().get_upstream_host();
  std::string upstream_body = request.get_body();

  Conf::getInstance().get_lua().set_function(
      "set_method", [&upstream_method](std::string method_str) {
        if (MethodDict.find(method_str) == MethodDict.end()) {
          upstream_method = MethodDict[method_str];
        } else {
          upstream_method = HttpMethod::OTHER;
        }
      });
  Conf::getInstance().get_lua().set_function(
      "set_path",
      [&upstream_path](std::string path_str) { upstream_path = path_str; });
  Conf::getInstance().get_lua().set_function(
      "set_host",
      [&upstream_host](std::string host_str) { upstream_host = host_str; });
  Conf::getInstance().get_lua().set_function(
      "set_body",
      [&upstream_body](std::string body_str) { upstream_body = body_str; });
  Conf::getInstance().get_lua().set_function(
      "set_upstream_domain", [&upstream_domain](std::string domain_str) {
        upstream_domain = domain_str;
      });
  Conf::getInstance().get_lua().set_function(
      "set_upstream_port",
      [&upstream_port](int port) { upstream_port = port; });

  sol::function upstream_set = Conf::getInstance().get_lua()["upstream_set"];
  if (upstream_set.valid()) {
    std::function<void(std::string, std::string, std::string)>
        upstream_set_func = upstream_set;
    upstream_set_func(request.get_path(), MethodString[request.get_method()],
                      request.get_host());
  }

  std::string upstream_url =
      upstream_protocol + "://" + upstream_domain +
      ((upstream_port == 0) ? "" : (":" + std::to_string(upstream_port))) +
      upstream_path;
  printf("upstream_url: %s\n", upstream_url.c_str());
  curl_easy_setopt(curl, CURLOPT_URL, upstream_url.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, MethodString[upstream_method].c_str());
  if (upstream_body.length() > 0) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, upstream_body.c_str());
  }
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, response);
  struct curl_slist* headers = NULL;
  printf("upstream_host: %s\n", upstream_host.c_str());
  headers = curl_slist_append(headers, ("Host: " + upstream_host).c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    response->set_status(HttpStatus::INTERNAL_SERVER_ERROR);
  }
  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  response->set_status((HttpStatus)http_code);
  curl_easy_cleanup(curl);
  response->delete_header("TRANSFER-ENCODING");
  return response;
}
}  // namespace yunying