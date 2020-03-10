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
    : message_queue_(message_queue) {
  using namespace boost::interprocess;
  // Remove old file if exists
  file_mapping::remove(file_path.c_str());
  // Create new file and map
  mapped_file_ = managed_mapped_file(open_or_create, file_path.c_str(), file_size);
  // Lock the file
  file_lock_ = file_path.c_str();
  if (!file_lock_.try_lock()) {
    throw std::runtime_error("Could not obtain exclusive ownership for the file" + file_path);
  };
}

OutputDataConsumer::~OutputDataConsumer() {}

void OutputDataConsumer::OnDataAvailable(const SignatureGenerator::ChunkSignatureSptr &signature_sptr) {
  // We do nto post job in a queue, but do it in-place, because writing several bytes to memory is very fast
  assert(!!signature_sptr && "received empty signature pointer");
  WriteSignatureToFile(*signature_sptr);
}

void OutputDataConsumer::WriteSignatureToFile(const SignatureGenerator::ChunkSignature &data) {
  const auto signature = data.GetSignature();
  const auto signature_size = data.GetSignatureSize();
  const auto offset = data.GetUniqueId() * signature_size;

  assert(mapped_file_.get_size() >= signature_size && "signature_size does not fit region");

  std::atomic_thread_fence(std::memory_order_relaxed);
  auto *memory_address = (char *) mapped_file_.get_address() + offset;
  std::memcpy(memory_address, &signature, signature_size);
}


