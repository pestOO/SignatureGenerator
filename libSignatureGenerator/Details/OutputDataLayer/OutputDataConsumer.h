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
#include <boost/interprocess/sync/file_lock.hpp>

#include "Utilities/CommonTypes.h"
#include "ProcessingLayer/SignatureGenerator.h"

//forward declaration
class MessageQueue;

class OutputDataConsumer : public SignatureGenerator::ChunkSignatureListener {
 public:
  OutputDataConsumer(const std::string &file_path, MessageQueue &message_queue);
  void OnChunkSignatureAvailable(const SignatureGenerator::ChunkSignatureSptr &data) override;

 private:
  void WriteSignatureToFile(const SignatureGenerator::ChunkSignature &data);

  //File reading information
  const std::string file_path_;
  // blocks other process to change the file
  boost::interprocess::file_lock file_lock_;
  // async working queue
  MessageQueue &message_queue_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_OUTPUTDATALAYER_OUTPUTDATACONSUMER_H_
