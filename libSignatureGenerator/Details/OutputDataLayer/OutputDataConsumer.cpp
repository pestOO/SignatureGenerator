/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "OutputDataConsumer.h"

#include <iostream>
#include <fstream>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "Utilities/MessageQueue.h"

OutputDataConsumer::OutputDataConsumer(const std::string &file_path,
                                       const std::size_t file_size,
                                       MessageQueue &message_queue)
    : file_path_(file_path),
      message_queue_(message_queue) {
  // Remove old file if exists
  boost::interprocess::file_mapping::remove(file_path_.c_str());
  // Create a new file
  std::fstream fstream;
  fstream.open(file_path, std::ios_base::binary | std::ios_base::out);
  for (int i = 0; i < file_size; ++i) {
    fstream.put(255);
  }
  fstream.close();
  //
  file_lock_ = file_path_.c_str();
  if (!file_lock_.try_lock()) {
    throw std::runtime_error("Could not obtain exclusive ownership for the file" + file_path_);
  };

  mapped_file_ = boost::interprocess::file_mapping(file_path_.c_str(), boost::interprocess::read_write);
//  region_ = boost::interprocess::mapped_region(mapped_file_,
//                                               boost::interprocess::read_write,
//                                               0,
//                                               file_size);
}

OutputDataConsumer::~OutputDataConsumer() {
}

void OutputDataConsumer::WriteData(const SignatureGenerator::ChunkSignatureSptr &data) {
  message_queue_.PostJob(
      //copy smart pointer to lambda
      [data, this]() {
        assert(!!data && "received empty signature pointer");
        WriteSignatureToFile(*data);
        return true;
      });
}

void OutputDataConsumer::WriteSignatureToFile(const SignatureGenerator::ChunkSignature &data) {
  const auto signature = data.GetSignature();
  const auto signature_size = data.GetSignatureSize();
  const auto offset = data.GetUniqueId() * signature_size;

  boost::interprocess::mapped_region region(mapped_file_,
                                            boost::interprocess::read_write,
                                            offset,
                                            signature_size);

  //TBD(EZ): move to facade
  assert(region.get_size() == signature_size && "signature_size does not fit region");

  std::atomic_thread_fence(std::memory_order_relaxed);
  std::memcpy(region.get_address(), &signature, signature_size);
  region.flush();
}
