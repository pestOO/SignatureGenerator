/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#include "Application.h"


#include <iostream>
#include <memory>
#include <string>

#include <boost/program_options.hpp>

// --------- Constants -------/

constexpr auto ARG_HELP_NAME = "help";

constexpr auto ARG_IN_FULL_NAME = "input,i";
constexpr auto ARG_IN_ACCESS_NAME = "input";

constexpr auto ARG_OUT_FULL_NAME = "outpute,o";
constexpr auto ARG_OUT_ACCESS_NAME = "outpute";

constexpr auto ARG_SIZE_FULL_NAME = "size,s";
constexpr auto ARG_SIZE_ACCESS_NAME = "size";

// --------- Application implementation -------/

int Application::run(int argc, char *argv[]) {
  using boost::program_options::value;
  boost::program_options::options_description desc("Hash-signatures generator");
  // TODO(EZamakhov): change default LLVM clang style format
  // clang-format off
  desc.add_options()
      (ARG_HELP_NAME,                                                 "Shows help infomration")
      (ARG_IN_FULL_NAME,    value<std::string>()->required(),         "input file path")
      (ARG_OUT_FULL_NAME,   value<std::string>()->required(),         "outpute file path")
      // TODO(EZmamakhov): add parsing MiB, KiB and so on
      (ARG_SIZE_FULL_NAME,  value<std::uint32_t>()->default_value(1024 * 1024), "file chunks size in bytes, default value is 1 MiB");
  // clang-format on
  try {
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.find(ARG_HELP_NAME) != std::end(vm)) {
      return EXIT_SUCCESS;
    }

    library.run(vm[ARG_IN_ACCESS_NAME].as<std::string>(), vm[ARG_OUT_ACCESS_NAME].as<std::string>(),
                vm[ARG_SIZE_ACCESS_NAME].as<std::uint32_t>());

  } catch (const std::exception &exception) {
    std::cerr << "Program stoped due to \"" << exception.what() << "\"" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Process finished successfully." << std::endl;
  return EXIT_SUCCESS;
}
