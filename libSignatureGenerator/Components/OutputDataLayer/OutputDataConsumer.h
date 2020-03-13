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

/**
 * Class abstracts output data caching and validation.
 * Facade object for all output data accessing.
 *
 * @implements DataAvailableListener to be able to process data as soon as it available
 */
class OutputDataConsumer : public DataAvailableListener {
 public:
  /**
   * Constructs output data layer component.
   * @param file_path of the file to be written
   * @param file_size size of the file to be written
   *
   * @warning file_size is mandatory due to loading entire feel in a memory
   */
  OutputDataConsumer(const std::string &file_path,
                     const std::size_t file_size);
 private:
  /**
  * @copydoc InDataListener::OnDataAvailable(InDataChunkSptr);
  */
  void OnDataAvailable(const ChunkSignatureSptr &sptr) override;

  void WriteSignatureToFile(const ChunkSignature &data);

  // blocks other process to change the file
  boost::interprocess::file_lock file_lock_;
  // file mapped to memory
  using MappedFile = boost::iostreams::mapped_file_sink;
  const std::string file_path_;
  MappedFile mapped_file_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_OUTPUTDATALAYER_OUTPUTDATACONSUMER_H_
