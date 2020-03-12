/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATACHUNK_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATACHUNK_H_

#include "Utilities/CommonTypes.h"

#include <memory>

/**
 * Interface encapsulates input data chunks.
 * Each chunk has data and numeric number, which allows to order chunks.
 */
class InDataChunk {
 public:
  /**
   * @return pointer to the chunk of loaded data.
   */
  virtual void *GetData() const = 0;
  /**
   * @return size of the block of loaded data.
   */
  virtual ChunkSize GetSize() const = 0;
  /**
   * @return number number of th chunk in input data.
   */
  virtual NumericOrder GetUniqueId() const = 0;

  virtual ~InDataChunk() = default;
};

using InDataChunkSptr = std::shared_ptr<InDataChunk>;

#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATACHUNK_H_
