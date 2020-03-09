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
class SignatureGenerator {
 public:
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
  // -- Typedef and using --
  using ChunkSignatureSptr = std::shared_ptr<ChunkSignature>;
  using ChunkSignaturSignal = SignalType<void(const ChunkSignatureSptr &)>;
  using ChunkSignaturSlot = ChunkSignaturSignal::slot_type;

  // -- Class members --
  explicit SignatureGenerator(MessageQueue &message_queue);

  /**
   * ChunkSignature
   */
  void GenerateSignature(const InputDataProvider::DataChunkSptr &data_chunk_sptr);

  /**
   * Connects
   * @warning not thread-safe
   */
  void ConnectChunkSignatureListener(const ChunkSignaturSlot &slot);
 private:
  MessageQueue &message_queue_;
  ChunkSignaturSignal on_data_available_signal_;
};

#endif  //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_PROCESSINGLAYER_SIGNATUREGENERATOR_H_
