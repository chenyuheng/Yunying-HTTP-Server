#ifndef YY_ORIGIN
#define YY_ORIGIN

#include "http.hpp"
#include "conf.hpp"
#include "connection.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

namespace yunying {
    class Origin {
        public:
            virtual std::string getKey(HttpRequest request) = 0;
            virtual HttpResponse* get(HttpRequest request, int* max_age) = 0;
    };

    class StaticFileOrigin : public Origin {
        private:
            std::string root_dir_;
            std::string handleDefault(std::string path);
        public:
            StaticFileOrigin() = delete;
            StaticFileOrigin(std::string root_dir);
            ~StaticFileOrigin();
            std::string getKey(HttpRequest request);
            HttpResponse* get(HttpRequest request, int* max_age);
    };

    class UpstreamOrigin : public Origin {
        public:
            UpstreamOrigin();
            ~UpstreamOrigin();
            std::string getKey(HttpRequest request);
            HttpResponse* get(HttpRequest request, int* max_age);
    };
}

#endif