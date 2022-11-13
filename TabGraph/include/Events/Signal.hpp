/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:15:35
*/
#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace TabGraph::Events {
/**
* Inspired by schneegans
* See : https://schneegans.github.io/tutorials/2015/09/20/signal-slot
*/
class Trackable {
public:
    class TrackablePointee {
    };
    Trackable() = default;
    Trackable(const Trackable&)
        : _controlBlock { static_cast<TrackablePointee*>(0) } {};
    auto GetWeakPtr() { return std::weak_ptr(_controlBlock); }

private:
    std::shared_ptr<TrackablePointee> _controlBlock { static_cast<TrackablePointee*>(0), [](TrackablePointee*) {} };
};

template <typename... Args>
class Signal : public Trackable {
public:
    using SlotID = uint32_t;
    struct Slot {
        Slot() = default;
        ~Slot() = default;
        bool Connected()
        {
            return !_signalRef.expired();
        }
        void Disconnect()
        {
            if (Connected())
                _signal->_Disconnect(this->_id);
            Reset();
        }
        SlotID Id()
        {
            return _id;
        }

    private:
        friend Signal;
        void Reset()
        {
            _signal = nullptr;
            _func = nullptr;
            _id = 0;
            _signalRef.reset();
            _trackedObjectRef.reset();
        }
        Slot(std::function<void(Args...)> func, SlotID id,
            Signal* signal, Trackable* trackable = nullptr)
            : _func(func)
            , _id(id)
            , _signal(signal)
            , _signalRef(signal->GetWeakPtr())
            , _trackedObjectRef(trackable ? trackable->GetWeakPtr() : signal->GetWeakPtr()) {};
        auto operator()(Args... args)
        {
            if (_func)
                return _func(args...);
        }
        std::function<void(Args...)> _func {};
        SlotID _id { 0 };
        Signal* _signal { nullptr };
        std::weak_ptr<TrackablePointee> _signalRef;
        std::weak_ptr<TrackablePointee> _trackedObjectRef;
    };
    struct ScoppedSlot : public Slot {
        ScoppedSlot() = default;
        ~ScoppedSlot()
        {
            Disconnect();
        }
        ScoppedSlot(const ScoppedSlot& other)
        {
            *this = other;
        }
        ScoppedSlot(const Slot& other)
        {
            *this = other;
        }
        ScoppedSlot& operator=(const ScoppedSlot& other)
        {
            Disconnect();
            _func = other._func;
            _id = other._id;
            _signal = other._signal;
            _signalRef = other._signalRef;
            _trackedObjectRef = other._trackedObjectRef;
            return *this;
        }
        ScoppedSlot& operator=(const Slot& other)
        {
            Disconnect();
            _func = other._func;
            _id = other._id;
            _signal = other._signal;
            _signalRef = other._signalRef;
            _trackedObjectRef = other._trackedObjectRef;
            return *this;
        }
    };

    Signal() {};
    ~Signal() {};
    Signal(const Signal&)
        : Signal()
    {
    }

    /**
	* @brief Connects a row functor to the signal
	* @return The slot ID
	*/
    auto Connect(std::function<void(Args...)> const& func)
    {
        ++_currentSlotID;
        //if slot is not member method, tracked object is this signal -> never expires until Signal's destruction
        return _Connect(Slot(func, _currentSlotID, this, this));
    }

    /**
    * @brief Connect member using raw ptr, less safe if object gets destroyed
    * @argument inst : the class instance the functor will use
    * @argument func : the functor to connect
    * @return The slot ID
    */
    template <typename T>
    auto ConnectMember(T* inst, void (T::*func)(Args...))
    {
        static_assert(std::is_base_of<Trackable, T>::value, "T must inherit Trackable");
        ++_currentSlotID;
        auto lambda = [inst, func](Args... args) {
            (inst->*func)(args...);
        };
        return _Connect(Slot(lambda, _currentSlotID, this, inst));
        //return _slots[_currentSlotID] = Slot(lambda, _currentSlotID, this, inst);
    }

    /**
	* @brief const overload of ConnectMember
	*/
    template <typename T>
    auto ConnectMember(T* inst, void (T::*func)(Args...) const)
    {
        static_assert(std::is_base_of<Trackable, T>::value, "T must inherit Trackable");
        ++_currentSlotID;
        auto lambda = [inst, func](Args... args) {
            (inst->*func)(args...);
        };
        return _Connect(Slot(lambda, _currentSlotID, this, inst));
        //return _slots[_currentSlotID] = Slot(lambda, _currentSlotID, this, inst);
    }

    /**
	* @brief Emits the signal and calls the functors connected to it
	*/
    auto operator()(Args... args)
    {
        std::unique_lock lock(_lock);
        for (auto& slotIt : _slots) {
            auto& slot { slotIt.second };
            //if slot is not member method, tracked object is this signal -> never expires until Signal's destruction
            if (slot._trackedObjectRef.expired()) {
                slot.Disconnect();
                continue;
            }
            if (std::find(_toDisconnect.begin(), _toDisconnect.end(), slot._id) == _toDisconnect.end())
                slot(args...);
        }
        for (const auto& slot : _toConnect)
            _slots[slot._id] = slot;
        _toConnect.clear();
        //IMPORTANT : If we have disconnected a signal after connecting it while emitting this signal, it should be disconnected at the end
        for (const auto& id : _toDisconnect)
            _slots.erase(id);
        _toDisconnect.clear();
    }

private:
    friend Slot;
    /**
    * @brief disconnects a slot from the signal
    */
    inline auto _Disconnect(SlotID slot)
    {
        if (_lock.try_lock()) {
            _slots.erase(slot);
            _lock.unlock();
        } else
            _toDisconnect.push_back(slot);
    }

    inline auto _Connect(const Slot& slot)
    {
        if (_lock.try_lock()) {
            _slots[slot._id] = slot;
            _lock.unlock();
        } else
            _toConnect.push_back(slot);
        return slot;
    }
    std::unordered_map<SlotID, Slot> _slots;
    std::vector<SlotID> _toDisconnect;
    std::vector<Slot> _toConnect;
    std::mutex _lock;
    SlotID _currentSlotID { 0 };
};
}