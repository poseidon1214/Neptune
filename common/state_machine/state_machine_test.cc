// Copyright (c) 2015 Tencent Inc.
// Author: Xie Yu (yuxie@tencent.com)

#include "common/state_machine/state_machine.h"

#include <utility>
#include <vector>

#include "common/base/callback.h"
#include "common/state_machine/base_task.h"
#include "common/system/concurrency/event.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gmock/gmock.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {
namespace common {
namespace {

class MockBaseTask : public BaseTask {
 public:
  MockBaseTask() : BaseTask("mock-task") { }
  using BaseTask::SetFinalStatus;
  MOCK_METHOD0(GetFinalStatus, TaskFinalStatus());
  MOCK_METHOD1(DoStart, void(Closure* done));
  MOCK_METHOD0(BeforeFinish, void());
  MOCK_METHOD0(BeforeExit, void());
};

ACTION(RunClosure) {
  arg0->Run();
}

class SyncClosure {
 public:
  SyncClosure() {
    closure_ = NewCallback(this, &SyncClosure::Done);
  }

  Closure* GetClosure() {
    return closure_;
  }

  void Done() {
    event_.Set();
  }

  void Wait() {
    event_.Wait();
  }

 private:
  ManualResetEvent event_;
  Closure* closure_;
};

class MockTaskCreator {
 public:
  // count means how many times the creator will be called.
  explicit MockTaskCreator(int count)
      : count_(count),
        already_called_(0) {
    mock_tasks_.resize(count);
    for (int i = 0; i < count; i++) {
      MockBaseTask* mock_task = new MockBaseTask();
      mock_tasks_[i] = mock_task;
      EXPECT_CALL(*mock_task, DoStart(::testing::NotNull()))
          .WillOnce(RunClosure());
      EXPECT_CALL(*mock_task, BeforeFinish()).Times(1);
      EXPECT_CALL(*mock_task, BeforeExit()).Times(1);
    }
    creator_ = NewPermanentCallback(this, &MockTaskCreator::Create);
  }

  ~MockTaskCreator() {
    CHECK_EQ(count_, already_called_)
        << "The TaskCreator was expected to be called " << count_
        << " times, but actually called " << already_called_ << " times.";
    for (int i = already_called_; i < count_; ++i) {
      delete mock_tasks_[i];
    }
  }

  StateMachine::TaskCreator* GetTaskCreator() {
    return creator_;
  }

  BaseTask* Create() {
    CHECK_LT(already_called_, count_)
        << "The TaskCreator was expected to be called " << count_
        << " times, but you are calling more than that.";
    return mock_tasks_[already_called_++];
  }

  // Be careful that the returned task maybe invalid after StateMachine::Start
  MockBaseTask* MockTaskAt(int i) {
    return mock_tasks_[i];
  }

  int count_;
  int already_called_;
  StateMachine::TaskCreator* creator_;
  std::vector<MockBaseTask*> mock_tasks_;
};


class StatesMachineTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  }

  virtual void TearDown() {
  }
};

BaseTask* InvalidTaskCreator() {
  return NULL;
}

void DoNothing() {
}

TEST_F(StatesMachineTest, AddState) {
  StateMachine state_machine;

  // Cannot add kStateEnd
  MockTaskCreator mock_creator(0);
  StateMachine::TaskCreator* creator = mock_creator.GetTaskCreator();
  EXPECT_DEATH(state_machine.AddState(StateMachine::kStateEnd, creator), ".*");
  delete creator;

  // NULL creator
  EXPECT_DEATH(state_machine.AddState(1, NULL), ".*");

  // Is not PermanentCallback
  StateMachine::TaskCreator* creator2 = NewCallback(&InvalidTaskCreator);
  EXPECT_DEATH(state_machine.AddState(1, creator2), ".*");
  delete creator2;

  // Success
  MockTaskCreator mock_creator_1(0);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
}

