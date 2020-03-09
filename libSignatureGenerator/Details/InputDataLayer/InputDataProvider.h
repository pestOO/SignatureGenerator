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
#include <queue>
#include <string>

// TODO(EZ): use Pimpl to hide all implementation in other file or move to utilities
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/sync/file_lock.hpp>

#include "Utilities/CommonTypes.h"

//forward declaration
class MessageQueue;

/**
 *
 * Flow: get file chunk next_chunk_offset_ -> read chunk
 *
 * @todo(EZ): we can create a memory pool and store data in the pool
 * @todo(EZ) extract interface and implementation to separate files
 */
class InputDataProvider {
 public:
  // -- Support structures and listener of this data --
  /**
   * to move in other file
   */
  class DataChunk {
   public:
    virtual void *GetData() const = 0;
    virtual ChunkSizeType GetSize() const = 0;
    virtual UniqueId GetUniqueId() const = 0;
    virtual ~DataChunk() = default;
  };
  // -- Typedef and using --
  using DataChunkSptr = std::shared_ptr<DataChunk>;
  using OffsetType = std::uint64_t;
  using DataAvailableSignal = SignalType<void(const DataChunkSptr &)>;
  using DataAvailableSlot = DataAvailableSignal::slot_type;
  // -- Class members --
  InputDataProvider(std::string file_path, ChunkSizeType chunk_size, MessageQueue &message_queue);

  void RunFirstJob();

  void PostJob();

  const std::uintmax_t  GetFileSize() const;
  /**
   * Connects
   * @warning not thread-safe
   */
  void ConnectChunkDataListener(const DataAvailableSlot &slot);

 private:
  DataChunkSptr GenerateNextDataChunk(const UniqueId unique_id, const OffsetType offset);

 private:
  // async working queue
  MessageQueue &message_queue_;
  //File reading information
  const std::string file_path_;
  const ChunkSizeType chunk_size_;
  const std::uintmax_t file_size_;
 private:
  // blocks other process to change the file
  boost::interprocess::file_lock file_lock_;
  // The next chunk info
  std::atomic<UniqueId> next_chunk_id_ = ATOMIC_VAR_INIT(0u);
  // data consumer
  DataAvailableSignal on_data_available_signal_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_INPUTDATALAYER_INPUTDATAPROVIDER_H_
