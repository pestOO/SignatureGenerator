/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_PROCESSINGLAYER_SIGNATUREGENERATOR_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_PROCESSINGLAYER_SIGNATUREGENERATOR_H_
#pragma once

#include <cinttypes>
#include <memory>

#include "Utilities/CommonTypes.h"
#include "details/ChunkSignature.h"
#include "details/DataAvailableListener.h"
#include "InputDataLayer/InputDataProvider.h"

//forward declaration
class MessageQueue;

/**
 * Class abstracts signature generation
 * Facade object for all read data calculation.
 *
 * @implements InDataListener to be able to process data as soon as it available
 */
class SignatureGenerator : public InDataListener {
 public:
  /**
   * Constructs processing data layer component.
   */
  explicit SignatureGenerator(MessageQueue &message_queue);

  using DataAvailableListenerSptr = std::shared_ptr<DataAvailableListener>;
  using DataAvailableListenerWptr = std::weak_ptr<DataAvailableListener>;
  /**
   * Connect one unique listener.
   * Override previous listeners.
   * @warning not thread-safe
   */
  void SetConnectChunkDataListener(const DataAvailableListenerSptr &listener);

  /**
   * @return size of generated signatures
   */
  static std::size_t GetSignatureSize();
 private:
  /**
    * @copydoc InDataListener::OnDataAvailable(InDataChunkSptr);
    */
  void OnDataAvailable(const InDataChunkSptr &sptr) override;

  MessageQueue &message_queue_;
  DataAvailableListenerWptr on_data_available_listener_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_PROCESSINGLAYER_SIGNATUREGENERATOR_H_
