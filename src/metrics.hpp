#ifndef YY_METRICS
#define YY_METRICS

#include <string>
#include <unordered_map>
#include <mutex>

namespace yunying {
    class Metrics {
    public:
        static Metrics& getInstance() {
            static Metrics instance;
            return instance;
        }
        void count(std::string key, double value) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (metrics_.find(key) == metrics_.end()) {
                metrics_[key] = value;
            } else {
                metrics_[key] += value;
            }
        }

        double get(std::string key) {
            if (metrics_.find(key) == metrics_.end()) {
                return 0;
            } else {
                return metrics_[key];
            }
        }

    private:
        std::unordered_map<std::string, double> metrics_;
        Metrics() {}
        Metrics(Metrics const&);
        void operator=(Metrics const&);

        std::mutex mutex_;
    };
}

#endif