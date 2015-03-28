#pragma once

#include "output.h"

#include <string>
#include <vector>

namespace testpp
{
  //------------------------------------------------------------------------------
  enum RunFlags : uint32_t
  {
    NONE = 0,

    // COLOR means use ANSI color coded output: red for fail, green for pass.
    COLOR = 1 << 0,

    // ALPHA_ORDER means run tests in alphabetical order (default is random
    // order).
    ALPHA_ORDER = 1 << 1,

    // QUIET_SUCCESS means output for failing tests only.
    QUIET_SUCCESS = 1 << 2,
  };

  //------------------------------------------------------------------------------
  struct RunParams
  {
    RunParams(const Outputter& o)
      : m_outputter(o)
    {}
    const Outputter& m_outputter;
    uint32_t m_flags = COLOR | QUIET_SUCCESS;
    size_t m_numPropertyChecks = 100;
    unsigned long m_randomSeed = 0;
  };

  //------------------------------------------------------------------------------
  struct Result
  {
    std::string m_suiteName;
    std::string m_testName;
    bool m_success;
    std::string m_message;
  };

  using Results = std::vector<Result>;

  //------------------------------------------------------------------------------
  class Test
  {
  public:
    Test(const std::string& name, const std::string& suiteName = std::string());
    virtual ~Test();

    virtual bool Setup(const RunParams&) { return true; }
    virtual void Teardown() {}
    virtual bool Run() { return true; }

    Result RunWrapper()
    {
      Result r;
      r.m_success = Run() && m_success;
      r.m_message = m_message;
      return r;
    }

    const std::string& name() const { return m_name; }
    bool success() const { return m_success; }

  protected:
    bool m_success = true;
    std::string m_name;
    std::string m_message;
  };
}

//------------------------------------------------------------------------------
#define DECLARE_TEST(NAME, SUITE)               \
  class SUITE##NAME : public testpp::Test       \
  {                                             \
  public:                                       \
    SUITE##NAME()                               \
      : testpp::Test(#NAME, #SUITE) {}          \
    virtual bool Run() override;                \
  } s_##SUITE##NAME##_Test;                     \
  bool SUITE##NAME::Run()

//------------------------------------------------------------------------------
#include "test_registry.h"

namespace testpp
{
  //------------------------------------------------------------------------------
  inline Results RunAllTests(const RunParams& params)
  {
    return GetTestRegistry().RunAllTests(params);
  }

  inline Results RunSuite(const std::string& suiteName, const RunParams& params)
  {
    return GetTestRegistry().RunSuite(suiteName, params);
  }

  inline Results RunTest(const std::string& testName, const RunParams& params)
  {
    return GetTestRegistry().RunTest(testName, params);
  }

  //------------------------------------------------------------------------------
  inline Test::Test(const std::string& name, const std::string& suite)
    : m_name(name)
  {
    GetTestRegistry().Register(this, name, suite);
  }

  inline Test::~Test()
  {
    GetTestRegistry().Unregister(this);
  }
}

#include "test_macros.h"
