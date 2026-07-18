#pragma once 

namespace Utils
{
    void SendModEvent(std::string_view eventName, std::string_view strArg = {}, float numArg = 0.0f, RE::TESForm* sender = nullptr);

    void RegisterString(RE::GFxValue* object, RE::GFxMovie* movie, std::string stringName, std::string stringContents);

    void AddBooleanMember(RE::GFxValue* entry, bool val, std::string_view name);
    void AddStringMember(RE::GFxValue* entry, const char* val, const char* name);
    void AddIntMember(RE::GFxValue* entry, i32 val, std::string_view name);
    void AddNumberMember(RE::GFxValue* entry, double val, std::string_view name);

    bool             GetBooleanMember(RE::GFxValue& obj, const char* memberName);
    std::string_view GetStringMember(RE::GFxValue& obj, const char* memberName);
    i32              GetIntMember(RE::GFxValue& obj, const char* memberName);

    bool IsClassOrTrait(RE::GFxValue entry);
} // namespace Utils

namespace GetTESGlobalNS
{
    class GetTESGlobal : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override;
    };

    bool Install(RE::GFxMovieView*, RE::GFxValue*);
    void Register();
} // namespace GetTESGlobalNS

