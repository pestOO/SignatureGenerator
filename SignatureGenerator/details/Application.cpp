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
constexpr auto ARG_HELP_NAME = "h,help";

constexpr auto ARG_IN_FULL_NAME = "input,i";
constexpr auto ARG_IN_ACCESS_NAME = "input";

constexpr auto ARG_OUT_FULL_NAME = "outpute,o";
constexpr auto ARG_OUT_ACCESS_NAME = "outpute";

constexpr auto ARG_SIZE_FULL_NAME = "size,s";
constexpr auto ARG_SIZE_ACCESS_NAME = "size";

// --------- Application implementation -------/
int Application::run(int argc, char *argv[]) {
  using boost::program_options::value;

  const auto start = std::chrono::system_clock::now();

  boost::program_options::options_description desc("File signatures generator");
  desc.add_options()
      (ARG_HELP_NAME, "Shows help information")
      (ARG_IN_FULL_NAME, value<std::string>()->required(), "input file path")
      (ARG_OUT_FULL_NAME, value<std::string>()->required(), "outpute file path")
      (ARG_SIZE_FULL_NAME, value<std::uint32_t>()->default_value(1024, "1 MiB)"),
       "file chunks size in KiBytes, default value is 1024 KiBytes (1 MiB)");
  try {
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.find(ARG_HELP_NAME) != std::end(vm)) {
      std::cout << desc << std::endl;
      return EXIT_SUCCESS;
    }
    library.run(
        vm[ARG_IN_ACCESS_NAME].as<std::string>(),
        vm[ARG_OUT_ACCESS_NAME].as<std::string>(),
        vm[ARG_SIZE_ACCESS_NAME].as<std::uint32_t>() * 1024);
  }
  catch (const boost::program_options::error_with_option_name &exception) {
    std::cerr << "Arguments parsing failed : " << exception.what() << "." << std::endl;
    std::cout << desc << std::endl;
    return EXIT_FAILURE;
  }
  catch (const std::exception &exception) {
    std::cerr << "Program is stopped: " << exception.what() << "." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Process finished successfully in "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
  return EXIT_SUCCESS;
}
