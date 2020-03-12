/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATALISTENER_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATALISTENER_H_

#include "InDataChunk.h"

/**
 * InDataListener allows to subscribe on input date
 */
class InDataListener {
 public:
  /**
   * Callback with a new input data chunk
   * @note thread-safe
   */
  virtual void OnDataAvailable(const InDataChunkSptr &) = 0;

  virtual ~InDataListener() = default;
};
#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATALISTENER_H_
