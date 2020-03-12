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
                                 const std::string &file_path,
                                 const Offset offset,
                                 const ChunkSize chunk_size)
    : numeric_order_(numeric_order),
    // TODO(EZ): move to the facade class, it can save couple of CPU cycles
      mapped_file(file_path.c_str(),
                  boost::interprocess::read_only),
      region(mapped_file,
             boost::interprocess::read_only,
             offset, chunk_size) {
}

void *InDataChunkImpl::GetData() const {
  return region.get_address();
}

ChunkSize InDataChunkImpl::GetSize() const {
  return region.get_size();
}

NumericOrder InDataChunkImpl::GetUniqueId() const {
  return numeric_order_;
}