TEST_F(StatesMachineTest, AddTransition) {
  StateMachine state_machine;

  // State not added.
  EXPECT_DEATH(state_machine.AddDefaultTransition(1, 2), ".*");
  EXPECT_DEATH(state_machine.AddTransition(1, BaseTask::kSuccess, 2), ".*");

  MockTaskCreator mock_creator_1(0);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  // To invalid
  EXPECT_DEATH(state_machine.AddDefaultTransition(1, 2), ".*");
  EXPECT_DEATH(state_machine.AddTransition(1, BaseTask::kSuccess, 2), ".*");
  // From invalid
  EXPECT_DEATH(state_machine.AddDefaultTransition(2, 1), ".*");
  EXPECT_DEATH(state_machine.AddTransition(2, BaseTask::kSuccess, 1), ".*");
  // From is kStateEnd
  EXPECT_DEATH(
      state_machine.AddDefaultTransition(StateMachine::kStateEnd, 1), ".*");
  EXPECT_DEATH(state_machine.AddTransition(
      StateMachine::kStateEnd, BaseTask::kSuccess, 1), ".*");

  // Success
  MockTaskCreator mock_creator_2(0);
  state_machine.AddState(2, mock_creator_2.GetTaskCreator());
  state_machine.AddDefaultTransition(1, 2);
  state_machine.AddTransition(1, BaseTask::kSuccess, 2);
}

TEST_F(StatesMachineTest, EmptyStateMachine) {
  StateMachine state_machine;

  EXPECT_DEATH(state_machine.Start(1, NULL), ".*");

  Closure* done = NewCallback(&DoNothing);
  EXPECT_DEATH(state_machine.Start(1, done), ".*");
  delete done;
}

TEST_F(StatesMachineTest, OneState) {
  StateMachine state_machine;

  MockTaskCreator mock_creator_1(1);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  state_machine.AddTransition(1, BaseTask::kSuccess, StateMachine::kStateEnd);

  EXPECT_CALL(*(mock_creator_1.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kSuccess));

  SyncClosure sync_closure;
  state_machine.Start(1, sync_closure.GetClosure());
  sync_closure.Wait();

  std::vector<std::pair<int, int> > results =
      state_machine.RanStatesAndResults();
  CHECK_EQ(2, results.size());
  EXPECT_EQ(1, results[0].first);
  EXPECT_EQ(BaseTask::kSuccess, results[0].second);
  EXPECT_EQ(StateMachine::kStateEnd, results[1].first);

  // AddState after Start
  MockTaskCreator mock_creator(0);
  StateMachine::TaskCreator* creator = mock_creator.GetTaskCreator();
  EXPECT_DEATH(state_machine.AddState(3, creator), ".*");
  delete creator;
  {
    // Already started, cannot start again.
    SyncClosure sync_closure;
    EXPECT_DEATH(state_machine.Start(1, sync_closure.GetClosure()), ".*");
    delete sync_closure.GetClosure();
  }
}

TEST_F(StatesMachineTest, InvalidState) {
  StateMachine state_machine;

  MockTaskCreator mock_creator_1(0);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  MockTaskCreator mock_creator_2(0);
  state_machine.AddState(2, mock_creator_2.GetTaskCreator());
  state_machine.AddTransition(1, BaseTask::kSuccess, 2);

  SyncClosure sync_closure;
  EXPECT_DEATH(state_machine.Start(1, sync_closure.GetClosure()), ".*");
  delete sync_closure.GetClosure();
}

TEST_F(StatesMachineTest, LoopState) {
  StateMachine state_machine;

  MockTaskCreator mock_creator_1(0);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  MockTaskCreator mock_creator_2(0);
  state_machine.AddState(2, mock_creator_2.GetTaskCreator());
  state_machine.AddTransition(1, BaseTask::kSuccess, 2);
  state_machine.AddTransition(2, BaseTask::kSuccess, 1);

  SyncClosure sync_closure;
  EXPECT_DEATH(state_machine.Start(1, sync_closure.GetClosure()), ".*");
  delete sync_closure.GetClosure();
}

void CheckRanResult(int state, int result, const std::pair<int, int>& actual) {
  EXPECT_EQ(std::make_pair(state, result), actual);
}

