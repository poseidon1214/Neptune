// Copyright    (C), 1998-2012, Tencent
// Author: jefftang@tencent.com
// Date: 2012-11-22

#include "common/base/callback.h"

#include <string>
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace gdt {

class CallbackTest : public testing::Test {
};

struct Response {
  int     key_;
  std::string  value_;
};

class Server {
 public:
  void StartLookup(Callback<void(Response)>* cb, int key) {
    Response r = { key, std::string() };  // Simply send back the key.
    if (cb) cb->Run(r);       // let the client run.
  }
  void Foo(const std::string& s) {
      Closure* c = NewCallback(this, &Server::Foo, s);
      delete c;
  }
};

class Client {
 public:
  explicit Client(Server* s) : key_(0), server_(s) { }

  void Lookup() {
    LOG(INFO) << "Lookup with " << key_;
    server_->StartLookup(NewCallback(this, &Client::LookupDone), key_);
  }

  void NullLookup() {
    server_->StartLookup(NULL, key_);
  }

  // callback function by the server when lookup is done.
  void LookupDone(Response resp) {
    CHECK_EQ(key_, resp.key_);
    LOG(INFO) << "Lookup done with " << key_;
    key_++;
    if (key_ > 3)
      return;
    Lookup();
  }

 private:
  int     key_;
  Server* server_;
};

TEST_F(CallbackTest, TestCallback) {
  LOG(INFO) << "==== TestCallback ====";
  Server s;
  Client c(&s);
  c.Lookup();

  LOG(INFO) << "Doing NULL callback test ";
  c.NullLookup();  // make sure NULL callback won't crash the system
  LOG(INFO) << "Done callback test ";
}

/***** For testing compilation *****/

// Distinct types to check overloading
struct A1 { } a1;
struct A2 { } a2;
struct P1 { } p1;
struct P2 { } p2;
typedef int R1;
static R1 r1 = 7;

static void Do(Closure* cb) {
  cb->Run();
}

static void Do(Callback<void(A1)>* cb) {
  cb->Run(a1);
}

static void Do(Callback<void(A1, A2)>* cb) {
  cb->Run(a1, a2);
}

static void Do(Callback<R1()>* cb) {
  CHECK_EQ(r1, cb->Run());
}

static void Do(Callback<R1(A1)>* cb) {
  CHECK_EQ(r1, cb->Run(a1));
}

static void Do(Callback<R1(A1, A2)>* cb) {
  CHECK_EQ(r1, cb->Run(a1, a2));
}

// Member callbacks
class CT {
 public:
  CT() : intrusive_count_(0) {
  }
  void CB_0_0()                           { LOG(INFO) << "CB_0_0"; }
  void CB_1_0(P1 p1)                      { LOG(INFO) << "CB_1_0"; }
  void CB_2_0(P1 p1, P2 p2)               { LOG(INFO) << "CB_2_0"; }

  void CB_0_1(A1 a1)                      { LOG(INFO) << "CB_0_1"; }
  void CB_1_1(P1 p1, A1 a1)               { LOG(INFO) << "CB_1_1"; }
  void CB_2_1(P1 p1, P2 p2, A1 a1)        { LOG(INFO) << "CB_2_1"; }

  void CB_0_2(A1 a1, A2 a2)               { LOG(INFO) << "CB_0_2"; }
  void CB_1_2(P1 p1, A1 a1, A2 a2)        { LOG(INFO) << "CB_1_2"; }
  void CB_2_2(P1 p1, P2 p2, A1 a1, A2 a2) { LOG(INFO) << "CB_2_2"; }

  void CCB_0_0() const                    { LOG(INFO) << "CCB_0_0"; }
  void CCB_1_0(P1 p1) const               { LOG(INFO) << "CCB_1_0"; }
  void CCB_2_0(P1 p1, P2 p2) const        { LOG(INFO) << "CCB_2_0"; }

  void CCB_0_1(A1 a1)  const              { LOG(INFO) << "CCB_0_1"; }
  void CCB_1_1(P1 p1, A1 a1) const         { LOG(INFO) << "CCB_1_1"; }
  void CCB_2_1(P1 p1, P2 p2, A1 a1) const  { LOG(INFO) << "CCB_2_1"; }

  void CCB_0_2(A1 a1, A2 a2)  const        { LOG(INFO) << "CCB_0_2"; }
  void CCB_1_2(P1 p1, A1 a1, A2 a2) const  { LOG(INFO) << "CCB_1_2"; }
  void CCB_2_2(P1 p1, P2 p2, A1 a1, A2 a2) const { LOG(INFO) << "CCB_2_2"; }

  R1   RCB_0_0() {
    LOG(INFO) << "RCB_0_0";
    return r1;
  }

  R1   RCB_1_0(P1 p1) {
    LOG(INFO) << "RCB_1_0";
    return r1;
  }

  R1   RCB_2_0(P1 p1, P2 p2) {
    LOG(INFO) << "RCB_2_0";
    return r1;
  }

  R1   RCB_0_1(A1 a1) {
    LOG(INFO) << "RCB_0_1";
    return r1;
  }

  R1   RCB_1_1(P1 p1, A1 a1) {
    LOG(INFO) << "RCB_1_1";
    return r1;
  }

  R1   RCB_2_1(P1 p1, P2 p2, A1 a1) {
    LOG(INFO) << "RCB_2_1";
    return r1;
  }

  R1   RCB_0_2(A1 a1, A2 a2) {
    LOG(INFO) << "RCB_0_2";
    return r1;
  }

  R1   RCB_1_2(P1 p1, A1 a1, A2 a2) {
    LOG(INFO) << "RCB_1_2";
    return r1;
  }

  R1   RCB_2_2(P1 p1, P2 p2, A1 a1, A2 a2) {
    LOG(INFO) << "RCB_2_2";
    return r1;
  }

  R1   CRCB_0_0() const {
    LOG(INFO) << "CRCB_0_0";
    return r1;
  }

  R1   CRCB_1_0(P1 p1) const {
    LOG(INFO) << "CRCB_1_0";
    return r1;
  }

  R1   CRCB_2_0(P1 p1, P2 p2) const {
    LOG(INFO) << "CRCB_2_0";
    return r1;
  }

  R1   CRCB_0_1(A1 a1) const {
    LOG(INFO) << "CRCB_0_1";
    return r1;
  }

  R1   CRCB_1_1(P1 p1, A1 a1) const {
    LOG(INFO) << "CRCB_1_1";
    return r1;
  }

  R1   CRCB_2_1(P1 p1, P2 p2, A1 a1) const {
    LOG(INFO) << "CRCB_2_1";
    return r1;
  }

  R1   CRCB_0_2(A1 a1, A2 a2) const {
    LOG(INFO) << "CRCB_0_2";
    return r1;
  }

  R1   CRCB_1_2(P1 p1, A1 a1, A2 a2) const {
    LOG(INFO) << "CRCB_1_2";
    return r1;
  }

  R1   CRCB_2_2(P1 p1, P2 p2, A1 a1, A2 a2) const {
    LOG(INFO) << "CRCB_2_2";
    return r1;
  }

  int intrusive_count_;
};

// Function callbacks
static void SCB_0_0()                           { LOG(INFO) << "SCB_0_0"; }
static void SCB_1_0(P1 p1)                      { LOG(INFO) << "SCB_1_0"; }
static void SCB_2_0(P1 p1, P2 p2)               { LOG(INFO) << "SCB_2_0"; }

static void SCB_0_1(A1 a1)                      { LOG(INFO) << "SCB_0_1"; }
static void SCB_1_1(P1 p1, A1 a1)               { LOG(INFO) << "SCB_1_1"; }
static void SCB_2_1(P1 p1, P2 p2, A1 a1)        { LOG(INFO) << "SCB_2_1"; }

static void SCB_0_2(A1 a1, A2 a2)               { LOG(INFO) << "SCB_0_2"; }
static void SCB_1_2(P1 p1, A1 a1, A2 a2)        { LOG(INFO) << "SCB_1_2"; }
static void SCB_2_2(P1 p1, P2 p2, A1 a1, A2 a2) { LOG(INFO) << "SCB_2_2"; }

static R1   SRCB_0_0() {
  LOG(INFO) << "SRCB_0_0";
  return r1;
}

static R1   SRCB_1_0(P1 p1) {
  LOG(INFO) << "SRCB_1_0";
  return r1;
}

static R1   SRCB_2_0(P1 p1, P2 p2) {
  LOG(INFO) << "SRCB_2_0";
  return r1;
}

static R1   SRCB_0_1(A1 a1) {
  LOG(INFO) << "SRCB_0_1";
  return r1;
}

static R1   SRCB_1_1(P1 p1, A1 a1) {
  LOG(INFO) << "SRCB_1_1";
  return r1;
}

static R1   SRCB_2_1(P1 p1, P2 p2, A1 a1) {
  LOG(INFO) << "SRCB_2_1";
  return r1;
}

static R1   SRCB_0_2(A1 a1, A2 a2) {
  LOG(INFO) << "SRCB_0_2";
  return r1;
}

static R1   SRCB_1_2(P1 p1, A1 a1, A2 a2) {
  LOG(INFO) << "SRCB_1_2";
  return r1;
}

static R1   SRCB_2_2(P1 p1, P2 p2, A1 a1, A2 a2) {
  LOG(INFO) << "SRCB_2_2";
  return r1;
}

TEST_F(CallbackTest, TestCompilation) {
  LOG(INFO) << "==== TestCompilation ====";
  CT* c = new CT;
  const CT *cc = new CT;

  Do(NewCallback(c, &CT::CB_0_0));
  Do(NewCallback(c, &CT::CB_1_0, p1));
  Do(NewCallback(c, &CT::CB_2_0, p1, p2));

  Do(NewCallback(c, &CT::CB_0_1));
  Do(NewCallback(c, &CT::CB_1_1, p1));
  Do(NewCallback(c, &CT::CB_2_1, p1, p2));

  Do(NewCallback(c, &CT::CB_0_2));
  Do(NewCallback(c, &CT::CB_1_2, p1));
  Do(NewCallback(c, &CT::CB_2_2, p1, p2));

  Do(NewCallback(c, &CT::RCB_0_0));
  Do(NewCallback(c, &CT::RCB_1_0, p1));
  Do(NewCallback(c, &CT::RCB_2_0, p1, p2));

  Do(NewCallback(c, &CT::RCB_0_1));
  Do(NewCallback(c, &CT::RCB_1_1, p1));
  Do(NewCallback(c, &CT::RCB_2_1, p1, p2));

  Do(NewCallback(c, &CT::RCB_0_2));
  Do(NewCallback(c, &CT::RCB_1_2, p1));
  Do(NewCallback(c, &CT::RCB_2_2, p1, p2));

  Do(NewCallback(cc, &CT::CCB_0_0));
  Do(NewCallback(cc, &CT::CCB_1_0, p1));
  Do(NewCallback(cc, &CT::CCB_2_0, p1, p2));

  Do(NewCallback(cc, &CT::CCB_0_1));
  Do(NewCallback(cc, &CT::CCB_1_1, p1));
  Do(NewCallback(cc, &CT::CCB_2_1, p1, p2));

  Do(NewCallback(cc, &CT::CCB_0_2));
  Do(NewCallback(cc, &CT::CCB_1_2, p1));
  Do(NewCallback(cc, &CT::CCB_2_2, p1, p2));

  Do(NewCallback(cc, &CT::CRCB_0_0));
  Do(NewCallback(cc, &CT::CRCB_1_0, p1));
  Do(NewCallback(cc, &CT::CRCB_2_0, p1, p2));

  Do(NewCallback(cc, &CT::CRCB_0_1));
  Do(NewCallback(cc, &CT::CRCB_1_1, p1));
  Do(NewCallback(cc, &CT::CRCB_2_1, p1, p2));

  Do(NewCallback(cc, &CT::CRCB_0_2));
  Do(NewCallback(cc, &CT::CRCB_1_2, p1));
  Do(NewCallback(cc, &CT::CRCB_2_2, p1, p2));

  Do(NewCallback(&SCB_0_0));
  Do(NewCallback(&SCB_1_0, p1));
  Do(NewCallback(&SCB_2_0, p1, p2));

  Do(NewCallback(&SCB_0_1));
  Do(NewCallback(&SCB_1_1, p1));
  Do(NewCallback(&SCB_2_1, p1, p2));

  Do(NewCallback(&SCB_0_2));
  Do(NewCallback(&SCB_1_2, p1));
  Do(NewCallback(&SCB_2_2, p1, p2));

  Do(NewCallback(&SRCB_0_0));
  Do(NewCallback(&SRCB_1_0, p1));
  Do(NewCallback(&SRCB_2_0, p1, p2));

  Do(NewCallback(&SRCB_0_1));
  Do(NewCallback(&SRCB_1_1, p1));
  Do(NewCallback(&SRCB_2_1, p1, p2));

  Do(NewCallback(&SRCB_0_2));
  Do(NewCallback(&SRCB_1_2, p1));
  Do(NewCallback(&SRCB_2_2, p1, p2));

  delete cc;
  delete c;
}

template <typename T0>
static void Repeatable(bool r, T0* cb) {
  CHECK_EQ(r, cb->IsRepeatable());
  Do(cb);
  if (r) {
    Do(cb);
    delete cb;
  }
}

TEST_F(CallbackTest, TestCallbackIsRepeatable) {
  LOG(INFO) << "==== TestCallbackIsRepeatable ====";
  CT* c = new CT;
  const CT* cc = new CT;

  Repeatable(false, NewCallback(c, &CT::CB_0_0));
  Repeatable(false, NewCallback(c, &CT::CB_1_0, p1));
  Repeatable(false, NewCallback(c, &CT::CB_2_0, p1, p2));

  Repeatable(false, NewCallback(&SCB_0_0));
  Repeatable(false, NewCallback(&SCB_1_0, p1));
  Repeatable(false, NewCallback(&SCB_2_0, p1, p2));

  Repeatable(true, NewPermanentCallback(c, &CT::CB_0_0));
  Repeatable(true, NewPermanentCallback(c, &CT::CB_1_0, p1));
  Repeatable(true, NewPermanentCallback(c, &CT::CB_2_0, p1, p2));

  Repeatable(true, NewPermanentCallback(&SCB_0_0));
  Repeatable(true, NewPermanentCallback(&SCB_1_0, p1));
  Repeatable(true, NewPermanentCallback(&SCB_2_0, p1, p2));

  Repeatable(false, NewCallback(cc, &CT::CCB_0_0));
  Repeatable(false, NewCallback(cc, &CT::CCB_1_0, p1));
  Repeatable(false, NewCallback(cc, &CT::CCB_2_0, p1, p2));

  Repeatable(true, NewPermanentCallback(cc, &CT::CCB_0_0));
  Repeatable(true, NewPermanentCallback(cc, &CT::CCB_1_0, p1));
  Repeatable(true, NewPermanentCallback(cc, &CT::CCB_2_0, p1, p2));

  delete cc;
  delete c;
}

static const std::string* FooWithConstRef(const std::string& a) {
  return &a;
}

TEST_F(CallbackTest, TestConstReferenceArg) {
  std::string s;
  Callback<const std::string*()>* c = NewCallback(&FooWithConstRef, s);
  // s should be bind by value, but pass by const reference
  ASSERT_NE(&s, c->Run());
}

}  // namespace gdt
