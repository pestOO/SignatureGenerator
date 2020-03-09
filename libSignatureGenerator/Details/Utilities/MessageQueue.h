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

/**
 * Functional object to be called in the tread.
 * @return true if job was done, otherwise false
 *
 * Job shall return fall only when job could not be handle NOW, but can be handled later/
 * Job shall throw exception if job could not be handle at all.
 */
using Job = std::function<bool()>;
using JobSptr = std::shared_ptr<Job>;

class BaseMessagesProvider {
 public:
  virtual std::vector<Job> GenerateJobs(unsigned amount) = 0;
};

using BaseMessagesProviderSptr = std::shared_ptr<BaseMessagesProvider>;
using BaseMessagesProviderWptr = std::weak_ptr<BaseMessagesProvider>;

class MessageQueue final {
 public:
  /**
   * Constructs MessageQueue with a pool of ${threads_count} Threads.
   * @param threads_count of threads to be created and warmed up for execution
   *
   * @note Use RecommendedAmountOfThreads as a hint to determine threads_count of threads.
   */
  explicit MessageQueue(unsigned threads_count);

  void Run(BaseMessagesProviderWptr provider);

  ~MessageQueue();

  void PostJob(Job job);

  /**
   * @return Preferred amount of threads to be used for the best CPUs usage.
   * @note If this value is not computable or well defined, the function returns 0.
   */
  static unsigned RecommendedAmountOfThreads();
 private:
  void osThreadExecutionLoop();

  void RequestStop();

  bool IsRunning() const;

  void RequestEvents();

  std::atomic_bool is_running_ = ATOMIC_VAR_INIT(true);
  std::vector<std::thread> thread_pool_;
  std::mutex mutex_;
  std::condition_variable condition_var;
  std::exception_ptr exception_ptr_; // ??

  // TBD(EZ): move to lock free queue
  std::queue<Job> messages_;

  std::weak_ptr<BaseMessagesProvider> provider_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_MESSAGEQUEUE_H_
