/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "SignatureGenerator.h"

#include <exception>
#include <memory>

#include "Utilities/MessageQueue.h"

class ChunkSignatureImpl : public SignatureGenerator::ChunkSignature {
 public:
  ChunkSignatureImpl(const InputDataProvider::DataChunk &chunk)
      : unique_id_(chunk.GetUniqueId()) {
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

void SignatureGenerator::OnDataAvailable(const InputDataProvider::DataChunkSptr &data_chunk_sptr) {
  message_queue_.PostJob(
      [data_chunk_sptr, this]() {
        if (auto listener = on_data_available_signal_.lock())
          listener->OnDataAvailable(std::make_shared<ChunkSignatureImpl>(*data_chunk_sptr));
        return true;
      });
}
