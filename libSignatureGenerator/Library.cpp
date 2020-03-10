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
  message_queue_ = std::make_shared<MessageQueue>(amount_of_threads);
  input_data_provider_ = std::make_shared<InputDataProvider>(input_file, chunk_size, *message_queue_);
  signature_generator_ = std::make_shared<SignatureGenerator>(*message_queue_);
  //TBD(EZ): move
  const auto in_file_size = input_data_provider_->GetFileSize();
  //TBD(EZ): add getter
  const auto hash_size = sizeof(SignatureGenerator::SignatureType);
  const auto chunks_amount = (in_file_size / chunk_size ) + (in_file_size % chunk_size == 0 ? 0 : 1);
  const auto out_file_size =  chunks_amount * hash_size;
  output_data_consumer_ = std::make_shared<OutputDataConsumer>(out_file, out_file_size, *message_queue_);

  // subscription order: from high level to low

  signature_generator_->ConnectChunkSignatureListener(
      SignatureGenerator::ChunkSignaturSlot(&OutputDataConsumer::WriteData, output_data_consumer_.get(), _1)
          .track_foreign(output_data_consumer_));

  input_data_provider_->ConnectChunkDataListener(InputDataProvider::DataAvailableSlot(
      &SignatureGenerator::GenerateSignature, signature_generator_.get(), _1)
                                                     .track_foreign(signature_generator_));

  message_queue_->ConnectJobsProvider(MessageQueue::ProvideJobsSlot(
      &InputDataProvider::PostJob, input_data_provider_.get(), _1)
                                          .track_foreign(input_data_provider_));

  input_data_provider_->RunFirstJob();
  message_queue_->Execute();
}

