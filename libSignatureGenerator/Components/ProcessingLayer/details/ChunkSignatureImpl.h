/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_CHUNKSIGNATUREIMPL_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_CHUNKSIGNATUREIMPL_H_
#pragma once

#include <boost/crc.hpp>

#include "ChunkSignature.h"

using Signature = boost::crc_32_type::value_type;

// TODO we can remove this dependency by passing raw data to this class
#include "InputDataLayer/details/InDataChunk.h"

/**
 * Calculates signatues base on the givven input data chunk
 *
 * @note supports only CRC32 for now
 */
class ChunkSignatureImpl : public ChunkSignature {
 public:
  /**
   * Constructs an object with generated CRC32 checksum of chunk data
   * @param chunk of the data to be used for CRC32 calculation
   */
  explicit ChunkSignatureImpl(const InDataChunk &chunk);

  /**
   * @copydoc ChunkSignature::GetSignatureData()
   */
  const void *GetSignatureData() const override;
  /**
   * @copydoc ChunkSignature::GetSize()
   */
  std::size_t GetSize() const override;
  /**
   * @copydoc ChunkSignature::GetNumericOrder()
   */
  NumericOrder GetNumericOrder() const override;

  /**
   * Static versions of GetSize.
   * Allows to receive signature size without ChunkSignature construction
   * @return
   */
  static std::size_t GetSignatureSize();

 private:
  Signature signature_;
  NumericOrder numeric_order_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_CHUNKSIGNATUREIMPL_H_
