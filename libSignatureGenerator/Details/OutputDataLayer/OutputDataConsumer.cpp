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

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "Utilities/MessageQueue.h"

OutputDataConsumer::OutputDataConsumer(const std::string &file_path, MessageQueue &message_queue)
    : file_path_(file_path), message_queue_(message_queue) {
}

void OutputDataConsumer::OnChunkSignatureAvailable(const SignatureGenerator::ChunkSignatureSptr &data) {
  message_queue_.PostJob(
      //copy smart pointer to lambda
      [data, this]() {
        assert(!!data && "received empty signature");
        try {
          WriteSignatureToFile(*data);
        } catch (const std::exception &exception) {
          std::cout << "Warning:" << exception.what() << std::endl;
          std::cout << "Postpone writing job." << std::endl;;
          return false;
        }
        return true;
      });
}

void OutputDataConsumer::WriteSignatureToFile(const SignatureGenerator::ChunkSignature &data) {
  const auto signature = data.GetSignature();
  const auto signature_size = data.GetSignatureSize();
  const auto offset = data.GetUniqueId() * signature_size;

  //TBD(EZ): move to facade
  boost::interprocess::file_mapping mapped_file(file_path_.c_str(), boost::interprocess::read_write);
  boost::interprocess::mapped_region region(mapped_file,
                                            boost::interprocess::read_write,
                                            offset, signature_size);
  assert(region.get_size() == signature_size && "region doe not fit signature");

  std::memcpy(region.get_address(), &signature, signature_size);
  region.flush();
}

