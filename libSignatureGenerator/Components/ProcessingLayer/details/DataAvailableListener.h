/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_DATAAVAILABLELISTENER_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_DATAAVAILABLELISTENER_H_

#include "ChunkSignature.h"

/**
 * DataAvailableListener allows to subscribe on the generated signature date
 */
class DataAvailableListener {
 public:
  /**
   * Callback with a new generated signature available
   * @note thread-safe
   */
  virtual void OnDataAvailable(const ChunkSignatureSptr &) = 0;

  virtual ~DataAvailableListener() = default;
};

#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_PROCESSINGLAYER_DETAILS_DATAAVAILABLELISTENER_H_
