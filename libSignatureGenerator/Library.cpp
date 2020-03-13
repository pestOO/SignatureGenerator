/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamahov.
 *****************************************************************************/

#include "Library.h"

#include <functional>

#include "Components/Utilities/MessageQueue.h"
#include "Components/InputDataLayer/InputDataProvider.h"
#include "Components/ProcessingLayer/SignatureGenerator.h"
#include "Components/OutputDataLayer/OutputDataConsumer.h"

Library::~Library() {
  // clear components in the reverse order of subscription and construction
  output_data_consumer_.reset();
  signature_generator_.reset();
  input_data_provider_.reset();
  message_queue_.reset();
}

void Library::run(const std::string &input_file,
                  const std::string &out_file,
                  const std::uint64_t chunk_size) {

  if (chunk_size <= 0) {
    throw std::invalid_argument("Chunk size could no be zero");
  };
  if (input_file.empty()) {
    throw std::invalid_argument("Input file path could not be empty");
  };
  if (out_file.empty()) {
    throw std::invalid_argument("Output file path could not be empty");
  };

  // Creation order: utilities, from bottom layers to top
  message_queue_ = std::make_shared<MessageQueue>();
  input_data_provider_ = std::make_shared<InputDataProvider>(input_file, chunk_size, *message_queue_);
  signature_generator_ = std::make_shared<SignatureGenerator>(*message_queue_);

  const auto out_file_size = input_data_provider_->PredictOutFileSize(SignatureGenerator::GetSignatureSize());
  output_data_consumer_ = std::make_shared<OutputDataConsumer>(out_file, out_file_size);

  // subscription order: from high level to low
  signature_generator_->SetConnectChunkDataListener(output_data_consumer_);
  input_data_provider_->SetConnectChunkDataListener(signature_generator_);
  message_queue_->SetJobsProvider(std::bind(&InputDataProvider::RequestChunkRead,
                                            input_data_provider_.get(),
                                            std::placeholders::_1));
  //TND(EZ): remove redundant first call
  message_queue_->Start();
}

