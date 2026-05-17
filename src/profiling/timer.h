#pragma once

#include <chrono>
#include <string>

template <typename ClockT>
class NamedTimer {    
 public:             
  using clock_t = ClockT;
  using time_point_t = typename std::chrono::time_point<ClockT>;
  using duration_t = typename ClockT::duration;
                                              
  explicit NamedTimer(std::string name)
      : startedAt{clock_t::now()},
        stoppedAt{startedAt},
        name{std::move(name)} {}

  time_point_t stop() {
    stoppedAt = clock_t::now();
    return stoppedAt;
  }

  duration_t elapsed() const { return stoppedAt - startedAt; }

  std::string getName() const { return name; } 

  time_point_t getStartedAt() const { return startedAt; }

  time_point_t getStoppedAt() const { return stoppedAt; }

 private:
  time_point_t startedAt;
  time_point_t stoppedAt;
  std::string name;
};

template <typename ClockT>
class ScopedProbe {
 public:
  using timer_t = NamedTimer<ClockT>;
  using callback_t =
      std::function<void(std::string name, typename timer_t::time_point_t ts)>;

  ScopedProbe(std::string name, callback_t entry_cb, callback_t exit_cb)
      : t{name}, entry_cb{std::move(entry_cb)}, exit_cb{std::move(exit_cb)} {
    this->entry_cb(t.getName(), t.getStartedAt());
  }

  ~ScopedProbe() { exit_cb(t.getName(), t.stop()); }

 private:
  timer_t t;
  callback_t entry_cb;
  callback_t exit_cb;
};