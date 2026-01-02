#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GetJSONData : public RE::GFxFunctionHandler
{

public:
    json LoadJSON(std::string filename);
    void Call(Params& params) override;
};
