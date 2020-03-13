/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamahov.
 *****************************************************************************/

#ifndef LIBRARY_LIBRARY_H
#define LIBRARY_LIBRARY_H

#include <cinttypes>
#include <string>
#include <memory>

//forward declaration
class MessageQueue;
class InputDataProvider;
class SignatureGenerator;
class OutputDataConsumer;

/**
 * @brief The Library class wraps all utilities and system layers.
 * Allows to re-use library with custom user interface or as a part of an other application
 */
class Library {
 public:
  /**
   * @brief Library default constructor
   */
  Library() = default;

  /**
   * @brief clean-up library components
   */
  virtual ~Library();

  /**
   * @brief Start execution
   * Reuses the caller thread for data processing in one of the configured threads
   *
   * @param input_file in a current system format
   * @param out_file in a current system format
   * @param chunk_size in bytes
   *
   * @throws a various types of exception with valid description
   *
   * @note "What" information of the thrown exception could be used by integration engineer
   *
   * @warning "What" information of the thrown exception couldn't be used for multi-language UI
   */
  void run(const std::string &input_file, const std::string &out_file, const std::uint64_t chunk_size);

 private:
  std::shared_ptr<MessageQueue> message_queue_;
  std::shared_ptr<InputDataProvider> input_data_provider_;
  std::shared_ptr<SignatureGenerator> signature_generator_;
  std::shared_ptr<OutputDataConsumer> output_data_consumer_;
};

#endif //LIBRARY_LIBRARY_H
