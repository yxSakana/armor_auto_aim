/**
 * @projectName armor_auto_aim
 * @file test_utils.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-27 21:51
 */

#include <armor_detector/utils.h>

namespace armor_auto_aim {
std::string getInputAndOutputsInfo(const ov::Model& network) {
    std::string info;
    info += "model name: " + network.get_friendly_name() + "\n";
    // input information
    const std::vector<ov::Output<const ov::Node>> inputs = network.inputs();
    for (const ov::Output<const ov::Node>& input : inputs) {
        info += "    inputs\n";
        const std::string name = input.get_names().empty() ? "NONE" : input.get_any_name();
        info += "        input name: " + name + "\n";
        const ov::element::Type type = input.get_element_type();
        info += "        input type: " + type.to_string() + "\n";
        const ov::Shape& shape = input.get_shape();
        info += "        input shape: " + shape.to_string() + "\n";
    //        const ov::Layout& layout = network.get_parameters()[1]->get_layout();
    //        info += "        input layout: " + layout.to_string() + "\n";
    }
    // output information
    const std::vector<ov::Output<const ov::Node>> outputs = network.outputs();
    for (const ov::Output<const ov::Node>& output : outputs) {
        info += "    outputs\n";
        const std::string name = output.get_names().empty() ? "NONE" : output.get_any_name();
        info += "        output name: " + name + "\n";
        const ov::element::Type type = output.get_element_type();
        info += "        output type: " + type.to_string() + "\n";
        const ov::Shape& shape = output.get_shape();
        info += "        output shape: " + shape.to_string();
    }
    return info;
}
}
