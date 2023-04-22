#include "Bluetooth/libbt/bt_mock.hpp"

namespace bluetooth {
namespace libbt {
namespace unittest {

testing::NiceMock<BTMock> *BTMock::btmock = nullptr;

BTMock *BTMock::getInstance() {
    if (BTMock::btmock == nullptr) {
        BTMock::btmock = new testing::NiceMock<BTMock>();
    }

    return BTMock::btmock;
}

void BTMock::removeInstance() {
    if (BTMock::btmock) {
        delete BTMock::btmock;
    }

    BTMock::btmock = nullptr;
}

}   /* unittest*/
}   /* libbt */
}   /* bluetooth */