#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Bluetooth/libbtplat/btplat_mock.hpp"
#include "Bluetooth/libbtres/btres_mock.hpp"
using namespace ::testing;
using ::testing::NiceMock;
using ::testing::Invoke;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

using bluetooth::libbtplat::unittest::BTPlatMock;
using bluetooth::libbtres::unittest::BTResMock;

class libbttest : public Test {
public:
    virtual  void SetUp();
    virtual  void TearDown();

    BTResMock *resMock = nullptr;
    BTPlatMock *platMock = nullptr;
};
#endif