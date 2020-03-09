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

#include <boost/signals2.hpp>

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
  // -- Typedef and using --
  using OffsetType = std::uint64_t;
  using DataAvailableSignal = boost::signals2::signal<void (const DataChunkSptr&)> ;
  using DataAvailableSlot = DataAvailableSignal::slot_type;

  // -- Support structures and listener of this data --
  /**
   *
   */
  class DataChunk {
   public:
    virtual void *GetData() const = 0;
    virtual ChunkSizeType GetSize() const = 0;
    virtual UniqueId GetUniqueId() const = 0;
    virtual ~DataChunk() = default;
  };
  using DataChunkSptr = std::shared_ptr<DataChunk>;

  class ChunkDataListener {
   public:
    /**
     * @param data
     * @warning do not forget to copy data chunk smart pointer
     */
    virtual void OnDataChunkAvailable(const DataChunkSptr& data) = 0;
  };
  using ChunkDataListenerWptr = std::weak_ptr<ChunkDataListener>;

  // -- Class members --
  InputDataProvider(std::string file_path, ChunkSizeType chunk_size, MessageQueue &message_queue);

  /**
   * @warning not thread-safe
   */
  void SetChunkDataListener(ChunkDataListenerWptr listener);

  /**
   * @warning not thread-safe
   */
  void ClearChunkDataListener();

  void Run();

  void PostJob();


  void ConnectChunkDataListener(const DataAvailableSlot& slot);
 public:

 private:
  DataChunkSptr GenerateNextDataChunk(const UniqueId unique_id, const OffsetType offset);

 private:
  //File reading information
  const std::string file_path_;
  const ChunkSizeType chunk_size_;
  // The next chunk info
  std::atomic<UniqueId> next_chunk_id_ = ATOMIC_VAR_INIT(0u);
  // blocks other process to change the file
  boost::interprocess::file_lock file_lock_;
  // async working queue
  MessageQueue &message_queue_;
  // data consumer
  ChunkDataListenerWptr chunk_data_listener_sptr_;
 private:
  DataAvailableSignal on_data_available_signal_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_INPUTDATALAYER_INPUTDATAPROVIDER_H_
