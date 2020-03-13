/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "InputDataProvider.h"

#include <exception>
#include <iostream>

#include <boost/filesystem.hpp>

#include "Utilities/MessageQueue.h"
#include "InputDataLayer/details/InDataChunkImpl.h"

InputDataProvider::InputDataProvider(std::string file_path,
                                     const ChunkSize chunk_size,
                                     MessageQueue &message_queue)
    : message_queue_(message_queue),
      file_path_(std::move(file_path)),
      chunk_size_(chunk_size),
      file_size_(boost::filesystem::file_size(file_path_)),
      file_lock_(file_path_.c_str()),
      mapped_file_(file_path_.c_str(),
                  boost::interprocess::read_only){

  assert(chunk_size_ > 0 && "chunk size could not be empty");

  if (!file_lock_.try_lock()) {
    throw std::runtime_error("Could not obtain exclusive ownership for the file" + file_path_);
  };
}

void InputDataProvider::SetConnectChunkDataListener(const InputDataProvider::InDataListenerSptr &listener) {
  on_data_available_listener_ = listener;
}

bool InputDataProvider::RequestChunkRead(const unsigned jobs_amount) {
  for (unsigned i = 0; i < jobs_amount; ++i) {
    const auto numeric_order = next_chunk_id_.fetch_add(1, std::memory_order_relaxed);
    assert(numeric_order >= 0 && "Have we overflow the type?");

    const auto offset = Offset(numeric_order) * chunk_size_;
    if (offset >= GetFileSize()) {
      // no more chunks left
      return false;
    }
    PostMQJob(numeric_order, offset);
  }
  return true;
}

void InputDataProvider::PostMQJob(const NumericOrder numeric_order, const Offset offset) {
  const auto new_job =
      [this, offset, numeric_order]() {
        try {
          if (auto listener = on_data_available_listener_.lock()) {
            listener->OnDataAvailable(GenerateNextDataChunk(numeric_order, offset));
          }
        } catch (const std::bad_alloc &exception) {
          std::cout << "Warning:" << exception.what() << std::endl;
          std::cout << "Postpone reading job." << std::endl;;
          return false;
        }
        return true;
      };
  message_queue_.PostJob(new_job);
}

InDataChunkSptr InputDataProvider::GenerateNextDataChunk(const NumericOrder numeric_order,
                                                         const Offset offset) {
  // We can create the last chunk with a size less than chunk_size_
  const bool is_last_chunk = (offset + chunk_size_) >= GetFileSize();
  const auto real_chunk_size = is_last_chunk ? (GetFileSize() - offset) : chunk_size_;
  return std::make_shared<InDataChunkImpl>(numeric_order, mapped_file_, offset, real_chunk_size);
}

std::uintmax_t InputDataProvider::GetFileSize() const {
  return file_size_;
}
std::uintmax_t InputDataProvider::PredictOutFileSize(const int chunks_output_size) const {
  const auto in_file_size = GetFileSize();
  const auto chunks_amount = (in_file_size / chunk_size_) + (in_file_size % chunk_size_ == 0 ? 0 : 1);
  const auto out_file_size = chunks_amount * chunks_output_size;
  return out_file_size;
}

