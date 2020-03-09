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

#include <boost/crc.hpp>

#include "Utilities/CommonTypes.h"
#include "InputDataLayer/InputDataProvider.h"

//forward declaration
class MessageQueue;

/**
 *
 * @todo extract InputDataProvider listener and remove dependecey from it
 */
class SignatureGenerator : public InputDataProvider::ChunkDataListener {
 public:
  // -- Typedef and using --
  using SignatureType = boost::crc_32_type::value_type;

  // -- Support structures and listener of this data --
  /**
   */
  class ChunkSignature {
   public:
    virtual SignatureType GetSignature() const = 0;
    virtual UniqueId GetUniqueId() const = 0;
    virtual std::size_t GetSignatureSize() const = 0;
    virtual ~ChunkSignature() = default;
  };
  using ChunkSignatureSptr = std::shared_ptr<ChunkSignature>;

  class ChunkSignatureListener {
   public:
    /**
     *
     * @param data
     * @warning do not forget to copy chunk signature smart pointer
     */
    virtual void OnChunkSignatureAvailable(const ChunkSignatureSptr &data) = 0;
  };
  using ChunkSignatureListenerWptr = std::weak_ptr<ChunkSignatureListener>;

  // -- Class members --
  explicit SignatureGenerator(MessageQueue &message_queue);
  /**
   * @warning not thread-safe
   */
  void SetChunkSignatureListener(ChunkSignatureListenerWptr listener);

  /**
   * @warning not thread-safe
   */
  void ClearChunkSignatureListener();

 private:
  /**
   * @copydoc InputDataProvider::ChunkDataListener::OnDataChunkAvailable(DataChunkSptr)
   */
  void OnDataChunkAvailable(const InputDataProvider::DataChunkSptr &data_chunk_sptr) override;

 private:
  MessageQueue &message_queue_;
  ChunkSignatureListenerWptr chunk_signature_listener_wptr_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_PROCESSINGLAYER_SIGNATUREGENERATOR_H_
