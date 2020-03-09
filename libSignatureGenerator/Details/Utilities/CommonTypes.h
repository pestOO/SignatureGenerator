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

#include <boost/signals2.hpp>

template<class Functor>
using SignalType = typename boost::signals2::signal<Functor>;
/*,boost::signals2::keywords::mutex_type< boost::signals2::dummy_mutex> >::type;*/


// TBD(EZ): rename to numerical order
using UniqueId = std::uint32_t;
using ChunkSizeType = std::uint32_t;

#endif //SIGNATUREGENERATOR_LIBSIGNATUREGENERATOR_DETAILS_UTILITIES_COMMONTYPES_H_
