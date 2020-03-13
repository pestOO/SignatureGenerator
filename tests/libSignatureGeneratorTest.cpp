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
// Short file 20 KiBytes - for the fast feedback
static std::size_t kChunkSize = 1024;
static std::size_t kChunkAmount = 20;
static char const *kInFile = "file.in";
static char const *kOutFile = "file.out";
// Reference (valid) file for comparison with kOutFile
static auto kOutFileCheck = "file.out.expected";
// Output file, if we consider entire kInFile as one chunk
static char const *kOutOneChunkFile = "fileOneHash.out";
// Reference (valid) file for comparison with kOutOneChunkFile
static char const *kOutOneChunkFileCheck = "fileOneHash.out.expected";

// Long file 20 GiByte - for the performance comparison, IO/RAM check
static std::size_t kChunkSizeBig = 1024 * 1024;
static std::size_t kChunkAmountBig = 1024 * 20;
static char const *kInBigFile = "bigfile.in";
static char const *kOutBigFile = "bigfile.out";
// Reference (valid) file for comparison with kOutBigFile
static char const *kOutFileCheckBig = "bigfile.out.expected";
// Output file, if we consider entire kInBigFile as one chunk
static char const *kOutOneChunkBigFile = "bigfileOneHash.out";
// Reference (valid) file for comparison with kOutOneChunkBigFile
static char const *kOutOneChunkBigFileCheck = "bigfileOneHash.out.expected";

// out file for kInFile + chunk size bigger than kInFile
static char const *kEmptyFile = "empty.file";

// -- Forward Declarations --
bool GenerateTestFiles();
bool CompareFiles(const std::string &p1, const std::string &p2);

// ---- TESTS -----
bool init_unit_test() {
  return GenerateTestFiles();
}

BOOST_AUTO_TEST_CASE(WrongArguments) {
  std::cout << "Running test: WrongArguments" << std::endl;
  Library library;
  BOOST_REQUIRE_THROW(library.run(kInFile, kOutFile, 0), std::exception);
  BOOST_REQUIRE_THROW(library.run("", kOutFile, kChunkSize), std::exception);
  BOOST_REQUIRE_THROW(library.run("nonexistent file path", kOutFile, kChunkSize), std::exception);
  BOOST_REQUIRE_THROW(library.run(kInFile, "", kChunkSize), std::exception);
}

BOOST_AUTO_TEST_CASE(EmptyInputFile) {
  std::cout << "Running test: EmptyInputFile" << std::endl;
  Library library;
  BOOST_REQUIRE_THROW(library.run(kEmptyFile, kOutFile, 1), std::exception);
  BOOST_REQUIRE_THROW(library.run(kEmptyFile, kOutFile, kChunkSize), std::exception);
}

BOOST_AUTO_TEST_CASE(BusyFiles) {
  Library library;
  // TBD
}

BOOST_AUTO_TEST_CASE(RunHappyFlow) {
  std::cout << "Running test: RunHappyFlow" << std::endl;
  Library library;
  library.run(kInFile, kOutFile, kChunkSize);
  BOOST_TEST(CompareFiles(kOutFile, kOutFileCheck));
}

BOOST_AUTO_TEST_CASE(OneChunkFile) {
  std::cout << "Running test: OneChunkFile" << std::endl;
  Library library;
  library.run(kInFile, kOutOneChunkFile, kChunkSize * kChunkAmount);
  BOOST_TEST(CompareFiles(kOutOneChunkFile, kOutOneChunkFileCheck));
}

BOOST_AUTO_TEST_CASE(ChunkBiggerThanFile) {
  std::cout << "Running test: ChunkBiggerThanFile" << std::endl;
  Library library;
  library.run(kInFile, kOutOneChunkFile, kChunkSize * kChunkAmount * 2);
  BOOST_TEST(CompareFiles(kOutOneChunkFile, kOutOneChunkFileCheck));
}

