/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "SignatureGenerator.h"

#include <fstream>
#include <exception>
#include <memory>
#include <iostream>

#include "Utilities/MessageQueue.h"

class ChunkSignatureImpl : public SignatureGenerator::ChunkSignature {
 public:
  ChunkSignatureImpl(const InputDataProvider::DataChunk &chunk) {
    boost::crc_32_type crc;
    crc.process_bytes(chunk.GetData(), chunk.GetSize());
    signature_ = crc.checksum();
  }
  SignatureGenerator::SignatureType GetSignature() const override {
    return signature_;
  }
  UniqueId GetUniqueId() const override {
    return unique_id_;
  }
  size_t GetSignatureSize() const override {
    return sizeof(signature_);
  }
 private:
  SignatureGenerator::SignatureType signature_;
  UniqueId unique_id_;
};

SignatureGenerator::SignatureGenerator(MessageQueue &message_queue)
    : message_queue_(message_queue) {}

void SignatureGenerator::SetChunkSignatureListener(SignatureGenerator::ChunkSignatureListenerWptr listener) {
  chunk_signature_listener_wptr_.swap(listener);
}
void SignatureGenerator::ClearChunkSignatureListener() {
  chunk_signature_listener_wptr_.reset();
}
void SignatureGenerator::OnDataChunkAvailable(const InputDataProvider::DataChunkSptr &data_chunk_sptr) {
  message_queue_.PostJob(
      [data_chunk_sptr, this]() {
        auto signature = std::make_shared<ChunkSignatureImpl>(*data_chunk_sptr);
        if (auto listener = chunk_signature_listener_wptr_.lock()) {
          listener->OnChunkSignatureAvailable(signature);
        }
        return true;
      });
}

