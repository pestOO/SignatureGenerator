/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "OutputDataConsumer.h"

#include "Utilities/MessageQueue.h"

OutputDataConsumer::OutputDataConsumer(const std::string &file_path,
                                       const std::size_t file_size)
    : file_path_(file_path) {
  boost::iostreams::mapped_file_params params;
  params.path = file_path;
  params.flags = boost::iostreams::mapped_file_base::readwrite;
  params.offset = 0;
  // output file is considerably small and we can open it entirely
  params.length = file_size;
  params.new_file_size = file_size;
  mapped_file_.open(params);

  if (!mapped_file_.is_open()) {
    throw std::runtime_error("Could not create output file " + file_path);
  }
  // Lock the file
  file_lock_ = file_path.c_str();
  if (!file_lock_.try_lock()) {
    throw std::runtime_error("Could not obtain exclusive ownership for the file " + file_path);
  };
}

void OutputDataConsumer::OnDataAvailable(const ChunkSignatureSptr &signature_sptr) {
  // We do not post job in a queue, but do it in-place, because writing several bytes to memory is very fast
  assert(!!signature_sptr && "received empty signature pointer");
  assert(signature_sptr->GetSize() < 100 &&
      "signature size has grown, please reconsider loading output file partially");
  WriteSignatureToFile(*signature_sptr);
}

void OutputDataConsumer::WriteSignatureToFile(const ChunkSignature &data) {
  const auto offset = data.GetNumericOrder() * data.GetSize();
  assert(mapped_file_.size() >= (data.GetSize() + offset) && "signature_size does not fit mapped region");
  std::memcpy(mapped_file_.data() + offset, data.GetSignatureData(), data.GetSize());
}


