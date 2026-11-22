/*****************************************************************************
**  undoUndoMgrTest.cpp
**
**      Multiple-operation blocks: everything added inside a block, nested
**      blocks included, undoes and redoes as the one outermost step.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif
#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	// counts how often it is undone and redone
	class CountingOperation : public undoUndoOperation
	{
	public:
		CountingOperation(int& io_Undos, int& io_Redos) : m_Undos(io_Undos), m_Redos(io_Redos) {}
		virtual void Undo() { ++m_Undos; }
		virtual void Redo() { ++m_Redos; }

	private:
		int& m_Undos;
		int& m_Redos;
	};

	// leaves the global undo stack empty before and after each test
	class undoUndoMgrTest : public testing::Test
	{
	protected:
		void SetUp() override { undoUndoMgr::Commit(); }
		void TearDown() override { undoUndoMgr::Commit(); }
	};
}

TEST_F(undoUndoMgrTest, BlockIsOneStep)
{
	int undos = 0, redos = 0;
	undoUndoMgr::BeginMultipleOperationBlock("Outer");
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::EndMultipleOperationBlock();

	EXPECT_EQ("Outer", undoUndoMgr::GetUndoOperationName());
	undoUndoMgr::Undo();
	EXPECT_EQ(2, undos);
	EXPECT_FALSE(undoUndoMgr::CanUndo());
}

TEST_F(undoUndoMgrTest, NestedBlockKeepsItsOperations)
{
	int undos = 0, redos = 0;
	undoUndoMgr::BeginMultipleOperationBlock("Outer");
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::BeginMultipleOperationBlock("Inner");
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::EndMultipleOperationBlock();
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::EndMultipleOperationBlock();

	// one step, named after the outer block, holding all four operations
	EXPECT_EQ("Outer", undoUndoMgr::GetUndoOperationName());
	undoUndoMgr::Undo();
	EXPECT_EQ(4, undos);
	EXPECT_FALSE(undoUndoMgr::CanUndo());

	undoUndoMgr::Redo();
	EXPECT_EQ(4, redos);
}

TEST_F(undoUndoMgrTest, EmptyNestedBlockAddsNothing)
{
	int undos = 0, redos = 0;
	undoUndoMgr::BeginMultipleOperationBlock("Outer");
	undoUndoMgr::BeginMultipleOperationBlock("Inner");
	undoUndoMgr::EndMultipleOperationBlock();
	undoUndoMgr::AddOperation(new CountingOperation(undos, redos));
	undoUndoMgr::EndMultipleOperationBlock();

	undoUndoMgr::Undo();
	EXPECT_EQ(1, undos);
	EXPECT_FALSE(undoUndoMgr::CanUndo());
}
