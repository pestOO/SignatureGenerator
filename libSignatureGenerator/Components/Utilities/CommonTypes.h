/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_COMMONTYPES_H_
#define SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_COMMONTYPES_H_

#include <cinttypes>
#include <functional>
#include <memory>

// Set of integer aliases
using NumericOrder = std::uint64_t;
using ChunkSize = std::uint64_t;
using Offset = std::uint64_t;

/**
 * Functional Job object are to be called in the thread.
 * @return true if job was done, otherwise false.
 *
 * @note Job shall return false only when job could not be handle NOW, but can be handled later on.
 * @note Job shall throw exception if job could not be handle at all.
 *
 * @warning need to be thread-safe
 */
using Job = std::function<bool()>;
using JobSptr = std::shared_ptr<Job>;

#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_COMMONTYPES_H_
