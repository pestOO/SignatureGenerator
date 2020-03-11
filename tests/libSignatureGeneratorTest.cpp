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
static char const *kInFile = "file.in";
static char const *kOutFile = "file.out";
static char const *kOutFileCheck = "file.out.expected";
static std::size_t kChunkSize = 1024;
static std::size_t kChunkAmount = 1024;

static char const *kInFileBig = "fileBig.in";
static char const *kOutFileBig = "fileBig.out";
static char const *kOutFileCheckBig = "fileBig.out.expected";
static std::size_t kChunkSizeBig = 1024 * 1024;
static std::size_t kChunkAmountBig = 1024 * 10;

// out file for kInFile + chunk size bigger than kInFile
static char const *kOutOneChunkFile = "fileOneHash.out";
static char const *kOutOneChunkFileCheck = "fileOneHash.out.expected";
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

BOOST_AUTO_TEST_CASE(BusyFiles) {
  Library library;
  // TBD
}

BOOST_AUTO_TEST_CASE(OneChunkFile) {
  Library library;
  library.run(kInFile, kOutOneChunkFile, kChunkSize * kChunkAmount);
  BOOST_TEST(CompareFiles(kOutOneChunkFile, kOutOneChunkFileCheck));
}

BOOST_AUTO_TEST_CASE(ChunkBiggerThanFile) {
  Library library;
  library.run(kInFile, kOutOneChunkFile, kChunkSize * kChunkAmount * 2);
  BOOST_TEST(CompareFiles(kOutOneChunkFile, kOutOneChunkFileCheck));
}

BOOST_AUTO_TEST_CASE(ChunkBiggerThanRAM) {
  Library library;
  // TBD
}

BOOST_AUTO_TEST_CASE(RunHappyFlow) {
  Library library;
  library.run(kInFile, kOutFile, kChunkSize);
  BOOST_TEST(CompareFiles(kOutFile, kOutFileCheck));
}

BOOST_AUTO_TEST_CASE(RunLoadTest) {
  const auto start = std::chrono::system_clock::now();
  Library library;
  library.run(kInFileBig, kOutFileBig, kChunkSizeBig);
  BOOST_TEST(CompareFiles(kOutFileBig, kOutFileCheckBig));
  std::cout << "Utility working time: "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
}

// --- Support methods implementation --
bool GenerateTestFilePair(char const *in_file,
                          char const *out_file_check,
                          const std::size_t chunk_size,
                          const std::size_t chunk_amount) {
  const auto chunk = std::vector<char>(chunk_size, 1);

  boost::crc_32_type crc;
  crc.process_bytes(chunk.data(), chunk_size);
  const auto hash = crc.checksum();

  const auto mode = std::ifstream::binary | std::ifstream::out;
  std::fstream input_file(in_file, mode);
  std::fstream output_file(out_file_check, mode);
  if (input_file.fail() || output_file.fail()) {
    throw boost::unit_test::framework::setup_error("Test file creation failed");
  }
  for (int i = 0; i < chunk_amount; ++i) {
    input_file.write(chunk.data(), chunk_size);
    output_file.write((const char *) &hash, sizeof(hash));
  }
  if (input_file.fail() || output_file.fail()) {
    throw boost::unit_test::framework::setup_error("Test file generation failed");
  }
  return true;
}

bool GenerateOneChunkFile(char const *in_file,
                          char const *out_file_check) {

  std::fstream input_file(in_file, std::ifstream::binary | std::ifstream::in);
  input_file.seekg(0, std::ios::end);
  const auto length = input_file.tellg();

  auto chunk = std::vector<char>(length);
  input_file.seekg(0, std::ios::beg);
  input_file.read(chunk.data(), length);

  boost::crc_32_type crc;
  crc.process_bytes(chunk.data(), length);
  const auto hash = crc.checksum();

  std::fstream output_file(out_file_check, std::ifstream::binary | std::ifstream::out);
  output_file.write((const char *) &hash, sizeof(hash));

  if (input_file.fail() || output_file.fail()) {
    throw boost::unit_test::framework::setup_error("One-chunk file generation failed");
  }
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

bool GenerateTestFiles() {
  const auto start = std::chrono::system_clock::now();
  std::cout << "Please, wait for generating test files..." << std::endl;
  const auto result = true;
      GenerateTestFilePair(kInFile, kOutFileCheck, kChunkSize, kChunkAmount)
          && GenerateTestFilePair(kInFileBig, kOutFileCheckBig, kChunkSizeBig, kChunkAmountBig)
          && GenerateOneChunkFile(kInFile, kOutOneChunkFileCheck);

  std::cout << "Test files are generated in "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
  return result;
}
