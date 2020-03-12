/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "MessageQueue.h"

#include <iostream>

/**
 * @return Preferred amount of threads to be used for the best CPUs usage.
 * @note If this value is not computable or well defined, the function returns 0.
 */
unsigned RecommendedAmountOfThreads() {
  return std::thread::hardware_concurrency();
}

MessageQueue::MessageQueue()
    : threads_count_(RecommendedAmountOfThreads()),
    // each thread can request for threads_count jobs
      jobs_stack_(threads_count_ * threads_count_) {
  assert(threads_count_ > 1 && "Could not determine amount of available cpu/cores.");
}

MessageQueue::~MessageQueue() {
  // TBD(EZ): avoid possible UB (access to thread fields by job  after destructor call) by adding public join
  RequestStop();
};

void MessageQueue::Start() {
  assert(jobs_provider_ && "No jobs provider, which generates jobs");
  RequestJobs();
  // use the current thread for execution
  osThreadExecutionLoop();

  JoinThreads();
  if (first_thrown_exception_ptr_) {
    std::rethrow_exception(first_thrown_exception_ptr_);
  }
}


void MessageQueue::CreateThreads() {
  thread_pool_.reserve(threads_count_);
  for (int i = 0; i < threads_count_; ++i) {
    // TBD(EZ): reduce stack size of the each thread
    thread_pool_.emplace_back(&MessageQueue::osThreadExecutionLoop, this);
  }
}

void MessageQueue::JoinThreads() {
  for (auto &thread : thread_pool_) {
    if (thread.joinable()) {
      assert(thread.joinable() && "all created threads shall be joinable");
      thread.join();
    }
  }
}

bool MessageQueue::IsFinished() const {
  return queue_jobs_finished_.load(std::memory_order_acquire);
}

void MessageQueue::RequestFinish() {
  queue_jobs_finished_.store(true, std::memory_order_release);
}

bool MessageQueue::IsStopped() const {
  return queue_stopped_.load(std::memory_order_acquire);
}

void MessageQueue::RequestStop() {
  queue_stopped_.store(false, std::memory_order_release);
}

void MessageQueue::PostJob(Job job) {
  jobs_stack_.push(job);
}

void MessageQueue::osThreadExecutionLoop() {
  do {
    Job next_job;
    const bool is_empty = !jobs_stack_.pop(next_job);
    if (IsStopped() || (IsFinished() && is_empty)) {
      return;
    }
    if (is_empty && !IsFinished()) {
      RequestJobs();
    };
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
      if (!first_thrown_exception_ptr_) {
        first_thrown_exception_ptr_ = std::current_exception();
      }
      RequestStop();
    }
  } while (!IsStopped());
}
void MessageQueue::RequestJobs() {
  if (jobs_provider_) {
    // in case od
    const auto amount = thread_pool_.size();
    const auto more_jobs_are_expected = jobs_provider_(amount > 0 ? amount : 1 );
    if (!more_jobs_are_expected) {
      RequestFinish();
    }
  }
}
