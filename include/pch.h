#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logger = SKSE::log;
using namespace std::literals;


template <typename T>
class Singleton
{
protected:
    constexpr Singleton() noexcept = default;
    constexpr ~Singleton() noexcept = default;

public:
    constexpr Singleton(const Singleton&) = delete;
    constexpr Singleton(Singleton&&) = delete;
    constexpr auto operator=(const Singleton&) = delete;
    constexpr auto operator=(Singleton&&) = delete;

    [[nodiscard]] static constexpr T* GetSingleton() noexcept
    {
        static T singleton;
        return std::addressof(singleton);
    }
};

template <typename TDerived, typename TEvent>
class EventSingleton : public RE::BSTEventSink<TEvent>
{
protected:
    constexpr EventSingleton() noexcept = default;
    constexpr ~EventSingleton() noexcept override = default;

public:
    constexpr EventSingleton(const EventSingleton&) = delete;
    constexpr EventSingleton(EventSingleton&&) = delete;
    constexpr auto operator=(const EventSingleton&) = delete;
    constexpr auto operator=(EventSingleton&&) = delete;

    [[nodiscard]] static constexpr TDerived* GetSingleton() noexcept
    {
        static TDerived singleton;
        return std::addressof(singleton);
    }

    static constexpr auto Register() noexcept
    {
        using TEventSource = RE::BSTEventSource<TEvent>;

        const std::string dirty_name{ typeid(TEvent).name() };
        const std::regex  p{ "struct |RE::|SKSE::| * __ptr64" };
        const auto        name{ std::regex_replace(dirty_name, p, "") };

        if constexpr (std::is_base_of_v<TEventSource, RE::BSInputDeviceManager>) {
            const auto manager{ RE::BSInputDeviceManager::GetSingleton() };
            manager->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_base_of_v<TEventSource, RE::UI>) {
            const auto ui{ RE::UI::GetSingleton() };
            ui->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::ActionEvent>) {
            SKSE::GetActionEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::CameraEvent>) {
            SKSE::GetCameraEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::CrosshairRefEvent>) {
            SKSE::GetCrosshairRefEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::ModCallbackEvent>) {
            SKSE::GetModCallbackEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_same_v<TEvent, SKSE::NiNodeUpdateEvent>) {
            SKSE::GetNiNodeUpdateEventSource()->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
        else if constexpr (std::is_base_of_v<TEventSource, RE::ScriptEventSourceHolder>) {
            const auto holder{ RE::ScriptEventSourceHolder::GetSingleton() };
            holder->AddEventSink(GetSingleton());
            logger::info("Registered {} handler", name);
            return;
        }
    }
};
