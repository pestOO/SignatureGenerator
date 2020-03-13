/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_CHUNKSIGNATURE_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_CHUNKSIGNATURE_H_

#include <memory>

#include "Utilities/CommonTypes.h"

/**
 * Interface encapsulates data chunks signature generation.
 * Each chunk has data and numeric number, which allows to order chunks.
 */
class ChunkSignature {
 public:
  /**
   * @return pointer to the generated signature of the data chunk
   */
  virtual const void *GetSignatureData() const = 0;
  /**
   * @return size of the block of loaded data.
   * @warning is a constant could not be changed for any chunk
   */
  virtual std::size_t GetSize() const = 0;
  /**
   * @return number number of th chunk in input data.
   */
  virtual NumericOrder GetNumericOrder() const = 0;

  virtual ~ChunkSignature() = default;
};
using ChunkSignatureSptr = std::shared_ptr<ChunkSignature>;

#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_CHUNKSIGNATURE_H_
