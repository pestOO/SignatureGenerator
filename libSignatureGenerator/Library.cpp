/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamahov.
 *****************************************************************************/

#include "Library.h"

#include "Details/Utilities/MessageQueue.h"
#include "Details/InputDataLayer/InputDataProvider.h"
#include "Details/ProcessingLayer/SignatureGenerator.h"
#include "Details/OutputDataLayer/OutputDataConsumer.h"

Library::Library() {
}

Library::~Library() {
  // clear components in the reverse order of subscription and construction
  input_data_provider_->ClearChunkDataListener();
  signature_generator_->ClearChunkSignatureListener();

  output_data_consumer_.reset();
  signature_generator_.reset();
  input_data_provider_.reset();
  message_queue_.reset();
}

void Library::run(const std::string &input_file,
                  const std::string &out_file,
                  const std::uint32_t chunk_size) {
  const auto recommended_amount_of_threads = MessageQueue::RecommendedAmountOfThreads();
  const auto amount_of_threads = recommended_amount_of_threads > 3 ? recommended_amount_of_threads : 3;

  // Creation order: utilities, from bottom layers to top
  message_queue_ = std::make_shared<MessageQueue>(amount_of_threads);
  input_data_provider_ = std::make_shared<InputDataProvider>(input_file, chunk_size, *message_queue_);
  signature_generator_ = std::make_shared<SignatureGenerator>(*message_queue_);
  output_data_consumer_ = std::make_shared<OutputDataConsumer>(out_file, *message_queue_);

  // subscription order: from high level to low
  signature_generator_->SetChunkSignatureListener(output_data_consumer_);
  input_data_provider_->SetChunkDataListener(signature_generator_);
}

