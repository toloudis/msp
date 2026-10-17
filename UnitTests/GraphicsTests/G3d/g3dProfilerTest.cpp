/*****************************************************************************
**  g3dProfilerTest.cpp
**
**      g3dProfiler: nested scopes per frame, inclusive draw counts, CPU
**      times, GPU times from a g3dProfilerGpu hook (late, invalid or never
**      resolved), and the CSV it writes.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtTempDir.hpp"

#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	const char* const c_Header = "backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws";

	//	FakeGpu gives each scope 1 + token ms. A frame becomes resolvable
	//	at the first Resolve call after it ended (or never, or invalid).
	class FakeGpu : public g3dProfilerGpu
	{
		public:
			enum Mode { e_Resolves, e_NeverResolves, e_Invalid };
			explicit FakeGpu(Mode i_Mode = e_Resolves) : m_Mode(i_Mode) {}

			void BeginFrame(uint64_t i_FrameId) override { m_Current = i_FrameId; m_Scopes = 0; }
			int  BeginScope() override { return m_Scopes++; }
			void EndScope(int) override {}
			void EndFrame() override { m_Finished.push_back(Frame{m_Current, m_Scopes}); }
			bool Resolve(bool, uint64_t& o_FrameId, std::vector<double>& o_ScopeMs, bool& o_bValid) override
			{
				if (m_Mode == e_NeverResolves || m_Finished.empty())
				{
					return false;
				}
				Frame frame = m_Finished.front();
				m_Finished.erase(m_Finished.begin());
				o_FrameId = frame.m_Id;
				o_ScopeMs.clear();
				for (int i = 0; i < frame.m_Scopes; ++i)
				{
					o_ScopeMs.push_back(1.0 + i);
				}
				o_bValid = (m_Mode != e_Invalid);
				return true;
			}

		private:
			struct Frame { uint64_t m_Id; int m_Scopes; };
			Mode m_Mode;
			uint64_t m_Current = 0;
			int m_Scopes = 0;
			std::vector<Frame> m_Finished;
	};

	struct Row
	{
		std::string backend, frame, sim_time, path, depth, cpu_ms, gpu_ms, draws;
	};

	std::vector<std::string> read_lines(const gtTempDir& i_Dir)
	{
		std::ifstream in((i_Dir.GetPath() / "p.csv").string());
		std::vector<std::string> lines;
		std::string line;
		while (std::getline(in, line))
		{
			lines.push_back(line);
		}
		return lines;
	}

	std::vector<Row> read_rows(const gtTempDir& i_Dir)
	{
		std::vector<std::string> lines = read_lines(i_Dir);
		EXPECT_FALSE(lines.empty());
		if (!lines.empty())
		{
			EXPECT_EQ(c_Header, lines[0]);
		}
		std::vector<Row> rows;
		for (size_t i = 1; i < lines.size(); ++i)
		{
			std::vector<std::string> f;
			std::stringstream ss(lines[i]);
			std::string field;
			while (std::getline(ss, field, ','))
			{
				f.push_back(field);
			}
			if (!lines[i].empty() && lines[i].back() == ',')
			{
				f.push_back("");
			}
			EXPECT_EQ(8u, f.size()) << lines[i];
			if (f.size() == 8)
			{
				rows.push_back(Row{f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]});
			}
		}
		return rows;
	}

	std::string csv_path(const gtTempDir& i_Dir)
	{
		return (i_Dir.GetPath() / "p.csv").string();
	}

	//	One frame: Target { HDR { Lit } }
	void record_nested_frame(float i_SimTime = 0.0f, int i_LitDraws = 0, int i_HdrDraws = 0)
	{
		g3dProfiler::BeginFrame(i_SimTime);
		g3dProfiler::PushScope("Target");
		g3dProfiler::PushScope("HDR");
		g3dProfiler::PushScope("Lit");
		g3dProfiler::AddDraws(i_LitDraws);
		g3dProfiler::PopScope();
		g3dProfiler::AddDraws(i_HdrDraws);
		g3dProfiler::PopScope();
		g3dProfiler::PopScope();
		g3dProfiler::EndFrame();
	}
}

TEST(g3dProfiler, WritesHeader)
{
	gtTempDir dir;
	g3dProfiler::Start(csv_path(dir), "dx11");
	EXPECT_TRUE(g3dProfiler::IsEnabled());
	g3dProfiler::Stop();
	EXPECT_FALSE(g3dProfiler::IsEnabled());

	std::vector<std::string> lines = read_lines(dir);
	ASSERT_EQ(1u, lines.size());
	EXPECT_EQ(c_Header, lines[0]);
}

TEST(g3dProfiler, NestedScopesGivePaths)
{
	gtTempDir dir;
	g3dProfiler::Start(csv_path(dir), "dx11");
	record_nested_frame();
	g3dProfiler::Stop();

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(3u, rows.size());
	EXPECT_EQ("Target", rows[0].path);
	EXPECT_EQ("Target/HDR", rows[1].path);
	EXPECT_EQ("Target/HDR/Lit", rows[2].path);
	EXPECT_EQ("0", rows[0].depth);
	EXPECT_EQ("1", rows[1].depth);
	EXPECT_EQ("2", rows[2].depth);
	EXPECT_EQ("0", rows[2].frame);
}

TEST(g3dProfiler, GpuTimesMatchTokens)
{
	gtTempDir dir;
	FakeGpu gpu;
	g3dProfiler::SetGpu(&gpu);
	g3dProfiler::Start(csv_path(dir), "dx11");
	record_nested_frame();
	g3dProfiler::Stop();
	g3dProfiler::SetGpu(NULL);

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(3u, rows.size());
	EXPECT_EQ("1.0000", rows[0].gpu_ms);
	EXPECT_EQ("2.0000", rows[1].gpu_ms);
	EXPECT_EQ("3.0000", rows[2].gpu_ms);
}

TEST(g3dProfiler, DrawsAreInclusive)
{
	gtTempDir dir;
	g3dProfiler::Start(csv_path(dir), "dx11");
	record_nested_frame(0.0f, 5, 2);
	g3dProfiler::Stop();

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(3u, rows.size());
	EXPECT_EQ("7", rows[0].draws);
	EXPECT_EQ("7", rows[1].draws);
	EXPECT_EQ("5", rows[2].draws);
}

TEST(g3dProfiler, FrameScopeNestsInsideOpenFrame)
{
	gtTempDir dir;
	g3dProfiler::Start(csv_path(dir), "dx11");
	{
		g3dProfileFrameScope outer("Target", 2.0f);
		EXPECT_TRUE(g3dProfiler::IsInFrame());
		{
			g3dProfileFrameScope inner("Target", 2.0f);
			g3dProfileScope pass("Pass");
		}
	}
	EXPECT_FALSE(g3dProfiler::IsInFrame());
	g3dProfiler::Stop();

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(3u, rows.size());
	EXPECT_EQ("Target", rows[0].path);
	EXPECT_EQ("Target/Target", rows[1].path);
	EXPECT_EQ("Target/Target/Pass", rows[2].path);
	EXPECT_EQ("0", rows[0].frame);
	EXPECT_EQ("0", rows[2].frame);
}

TEST(g3dProfiler, NoGpuLeavesGpuEmpty)
{
	gtTempDir dir;
	g3dProfiler::SetGpu(NULL);
	g3dProfiler::Start(csv_path(dir), "dx11");
	record_nested_frame();
	g3dProfiler::Stop();

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(3u, rows.size());
	EXPECT_EQ("", rows[0].gpu_ms);
	EXPECT_GE(std::stod(rows[0].cpu_ms), 0.0);
	EXPECT_NE(std::string::npos, rows[0].cpu_ms.find('.'));
}

TEST(g3dProfiler, InvalidGpuFrameLeavesGpuEmpty)
{
	gtTempDir dir;
	FakeGpu gpu(FakeGpu::e_Invalid);
	g3dProfiler::SetGpu(&gpu);
	g3dProfiler::Start(csv_path(dir), "dx11");
	record_nested_frame();
	g3dProfiler::Stop();
	g3dProfiler::SetGpu(NULL);

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(3u, rows.size());
	EXPECT_EQ("", rows[0].gpu_ms);
	EXPECT_EQ("", rows[2].gpu_ms);
}

TEST(g3dProfiler, FlushWritesUnresolvedFramesWithEmptyGpu)
{
	gtTempDir dir;
	FakeGpu gpu(FakeGpu::e_NeverResolves);
	g3dProfiler::SetGpu(&gpu);
	g3dProfiler::Start(csv_path(dir), "dx11");
	record_nested_frame();
	record_nested_frame();
	record_nested_frame();
	g3dProfiler::Stop();
	g3dProfiler::SetGpu(NULL);

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(9u, rows.size());
	EXPECT_EQ("0", rows[0].frame);
	EXPECT_EQ("1", rows[3].frame);
	EXPECT_EQ("2", rows[6].frame);
	for (const Row& row : rows)
	{
		EXPECT_EQ("", row.gpu_ms);
	}
}

TEST(g3dProfiler, LateGpuResultsKeepFrameOrder)
{
	//	Frames finish before their GPU data arrives; rows still come out
	//	in frame order with the right times.
	gtTempDir dir;
	FakeGpu gpu;
	g3dProfiler::SetGpu(&gpu);
	g3dProfiler::Start(csv_path(dir), "dx11");
	for (int i = 0; i < 5; ++i)
	{
		record_nested_frame(static_cast<float>(i));
	}
	g3dProfiler::Stop();
	g3dProfiler::SetGpu(NULL);

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(15u, rows.size());
	for (int i = 0; i < 5; ++i)
	{
		EXPECT_EQ(std::to_string(i), rows[i * 3].frame);
		EXPECT_EQ("1.0000", rows[i * 3].gpu_ms);
		EXPECT_EQ("3.0000", rows[i * 3 + 2].gpu_ms);
	}
}

TEST(g3dProfiler, DisabledIsNoOp)
{
	EXPECT_FALSE(g3dProfiler::IsEnabled());
	g3dProfiler::BeginFrame(0.0f);
	g3dProfiler::PushScope("Target");
	g3dProfiler::AddDraws(3);
	g3dProfiler::PopScope();
	g3dProfiler::EndFrame();
	EXPECT_FALSE(g3dProfiler::IsInFrame());
	{
		g3dProfileFrameScope frame("Target", 0.0f);
		g3dProfileScope pass("Pass");
		EXPECT_FALSE(g3dProfiler::IsInFrame());
	}
}

TEST(g3dProfiler, SimTimeAndBackendWritten)
{
	gtTempDir dir;
	g3dProfiler::Start(csv_path(dir), "dx11");
	g3dProfiler::BeginFrame(1.5f);
	g3dProfiler::PushScope("Target");
	g3dProfiler::PopScope();
	g3dProfiler::EndFrame();
	g3dProfiler::Stop();

	std::vector<Row> rows = read_rows(dir);
	ASSERT_EQ(1u, rows.size());
	EXPECT_EQ("dx11", rows[0].backend);
	EXPECT_EQ("1.5000", rows[0].sim_time);
}
