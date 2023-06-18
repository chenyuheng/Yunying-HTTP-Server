#ifndef YY_ORIGIN
#define YY_ORIGIN

#include "http.hpp"

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
            StaticFileOrigin();
            StaticFileOrigin(std::string root_dir);
            ~StaticFileOrigin();
            std::string getKey(HttpRequest request);
            HttpResponse* get(HttpRequest request, int* max_age);
    };
}

#endif