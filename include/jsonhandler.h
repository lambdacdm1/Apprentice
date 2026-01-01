#pragma once

#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class GetJSONData : public RE::GFxFunctionHandler
{

public:
    std::string path_to_json;

    json LoadJSON(std::string filename);
    void Call(Params& params) override;

    static void InjectScaleform();
};
