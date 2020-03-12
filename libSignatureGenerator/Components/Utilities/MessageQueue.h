/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_MESSAGEQUEUE_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_MESSAGEQUEUE_H_
#pragma once

#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <boost/lockfree/stack.hpp>

#include "CommonTypes.h"

/**
 * Functional object to be called in the tread.
 * @return true if job was done, otherwise false
 *
 * Job shall return fall only when job could not be handle NOW, but can be handled later/
 * Job shall throw exception if job could not be handle at all.
 */
using Job = std::function<bool()>;
using JobSptr = std::shared_ptr<Job>;

class MessageQueue final {
 public:
  // -- Typedef and using --
  using JobsProvider = std::function<bool(int)>;
  using MutexType = std::recursive_mutex;
  // -- Class members --
  /**
   * Constructs MessageQueue with a pool of ${threads_count} Threads.
   * @param threads_count of threads to be created and warmed up for execution
   *
   * @note Use RecommendedAmountOfThreads as a hint to determine threads_count of threads.
   */
  explicit MessageQueue();

  ~MessageQueue();

  void PostJob(Job job);

  void Start();

  void SetJobsProvider(const JobsProvider& provider) {
    jobs_provider_ = provider;
  }

 private:
  void osThreadExecutionLoop();

  void CreateThreads();

  void JoinThreads();

  /**
   * @return
   */
  void RequestJobs();

  void RequestStop();

  bool IsStopped() const;

  void RequestFinish();
  bool IsFinished() const;

  // Stop meas that for some reasons all threads need to me immediately stopped
  std::atomic_bool queue_stopped_ = ATOMIC_VAR_INIT(false);
  // Finish means that there is no more jobs are expected - we can finish waiting threads
  std::atomic_bool queue_jobs_finished_ = ATOMIC_VAR_INIT(false);
  const unsigned threads_count_;
  std::vector<std::thread> thread_pool_;
  std::exception_ptr first_thrown_exception_ptr_;

  // stack is used for two reasons:
  // - we want to promote execution to reading
  // - boost::lockfree::queue requires trivial destructor
  boost::lockfree::stack<Job> jobs_stack_;

  JobsProvider jobs_provider_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_MESSAGEQUEUE_H_