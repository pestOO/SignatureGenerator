/******************************************************************************
 * Copyright (c) 2020-2020 Elisey Zamakhov, eliseyzamahov@gmail.com
 *
 * This file is part of the SignatureGenerator.
 *
 * File can not be copied and/or distributed without the express
 * permission of Elisey Zamakhov.
 *****************************************************************************/

#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <boost/crc.hpp>

#include "libSignatureGenerator/Library.h"

// ------ Constants --------
static std::size_t kChunkSize = 1024 * 1024 * 5;
static std::size_t kChunkAmount = 1024 * 2;
static char const *kInFile = "file.in";
static char const *kOutFile = "hash.out";
static char const *kOutFileCheck = "hash.check";
static char const *kEmptyFile = "empty.file";

// -- Forward Declarations --
bool GenerateTestFiles();
bool CompareFiles(const std::string &p1, const std::string &p2);

// ---- TESTS -----
bool init_unit_test() {
  return GenerateTestFiles();
}

BOOST_AUTO_TEST_CASE(WrongArguments) {
  Library library;
  BOOST_REQUIRE_THROW(library.run(kInFile, kOutFile, 0), std::exception);
  BOOST_REQUIRE_THROW(library.run("", kOutFile, kChunkSize), std::exception);
  BOOST_REQUIRE_THROW(library.run("nonexistent file", kOutFile, kChunkSize), std::exception);
  BOOST_REQUIRE_THROW(library.run(kInFile, "", kChunkSize), std::exception);
}

BOOST_AUTO_TEST_CASE(EmptyInputFile) {
  Library library;
  BOOST_REQUIRE_THROW(library.run(kEmptyFile, kOutFile, 1), std::exception);
  BOOST_REQUIRE_THROW(library.run(kEmptyFile, kOutFile, kChunkSize), std::exception);
}

BOOST_AUTO_TEST_CASE(RunHappyFlow) {
  const auto start = std::chrono::system_clock::now();
  Library library;
  library.run(kInFile, kOutFile, kChunkSize);
  BOOST_TEST(CompareFiles(kOutFile, kOutFileCheck));

  std::cout << "Utility working time: "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
}

// --- Support methods implementation --

bool GenerateTestFiles() {
  const auto start = std::chrono::system_clock::now();
  std::cout << "Please, wait for generating test files..." << std::endl;
  const auto chunk = std::vector<char>(kChunkSize, 1);

  boost::crc_32_type crc;
  crc.process_bytes(chunk.data(), kChunkSize);
  const auto hash = crc.checksum();

  const auto mode = std::ifstream::binary | std::ifstream::out;
  std::fstream input_file(kInFile, mode);
  std::fstream output_file(kOutFileCheck, mode);
  if (input_file.fail() || output_file.fail()) {
    throw boost::unit_test::framework::setup_error("Test file creation failed");
  }
  for (int i = 0; i < kChunkAmount; ++i) {
    input_file.write(chunk.data(), kChunkSize);
    output_file.write((const char *) &hash, sizeof(hash));
  }
  if (input_file.fail() || output_file.fail()) {
    throw boost::unit_test::framework::setup_error("Test file generation failed");
  }
  std::cout << "Test files are generated in "
      << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
      << " seconds." << std::endl;
  return true;
}

bool CompareFiles(const std::string &p1, const std::string &p2) {
  std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);
  if (f1.fail() || f2.fail()) {
    BOOST_TEST_REQUIRE("Comparing files could nto be open");
    return false;
  }
  if (f1.tellg() != f2.tellg()) {
    BOOST_TEST_REQUIRE("Comparing files size mismatch");
    return false;
  }
  //seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}