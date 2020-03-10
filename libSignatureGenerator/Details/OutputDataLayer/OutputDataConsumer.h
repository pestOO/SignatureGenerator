/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_OUTPUTDATALAYER_OUTPUTDATACONSUMER_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_OUTPUTDATALAYER_OUTPUTDATACONSUMER_H_
#pragma once

// TODO(EZ): use Pimpl to hide all implementation in other file or move to utilities
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/managed_mapped_file.hpp>

#include "Utilities/CommonTypes.h"
#include "ProcessingLayer/SignatureGenerator.h"

//forward declaration
class MessageQueue;

class OutputDataConsumer : public SignatureGenerator::DataAvailableListener  {
 public:
  OutputDataConsumer(const std::string &file_path,
                     const std::size_t file_size,
                     MessageQueue &message_queue);
  virtual ~OutputDataConsumer();
 private:
  void OnDataAvailable(const SignatureGenerator::ChunkSignatureSptr &sptr) override;
 public:
  void WriteData(const SignatureGenerator::ChunkSignatureSptr &data);

 private:
  void WriteSignatureToFile(const SignatureGenerator::ChunkSignature &data);

  // blocks other process to change the file
  boost::interprocess::file_lock file_lock_;
  // file mapped to memory
  boost::interprocess::managed_mapped_file mapped_file_;
  // async working queue
  MessageQueue &message_queue_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_OUTPUTDATALAYER_OUTPUTDATACONSUMER_H_