// This test case could be used as performance comparision framework
BOOST_AUTO_TEST_CASE(LoadTest) {
  std::cout << "Running test: RunLoadTest" << std::endl;
  const auto start = std::chrono::system_clock::now();
  Library library;
  library.run(kInBigFile, kOutBigFile, kChunkSizeBig);
  BOOST_TEST(CompareFiles(kOutBigFile, kOutFileCheckBig));
  // The following timing helps to compare performance
  std::cout << "LoadTest working time: "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
}

BOOST_AUTO_TEST_CASE(ChunkBiggerThanRAM) {
  std::cout << "Running test: ChunkBiggerThanRAM" << std::endl;
  const auto start = std::chrono::system_clock::now();
  Library library;
  library.run(kInBigFile, kOutBigFile, kChunkSizeBig * kChunkAmountBig);
  BOOST_TEST(CompareFiles(kOutBigFile, kOutOneChunkBigFileCheck));
  std::cout << "ChunkBiggerThanRAM working time: "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
}

// --- Support methods implementation --
/**
 * Generates several test files
 * @param in_file input utility files
 * @param out_file_check reference output file, used as standard of comparison
 * @param out_one_chunk_file_check reference one-chunk output file, used as standard of comparison
 * @param chunk_size size of chunks for out_file_check
 * @param chunk_amount amount of chunks for out_file_check
 * @return false if generation failed, otherwise true
 */
bool GenerateTestAndReferenceFiles(char const *in_file,
                                   char const *out_file_check,
                                   char const *out_one_chunk_file_check,
                                   const std::size_t chunk_size,
                                   const std::size_t chunk_amount) {
  const auto chunk = std::vector<char>(chunk_size, 0xFF);

  boost::crc_32_type crc32;
  crc32.process_bytes(chunk.data(), chunk_size);
  const auto chunk_checksum = crc32.checksum();

  const auto mode = std::ifstream::binary | std::ifstream::out;
  std::fstream input_file(in_file, mode);
  std::fstream output_check_file(out_file_check, mode);
  std::fstream output_one_check_file(out_one_chunk_file_check, mode);
  if (input_file.fail() || output_check_file.fail() || output_one_check_file.fail()) {
    throw boost::unit_test::framework::setup_error("Test file creation failed");
  }

  for (std::size_t i = 0; i < chunk_amount; ++i) {
    input_file.write(chunk.data(), chunk_size);
    output_check_file.write((const char *) &chunk_checksum, sizeof(chunk_checksum));
  }

  crc32.reset();
  for (std::size_t i = 0; i < chunk_amount; ++i) {
    crc32.process_bytes(chunk.data(), chunk.size());
  }

  const auto entire_file_checksum = crc32.checksum();
  output_one_check_file.write((const char *) &entire_file_checksum, sizeof(entire_file_checksum));

  if (input_file.fail() || output_check_file.fail() || output_one_check_file.fail()) {
    throw boost::unit_test::framework::setup_error("Test file generation failed");
  }
  return true;
}

bool CompareFiles(const std::string &p1, const std::string &p2) {
  std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);
  if (f1.fail() || f2.fail()) {
    BOOST_TEST_REQUIRE("Comparing files could not be open");
    return false;
  }
  if (f1.tellg() != f2.tellg()) {
    BOOST_TEST_REQUIRE("Comparing files size mismatch");
    return false;
  }
  //seek back to beginning and use std::equal to compare content
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}

bool GenerateTestFiles() {
  const auto start = std::chrono::system_clock::now();
  std::cout << "Please, wait for generating test files..." << std::endl;

  // if you want to avoid long file generation, use commented line
//   const auto result = true ||
  const auto result =
      GenerateTestAndReferenceFiles(kInFile,
                                    kOutFileCheck,
                                    kOutOneChunkFileCheck,
                                    kChunkSize,
                                    kChunkAmount)
          && GenerateTestAndReferenceFiles(kInBigFile,
                                           kOutFileCheckBig,
                                           kOutOneChunkBigFileCheck,
                                           kChunkSizeBig,
                                           kChunkAmountBig);
  std::cout << "Test files are generated in "
            << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count()
            << " seconds." << std::endl;
  return result;
}
