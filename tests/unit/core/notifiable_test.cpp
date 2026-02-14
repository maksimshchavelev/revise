#include <core/Notifiable.hpp>
#include <gtest/gtest.h>

class MyNotifiable final : public core::Notifiable {
    EVENT(updated)
    EVENT(updated_value, int value)

public:
    int value() const noexcept {
        return m_value;
    }

    void update() {
        dispatch(updated{});
    }

    void update(int value) {
        m_value = value;
        dispatch(updated_value{m_value});
    }

  private:
    int m_value{0};
};


TEST(NotifiableTest, NoArgsSignal) {
    MyNotifiable n;
    bool called{false};

    n.connect<MyNotifiable::updated>([&called]([[maybe_unused]] const MyNotifiable::updated& signal){
        called = true;
    });

    n.update();

    ASSERT_TRUE(called);
}


TEST(NotifiableTest, OneArgumentSignal) {
    MyNotifiable n;
    bool called{false};
    int got_value{0};

    n.connect<MyNotifiable::updated_value>([&called, &got_value](const MyNotifiable::updated_value& signal){
        called = true;
        got_value = signal.value;
    });

    n.update(45);

    ASSERT_TRUE(called);
    ASSERT_EQ(got_value, 45);
}
