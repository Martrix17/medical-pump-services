#ifndef CONCCURENT_QUEUE_HPP
#define CONCCURENT_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <typename T> class ConcurrentQueue {
  public:
    // ConcurrentQueue(const ConcurrentQueue&) = delete;
    // ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    bool push(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stopped_) {
                return false;
            }

            queue_.push(std::move(value));
        }

        condition_.notify_one();
        return true;
    }

    std::optional<T> waitAndPop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return stopped_ || !queue_.empty(); });

        if (queue_.empty()) {
            return std::nullopt;
        }

        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        condition_.notify_all();
    }

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;

    bool stopped_ = false;
};

#endif  // CONCCURENT_QUEUE_HPP