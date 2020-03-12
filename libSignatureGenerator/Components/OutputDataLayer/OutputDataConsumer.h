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

#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include "Utilities/CommonTypes.h"
#include "ProcessingLayer/SignatureGenerator.h"

//forward declaration
class MessageQueue;

class OutputDataConsumer : public SignatureGenerator::DataAvailableListener {
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
//  using MappedFile1 = boost::interprocess::managed_mapped_file;
//  using MappedFile = boost::interprocess::basic_managed_mapped_file<char,
//                                                                    boost::interprocess::simple_seq_fit<
//                                                                        boost::interprocess::null_mutex_family>,
//                                                                    boost::interprocess::null_index>;
  using MappedFile = boost::iostreams::mapped_file_sink;
  const std::string file_path_;
  MappedFile mapped_file_;
  // async working queue
  MessageQueue &message_queue_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_OUTPUTDATALAYER_OUTPUTDATACONSUMER_H_
