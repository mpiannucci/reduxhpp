#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include "redux.hpp"

struct CounterState {
    int value = 0;
};

enum Actions {
    INCREMENT = 1,
    DECREMENT = 2,
    STAY = 3,
    SET_VALUE= 4
};

struct IncrementAction : public redux::Action {
    virtual int id() const override {
        return Actions::INCREMENT;
    }
};

struct DecrementAction : public redux::Action {
    virtual int id() const override {
        return Actions::DECREMENT;
    }
};

struct StayAction : public redux::Action {
    virtual int id() const override {
        return Actions::STAY;
    }
};

struct SetValueAction : public redux::Action {
    int value;

    SetValueAction(int value_) : value(value_) {}

    virtual int id() const override {
        return Actions::SET_VALUE;
    }
};

struct IncrementLaterAction : public redux::ActionCreator {

    int defer_time;
    redux::Store<CounterState>* store;

    IncrementLaterAction(int defer, redux::Store<CounterState>* store_) {
        defer_time = defer;
        store = store_;
    }

    virtual redux::Action execute() const override {
        std::async([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(defer_time));
            store->dispatch(IncrementAction());
        });

        return StayAction();
    }
};

CounterState reducer(const CounterState& state, const redux::Action& action) {
    CounterState new_state = state;

    Actions counter_action = static_cast<Actions>(action.id());

    switch (counter_action) {
    case INCREMENT:
        new_state.value = state.value + 1;
        break;
    case DECREMENT:
        new_state.value = state.value - 1;
        break;
    case SET_VALUE: {
        const SetValueAction& set_action = static_cast<const SetValueAction&>(action);
        new_state.value = set_action.value;
        break;
    }
    default:
        break;
    }

    return new_state;
}

class CounterSubscriber : public redux::Store<CounterState>::Subscriber {
public:
    int value = 0;

    virtual void new_state(CounterState new_state) override {
        if (value != new_state.value) {
            std::cout << "Value Changed: " << new_state.value << std::endl;
            value = new_state.value;
        }
    }
};

int main(int argc, char* argv[]) {
    redux::Store<CounterState> store(CounterState{ 0 }, reducer);
    auto subscriber = std::make_shared<CounterSubscriber>();
    store.subscribe(subscriber);

    store.dispatch(SetValueAction{7});
    store.dispatch(IncrementAction());
    store.dispatch(IncrementAction());
    store.dispatch(IncrementAction());
    store.dispatch(IncrementLaterAction{ 2000, &store });

    return 0;
}