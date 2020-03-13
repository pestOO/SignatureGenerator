/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "ChunkSignatureImpl.h"

#include <memory>

ChunkSignatureImpl::ChunkSignatureImpl(const InDataChunk &chunk)
    : numeric_order_(chunk.GetUniqueId()) {
  boost::crc_32_type crc;
  crc.process_bytes(chunk.GetData(), chunk.GetSize());
  signature_ = crc.checksum();
}

const void *ChunkSignatureImpl::GetSignatureData() const {
  return std::addressof(signature_);
}

std::size_t ChunkSignatureImpl::GetSize() const {
  return GetSignatureSize();
}

NumericOrder ChunkSignatureImpl::GetNumericOrder() const {
  return numeric_order_;
}
std::size_t ChunkSignatureImpl::GetSignatureSize() {
  return sizeof(signature_);
}
