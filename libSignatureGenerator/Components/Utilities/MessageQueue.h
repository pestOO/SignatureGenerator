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
 * Implements Message Queue pattern for heavy (non-stoppable) operations processing.
 *
 * @warning optimised with a lock-free structure, - waiting new jobs is very expensive for CPU.
 */
class MessageQueue final {
 public:
  /**
   * Constructs MessageQueue with a pool of threads for execution.
   * Threads are not warmed upa nd waiting Start call.
   * @warning Relies on std::thread::hardware_concurrency, which could be nto implmented for some platforms/compiler.
   */
  explicit MessageQueue();

  /**
   * Force all threads to stop. Wait all threads to be stopped.
   */
  ~MessageQueue();

  /**
   * Post one job to the message queue.
   * @param job to be executed in one the threads.
   */
  void PostJob(const Job &job);

  /**
   * Start all threads for jobs execution.
   * @note blocks calling thread and reuse it for jobs proessing.
   */
  void Start();

  using JobsProvider = std::function<bool(unsigned)>;
  /**
   *  Set Jobs provider, which will be requested for a new job each time queue is empty.
   * @param provider
   */
  void SetJobsProvider(const JobsProvider &provider);

 private:
  void osThreadExecutionLoop();
  void CreateThreads();
  void JoinThreads();
  void RequestNewJobs();
  void RequestStop();
  bool IsStopped() const;
  void RequestFinish();
  bool IsFinished() const;

  // Stop meas that for some reasons all threads need to me immediately stopped
  std::atomic_bool queue_stopped_ = ATOMIC_VAR_INIT(false);
  // Finish means that there is no more jobs are expected - we can finish jobs before exit
  std::atomic_bool queue_jobs_finished_ = ATOMIC_VAR_INIT(false);

  const unsigned threads_count_;
  std::vector<std::thread> thread_pool_;
  std::exception_ptr first_thrown_exception_ptr_;

  // stack is used for several reasons:
  // - we want to promote execution (CPU load) to reading (IO operation)
  // - we can reduce the case when all memory is occupied by read data and we have no resources to calculate
  // - boost::lockfree::queue requires trivial destructor
  boost::lockfree::stack<Job> jobs_stack_;

  JobsProvider jobs_provider_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_MESSAGEQUEUE_H_
