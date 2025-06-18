#pragma once
#include "RenderPass.h"

/// 只有输出 "Cleared"，无需任何输入
class ClearPass : public RenderPass {
public:
    ClearPass();
    const std::string& getName() const override;
    const std::vector<std::string>& getInputs() const override;
    const std::vector<std::string>& getOutputs() const override;
    void execute(IDevice* device, ICommandBuffer* cmd) override;

private:
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
};
