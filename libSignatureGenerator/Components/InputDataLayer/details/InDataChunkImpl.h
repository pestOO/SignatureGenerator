/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATACHUNKIMPL_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATACHUNKIMPL_H_

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>

#include "Utilities/CommonTypes.h"
#include "InDataChunk.h"

/**
 * Open the file mapping and map it as read-only.
 * Map chunk of the file with read permissions
 */
class InDataChunkImpl : public InDataChunk {
 public:
  /**
   * Constructs new data chunk and pre-load all data from the disk to the memory.
   * @param numeric_order chunk order in the input file
   * @param file_path path tot he file for readign chunk
   * @param offset of the cunk in the file
   * @param chunk_size to be read in memeory
   */
  InDataChunkImpl(const NumericOrder numeric_order,
                  const std::string &file_path,
                  const Offset offset,
                  const ChunkSize chunk_size);

  /**
   * @copydoc InDataChunk::GetData()
   */
  void *GetData() const override;;

  /**
   * @copydoc InDataChunk::GetSize()
   */
  ChunkSize GetSize() const override;

  /**
   * @copydoc InDataChunk::GetUniqueId()
   */
  NumericOrder GetUniqueId() const override;

 private:
  const NumericOrder numeric_order_;
  boost::interprocess::file_mapping mapped_file;
  boost::interprocess::mapped_region region;
};

#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_COMPONENTS_INPUTDATALAYER_INDATACHUNKIMPL_H_
