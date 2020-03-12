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

#include <boost/config.hpp>

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

void MessageQueue::SetJobsProvider(const MessageQueue::JobsProvider &provider) {
  jobs_provider_ = provider;
}

MessageQueue::~MessageQueue() {
  RequestStop();
  JoinThreads();
};

void MessageQueue::Start() {
  assert(jobs_provider_ && "No jobs provider, which generates jobs");
  RequestNewJobs();
  CreateThreads();
  // re-use the current main thread for real work
  osThreadExecutionLoop();
  JoinThreads();
  if (first_thrown_exception_ptr_) {
    std::rethrow_exception(first_thrown_exception_ptr_);
  }
}

void MessageQueue::CreateThreads() {
  thread_pool_.reserve(threads_count_);
  for (unsigned i = 0; i < threads_count_; ++i) {
    // TODO: we can reduce stack size of the each thread
    thread_pool_.emplace_back(&MessageQueue::osThreadExecutionLoop, this);
  }
}

void MessageQueue::JoinThreads() {
  for (auto &thread : thread_pool_) {
    assert(thread.joinable() && "all created threads shall be joinable");
    if (thread.joinable()) {
      thread.join();
    }
  }
  thread_pool_.clear();
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

void MessageQueue::PostJob(const Job &job) {
  jobs_stack_.push(job);
}

void MessageQueue::osThreadExecutionLoop() {
  do {
    Job next_job;
    const bool is_stack_empty = !jobs_stack_.pop(next_job);
    if (is_stack_empty) {
      if (BOOST_UNLIKELY(IsFinished())) {
        return;
      }
      RequestNewJobs();
      continue;
    };
    try {
      // check job for validity
      if (BOOST_LIKELY(!!next_job)) {
        const auto job_succeed = next_job();
        // run the job
        if (!BOOST_LIKELY(job_succeed)) {
          // push failed job back
          PostJob(next_job);
        };
      }
    } catch (...) {
      RequestStop();
      // catch the first exception
      if (!first_thrown_exception_ptr_) {
        first_thrown_exception_ptr_ = std::current_exception();
      }
    }
  } while (BOOST_LIKELY(!IsStopped()));
}

void MessageQueue::RequestNewJobs() {
  if (jobs_provider_) {
    assert(thread_pool_.size() < std::numeric_limits<unsigned>::max() && "pool size does not fit type");
    const auto amount = static_cast<unsigned>(thread_pool_.size());
    const auto more_jobs_are_expected = jobs_provider_(amount > 0 ? amount : 1);
    if (BOOST_UNLIKELY(!more_jobs_are_expected)) {
      RequestFinish();
    }
  }
}
