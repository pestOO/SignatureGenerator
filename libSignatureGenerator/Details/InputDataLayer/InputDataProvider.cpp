/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "InputDataProvider.h"

#include <fstream>
#include <exception>
#include <memory>
#include <iostream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>

#include "Utilities/MessageQueue.h"

/**
 * Open the file mapping and map it as read-only.
 * Map chunk of the file with read permissions
 */
class DataChunkImpl : public InputDataProvider::DataChunk {
 public:
  DataChunkImpl(const UniqueId unique_id,
                const std::string &file_path,
                const InputDataProvider::OffsetType offset,
                const ChunkSizeType chunk_size)
      : unique_id_(unique_id),
      // TDB(EZ): move to the facade class
        mapped_file(file_path.c_str(),
                    boost::interprocess::read_only),
        region(mapped_file,
               boost::interprocess::read_only,
               offset, chunk_size) {
  }

  void *GetData() const override {
    return region.get_address();
  };

  ChunkSizeType GetSize() const override {
    return region.get_size();
  }

  UniqueId GetUniqueId() const override {
    return unique_id_;
  }
 private:
  const UniqueId unique_id_;
  boost::interprocess::file_mapping mapped_file;
  boost::interprocess::mapped_region region;
};

InputDataProvider::InputDataProvider(std::string file_path,
                                     const ChunkSizeType chunk_size,
                                     MessageQueue &message_queue)
    : message_queue_(message_queue),
      file_path_(std::move(file_path)),
      chunk_size_(chunk_size),
      file_size_(boost::filesystem::file_size(file_path_)),
      file_lock_(file_path_.c_str()) {
  assert(chunk_size_ > 0 && "chunk size could not be empty");

  if (!file_lock_.try_lock()) {
    throw std::runtime_error("Could not obtain exclusive ownership for the file" + file_path_);
  };
}

void InputDataProvider::RunFirstJob() {
  // run the first time to be sure that at least on chunk could be readed
  const auto unique_id = next_chunk_id_.fetch_add(1, std::memory_order_relaxed);
  assert(unique_id == 0 && "The first chunk has 0 id");
  const auto offset = OffsetType(unique_id) * chunk_size_;

  DataChunkSptr the_first_chunk;
  try {
    the_first_chunk = GenerateNextDataChunk(offset, unique_id);
    assert(the_first_chunk && "could not create the first chunk pointer");
    assert(the_first_chunk->GetUniqueId() == 0 && "First chink shall start with 0");
  } catch (const std::exception &exception) {
    throw std::runtime_error(
        std::string("Could load file chunk, maybe chunk size does not fit RAM, exact error is: ") + exception.what());
  }

  assert((!on_data_available_signal_.empty()) && "nobody listens InputDataProvider");
  on_data_available_signal_(the_first_chunk);
}

bool InputDataProvider::PostJob(const unsigned jobs_amount) {
  for (int i = 0; i < jobs_amount; ++i) {
    const auto unique_id = next_chunk_id_.fetch_add(1, std::memory_order_relaxed);
    assert(unique_id > 0 && "Have we overflow the type?");
    const auto offset = OffsetType(unique_id) * chunk_size_;

    if (offset >= GetFileSize()) {
      // we have jobs for all file chunks
      return false;
    }
    const auto nextJob =
        [this, offset, unique_id]() {
          try {
            on_data_available_signal_(GenerateNextDataChunk(offset, unique_id));
          } catch (const std::bad_alloc &exception) {
            std::cout << "Warning:" << exception.what() << std::endl;
            std::cout << "Postpone reading job." << std::endl;;
            return false;
          }
          return true;
        };
    message_queue_.PostJob(std::move(nextJob));
  }
  return true;
}

InputDataProvider::DataChunkSptr InputDataProvider::GenerateNextDataChunk(const UniqueId unique_id,
                                                                          const InputDataProvider::OffsetType offset) {
  return std::make_shared<DataChunkImpl>(unique_id, file_path_, offset, chunk_size_);
}

void InputDataProvider::ConnectChunkDataListener(const InputDataProvider::DataAvailableSlot &slot) {
  on_data_available_signal_.connect(slot);
}
const std::uintmax_t InputDataProvider::GetFileSize() const {
  return file_size_;
}
