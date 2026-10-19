/*****************************************************************************
**  gtDx11Child.cpp
**
**      See gtDx11Child.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtDx11Child.hpp"
#include "gtBackend.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <system_error>

namespace
{
	//	The child process's half of gtInDx11: runs i_Function with DX11 up,
	//	writes what it returned to i_File and exits (0 if all went well).
	//	(The child runs the test from the start, so failures before this
	//	one aren't its own.)
	void dx11_child(const gtDx11Function& i_Function, const std::filesystem::path& i_File)
	{
		const int before = gtFailures();
		std::vector<uint8_t> bytes;
		bool ok = gtBackend::Run(gtBackend::e_DX11, [&]() { bytes = i_Function(); }) && gtFailures() == before;
		if (ok)
		{
			std::ofstream out(i_File, std::ios::binary);
			out.write((const char*)bytes.data(), bytes.size());
			ok = out.good();
		}
		if (!ok)
			std::cerr << "the DX11 half failed (its output is above)" << std::endl;
		std::cout.flush();
		std::exit(ok ? 0 : 1);
	}
}

int gtFailures()
{
	const testing::TestResult* result = testing::UnitTest::GetInstance()->current_test_info()->result();
	int n = 0;
	for (int i = 0; i < result->total_part_count(); i++)
		n += result->GetTestPartResult(i).failed() ? 1 : 0;
	return n;
}

std::vector<uint8_t> gtInDx11(const gtDx11Function& i_Function)
{
	//	Named after the test (the child runs the same test, so it finds the
	//	same name), next to the gtTempDir folders but not in one: a
	//	gtTempDir here would share the test's own folder and delete it
	const testing::TestInfo* info = testing::UnitTest::GetInstance()->current_test_info();
	const std::filesystem::path root = std::filesystem::temp_directory_path() / "MspGraphicsTests";
	std::filesystem::create_directories(root);
	const std::filesystem::path file = root / (std::string(info->test_suite_name()) + "." + info->name() + ".dx11.bin");
	std::error_code ec;
	std::filesystem::remove(file, ec);
	EXPECT_EXIT(dx11_child(i_Function, file), ::testing::ExitedWithCode(0), "");
	std::vector<uint8_t> bytes;
	{
		std::ifstream in(file, std::ios::binary);
		bytes.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}
	std::filesystem::remove(file, ec);
	return bytes;
}