TEST_F(StatesMachineTest, SuccTwoState) {
  StateMachine state_machine;

  MockTaskCreator mock_creator_1(1);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  MockTaskCreator mock_creator_2(1);
  state_machine.AddState(2, mock_creator_2.GetTaskCreator());

  state_machine.AddDefaultTransition(1, 2);
  state_machine.AddDefaultTransition(2, StateMachine::kStateEnd);

  EXPECT_CALL(*(mock_creator_1.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kSuccess));
  EXPECT_CALL(*(mock_creator_2.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kSuccess));

  SyncClosure sync_closure;
  state_machine.Start(1, sync_closure.GetClosure());
  sync_closure.Wait();

  std::vector<std::pair<int, int> > results =
      state_machine.RanStatesAndResults();
  CHECK_EQ(3, results.size());
  CheckRanResult(1, BaseTask::kSuccess, results[0]);
  CheckRanResult(2, BaseTask::kSuccess, results[1]);
  EXPECT_EQ(StateMachine::kStateEnd, results[2].first);
}


TEST_F(StatesMachineTest, SuccLoopState) {
  StateMachine state_machine;

  MockTaskCreator mock_creator_1(2);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  MockTaskCreator mock_creator_2(2);
  state_machine.AddState(2, mock_creator_2.GetTaskCreator());

  state_machine.AddDefaultTransition(1, 2);
  state_machine.AddTransition(2, BaseTask::kFailure, 1);
  state_machine.AddDefaultTransition(2, StateMachine::kStateEnd);

  EXPECT_CALL(*(mock_creator_1.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kSuccess));
  EXPECT_CALL(*(mock_creator_2.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kFailure));
  EXPECT_CALL(*(mock_creator_1.MockTaskAt(1)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kFailure));
  EXPECT_CALL(*(mock_creator_2.MockTaskAt(1)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kSuccess));

  SyncClosure sync_closure;
  state_machine.Start(1, sync_closure.GetClosure());
  sync_closure.Wait();

  std::vector<std::pair<int, int> > results =
      state_machine.RanStatesAndResults();
  CHECK_EQ(5, results.size());
  CheckRanResult(1, BaseTask::kSuccess, results[0]);
  CheckRanResult(2, BaseTask::kFailure, results[1]);
  CheckRanResult(1, BaseTask::kFailure, results[2]);
  CheckRanResult(2, BaseTask::kSuccess, results[3]);
  EXPECT_EQ(StateMachine::kStateEnd, results[4].first);
}

TEST_F(StatesMachineTest, NoTransitionFound) {
  StateMachine state_machine;

  MockTaskCreator mock_creator_1(1);
  state_machine.AddState(1, mock_creator_1.GetTaskCreator());
  MockTaskCreator mock_creator_2(1);
  state_machine.AddState(2, mock_creator_2.GetTaskCreator());

  state_machine.AddTransition(1, BaseTask::kFailure, 2);
  state_machine.AddDefaultTransition(1, StateMachine::kStateEnd);

  EXPECT_CALL(*(mock_creator_1.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kFailure));
  EXPECT_CALL(*(mock_creator_2.MockTaskAt(0)), GetFinalStatus())
      .WillOnce(::testing::Return(BaseTask::kSuccess));

  SyncClosure sync_closure;
  state_machine.Start(1, sync_closure.GetClosure());
  sync_closure.Wait();

  std::vector<std::pair<int, int> > results =
      state_machine.RanStatesAndResults();
  CHECK_EQ(2, results.size());
  CheckRanResult(1, BaseTask::kFailure, results[0]);
  CheckRanResult(2, BaseTask::kSuccess, results[1]);
}

BaseTask* NullTask() {
  return NULL;
}

TEST_F(StatesMachineTest, EmptyState) {
  StateMachine state_machine;

  state_machine.AddState(1, NewPermanentCallback(&NullTask));
  state_machine.AddDefaultTransition(1, StateMachine::kStateEnd);

  SyncClosure sync_closure;
  state_machine.Start(1, sync_closure.GetClosure());
  sync_closure.Wait();

  std::vector<std::pair<int, int> > results =
      state_machine.RanStatesAndResults();
  CHECK_EQ(2, results.size());
  EXPECT_EQ(1, results[0].first);
  EXPECT_EQ(BaseTask::kSkiped, results[0].second);
  EXPECT_EQ(StateMachine::kStateEnd, results[1].first);
}

}  // namespace
}  // namespace common
}  // namespace gdt
