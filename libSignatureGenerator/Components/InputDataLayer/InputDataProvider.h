/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_INPUTDATALAYER_INPUTDATAPROVIDER_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_INPUTDATALAYER_INPUTDATAPROVIDER_H_
#pragma once

#include <cinttypes>
#include <memory>
#include <string>

// TODO(EZ): use Pimpl to hide all implementation in other file or move to utilities
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/sync/file_lock.hpp>

#include "Utilities/CommonTypes.h"
#include "InputDataLayer/details/InDataListener.h"

//forward declaration
class MessageQueue;

/**
 * Class abstracts input data caching and validation.
 * Facade object for all input data processing.
 *
 * @todo: Later on we can create a memory pool and store data in the pool
 */
class InputDataProvider {
 public:

  /**
   * Constructs input data layer component.
   * @param file_path of the file to be read
   * @param chunk_size size of the chunks to be splitter
   * @param message_queue for posting Jobs
   */
  InputDataProvider(std::string file_path, ChunkSize chunk_size, MessageQueue &message_queue);

  using InDataListenerSptr = std::shared_ptr<InDataListener>;
  using InDataListenerWptr = std::weak_ptr<InDataListener>;
  /**
   * Connect one unique listener. Override previous listeners.
   * @warning setter itself is not thread-safe.
   */
  void SetConnectChunkDataListener(const InDataListenerSptr &listener);

  /**
   * Starts several jobs with reading chunks from the inpout file.
   *
   * @param jobs_amount to be posted to the queue one by one.
   * @return true if more chunks for processing is available, otherwise false.
   */
  bool RequestChunkRead(unsigned jobs_amount);

  /**
   * @return size of the input file in bytes.
   */
  std::uintmax_t GetFileSize() const;

  /**
   * Determines sizew of the output file base on the size of the input file,
   * chunk size and given chunk
   * @param chunks_output_size
   * @return size of the output file in bytes.
   *
   * @todo move to utilities due to mix of layer responsibilities
   */
  std::uintmax_t PredictOutFileSize(int chunks_output_size) const;

 private:
  void PostMQJob(NumericOrder numeric_order, Offset offset);

  InDataChunkSptr GenerateNextDataChunk(const NumericOrder numeric_order, const Offset offset);

  // async working queue
  MessageQueue &message_queue_;
  // File reading information
  const std::string file_path_;
  const ChunkSize chunk_size_;
  const std::uintmax_t file_size_;
  // blocks other process to change the file
  boost::interprocess::file_lock file_lock_;
  // The next chunk info
  std::atomic<NumericOrder> next_chunk_id_ = ATOMIC_VAR_INIT(0u);
  // data consumer
  InDataListenerWptr on_data_available_listener_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_INPUTDATALAYER_INPUTDATAPROVIDER_H_
