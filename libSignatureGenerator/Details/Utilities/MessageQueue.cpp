/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/


#include "MessageQueue.h"

unsigned MessageQueue::RecommendedAmountOfThreads() {
  return std::thread::hardware_concurrency();
}

MessageQueue::MessageQueue(unsigned threads_count) {
  assert(threads_count > 1 && "using message queue with low number of threads is useless");
  thread_pool_.reserve(threads_count);
  for (int i = 0; i < threads_count; ++i) {
    // TBD(EZ): reduce stack size of the each thread
    thread_pool_.emplace_back(&MessageQueue::osThreadExecutionLoop, this);
  }
}

MessageQueue::~MessageQueue() {
  // TBD(EZ): avoid possible UB (access to thread fields by job  after destructor call) by adding public join
  RequestStop();
  for (auto &thread : thread_pool_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
};

void MessageQueue::Execute() {
  RequestEvents();
  for (auto &thread : thread_pool_) {
    assert(thread.joinable());
    thread.join();
  }
}

bool MessageQueue::IsRunning() const {
  return is_running_.load(std::memory_order_acquire);
}

void MessageQueue::RequestStop() {
  is_running_.store(false, std::memory_order_release);
  condition_var.notify_all();
}

void MessageQueue::PostJob(Job job) {
  {
    std::lock_guard<MutexType> lock(mutex_);
    queue_.emplace(std::move(job));
  }
  condition_var.notify_one();
}

void MessageQueue::osThreadExecutionLoop() {
  do {
    Job next_job;
    {
      std::unique_lock<MutexType> lock(mutex_);
      if (queue_.empty()) {
        if (!RequestEvents()) {
          // no more data is available - finish thread
          return;
        }
      };
      condition_var.wait(lock, [this] { return !queue_.empty() || !IsRunning(); });
      if (!IsRunning()) {
        return;
      }
      assert(!queue_.empty());
      next_job = std::move(queue_.front());
      queue_.pop();
    }
    try {
      // check functor for validity
      if (next_job) {
        // run the job
        if (!next_job()) {
          // push failed job back
          PostJob(std::move(next_job));
        };
      }
    } catch (...) {
      // catch the first exception
      if (!exception_ptr_) {
        exception_ptr_ = std::current_exception();
      }
      RequestStop();
    }
  } while (IsRunning());
}
bool MessageQueue::RequestEvents() {
  const auto optional_result = request_jobs_signal_(thread_pool_.size());
  if (optional_result) {
    return *optional_result;
  }
  return false;
}
void MessageQueue::ConnectJobsProvider(const ProvideJobsSlot &slot) {
  request_jobs_signal_.connect(slot);
}
