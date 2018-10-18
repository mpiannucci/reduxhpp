#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace redux {

class Action {
  public:
	virtual ~Action() {

	}

	virtual int id() const {
		return -1;
	}
};

class ActionCreator {
  public:
	virtual ~ActionCreator() {

	}

	virtual Action execute() const = 0;
};

template <typename T>
class Store {

	typedef T (*Reducer)(const T& state, const Action& action);

  public:
	class Subscriber {
	public:
		virtual ~Subscriber() {
		}

	    virtual void new_state(T state) = 0;
	};

	Store(T initialState, Reducer reducer) :
	  m_state(initialState),
	  m_reducer(reducer) {

	}

	void dispatch(const Action& action) {
		m_state = m_reducer(m_state, action);
		notify_subscriptions();
	}

	void dispatch(const ActionCreator& action_creator) {
		auto action = action_creator.execute();
		dispatch(action); 
	}

	void subscribe(const std::shared_ptr<Subscriber>& new_subscriber) {
		auto existing_subscriber = std::find_if(m_subscribers.begin(), m_subscribers.end(), [&new_subscriber](const std::weak_ptr<Subscriber> weak_subscriber){
			return weak_subscriber.lock() == new_subscriber;
		});
		if (existing_subscriber != m_subscribers.end()) {
			return;
		}

		m_subscribers.push_back(new_subscriber);
		new_subscriber->new_state(m_state);
	}

	void unsubscribe(const std::shared_ptr<Subscriber>& subscriber) {
		auto existing_subscriber = std::find_if(m_subscribers.begin(), m_subscribers.end(), [&subscriber](const std::weak_ptr<Subscriber> weak_subscriber){
			return weak_subscriber.lock() == subscriber;
		});
		if (existing_subscriber == m_subscribers.end()) {
			return;
		}

		m_subscribers.erase(existing_subscriber);
	}

  private:
  	void notify_subscriptions() {
  		for (auto weak_subscriber : m_subscribers) {
            if (auto subscriber = weak_subscriber.lock()) {
  			    subscriber->new_state(m_state);
            }
  		}
  	}

    T m_state;
    Reducer m_reducer;
    std::vector<std::weak_ptr<Subscriber>> m_subscribers;
};

}