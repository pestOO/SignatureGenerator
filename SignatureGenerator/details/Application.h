/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#ifndef SIGNATUREGENERATOR_SIGNATUREGENERATOR_DETAILS_APPLICATION_H_
#define SIGNATUREGENERATOR_SIGNATUREGENERATOR_DETAILS_APPLICATION_H_
#pragma once

#include <memory>

//TBD(EZ): remove relative includes by updating cmake
#include "../../libSignatureGenerator/Library.h"

/**
 * @brief The Application class
 * Wraps library to the console UI.
 *
 * Manipulates user input and provides "help" infomration.
 */
class Application {
 private:
  Library library;

 public:
  /**
   * @brief Application default constructor
   */
  Application() = default;

  /**
   * @brief run application
   * @param argc amount of arguments passed from user
   * @param argv - list of arguments
   * @return EXIT_SUCCESS in case of success, otherwise EXIT_FAILURE
   *
   * @note printing help result without data processign in a success result
   */
  int run(int argc, char *argv[]);
};

#endif  //SIGNATUREGENERATOR_SIGNATUREGENERATOR_DETAILS_APPLICATION_H_
