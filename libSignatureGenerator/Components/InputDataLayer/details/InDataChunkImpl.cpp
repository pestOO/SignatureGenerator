/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "InDataChunkImpl.h"

InDataChunkImpl::InDataChunkImpl(const NumericOrder numeric_order,
                                 boost::interprocess::file_mapping &mapped_file,
                                 const Offset offset,
                                 const ChunkSize chunk_size)
    : numeric_order_(numeric_order),
      mapped_file_(mapped_file),
      mapped_region_(mapped_file_,
                     boost::interprocess::read_only,
                     offset, chunk_size) {
}

void *InDataChunkImpl::GetData() const {
  return mapped_region_.get_address();
}

ChunkSize InDataChunkImpl::GetSize() const {
  return mapped_region_.get_size();
}

NumericOrder InDataChunkImpl::GetUniqueId() const {
  return numeric_order_;
}
