#ifndef WORKER_HPP
#define WORKER_HPP

#include <thread>

#include "concurrent_queue.hpp"
#include "device/adapter/interface_device_adapter.hpp"
#include "device/event/interface_event_sink.hpp"

class Worker {
  public:
    Worker(ConcurrentQueue<std::string>& queue, IDeviceAdapter& adapter, IEventSink& eventSink);

    void start();
    void stop();
    void join();

  private:
    void run();

    ConcurrentQueue<std::string>& queue_;
    IDeviceAdapter& adapter_;
    IEventSink& eventSink_;

    std::thread thread_;
};

#endif  // WORKER_HPP