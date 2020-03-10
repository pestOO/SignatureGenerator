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
  assert(jobs_provider_ && "No jobs provider, which generates jobs");
  RequestJobs();
  for (auto &thread : thread_pool_) {
    assert(thread.joinable());
    thread.join();
  }
}

bool MessageQueue::IsFinished() const {
  return queue_jobs_finished_.load(std::memory_order_acquire);
}
void MessageQueue::RequestFinish() {
  queue_jobs_finished_.store(true, std::memory_order_release);
  condition_var.notify_all();
}

bool MessageQueue::IsStopped() const {
  return queue_stopped_.load(std::memory_order_acquire);
}

void MessageQueue::RequestStop() {
  queue_stopped_.store(false, std::memory_order_release);
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
      if (queue_.empty() && !IsFinished()) {
        RequestJobs();
      };
      condition_var.wait(lock, [this] {
        return !queue_.empty() || IsStopped() || IsFinished();
      });
      if (IsStopped() || (IsFinished() && queue_.empty())) {
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
  } while (!IsStopped());
}
void MessageQueue::RequestJobs() {
  if(jobs_provider_) {
    const auto more_jobs_are_expected = jobs_provider_(thread_pool_.size());
    if (!more_jobs_are_expected) {
      RequestFinish();
    }
  }
}
