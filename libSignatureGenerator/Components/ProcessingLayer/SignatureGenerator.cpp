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

#include "details/ChunkSignatureImpl.h"

SignatureGenerator::SignatureGenerator(MessageQueue &message_queue)
    : message_queue_(message_queue) {}

void SignatureGenerator::OnDataAvailable(const InDataChunkSptr &data_chunk_sptr) {
  message_queue_.PostJob(
      [data_chunk_sptr, this]() {
        const auto signatureDataSptr = std::make_shared<ChunkSignatureImpl>(*data_chunk_sptr);
        if (auto listener = on_data_available_listener_.lock())
          listener->OnDataAvailable(signatureDataSptr);
        return true;
      });
}
void SignatureGenerator::SetConnectChunkDataListener(const SignatureGenerator::DataAvailableListenerSptr &listener) {
  on_data_available_listener_ = listener;
}

std::size_t SignatureGenerator::GetSignatureSize() {
  return ChunkSignatureImpl::GetSignatureSize();
}
