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
  //notify and join
  condition_var.notify_one();
  for (auto &thread : thread_pool_) {
    thread.join();
  }
};

void MessageQueue::Run(BaseMessagesProviderWptr provider) {
  provider_ = provider;
  RequestEvents();
}

bool MessageQueue::IsRunning() const {
  return is_running_.load(std::memory_order_acquire);
}

void MessageQueue::RequestStop() {
  provider_.reset();
  is_running_.store(false, std::memory_order_release);
}

void MessageQueue::PostJob(Job job) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.emplace(std::move(job));
  }
  condition_var.notify_one();
}

void MessageQueue::osThreadExecutionLoop() {
  do {
    Job job;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (messages_.empty()) {
        RequestEvents();
      }
      condition_var.wait(lock,
                         [this] {
                           return !messages_.empty() || IsRunning();
                         });
      if (!IsRunning()) {
        return;
      }
      if (!messages_.empty()) {
        job = std::move(messages_.front());
        messages_.pop();
      }
    }

    try {
      if (job) {
        if (job()) {
          // post job back till we can handle it
          PostJob(std::move(job));
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
void MessageQueue::RequestEvents() {
  if (auto provider = provider_.lock()) {
    const auto amount = thread_pool_.size() * 2;
    for (auto &message : provider->GenerateJobs(amount)) {
      messages_.push(message);
    }
  }
}
