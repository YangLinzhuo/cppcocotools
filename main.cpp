#include "src/coco.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json;

int main() {
//    std::cout << "Hello, World!" << std::endl;
//    std::ifstream f("/home/linn/code/cpp_numpy/instances_val2017.json");
//    json data = json::parse(f);
    std::string annotation_file_path = "/home/linn/YOLOv5/datasets/annotations/instances_val2017.json";
    auto coco = COCO(annotation_file_path);
    coco.createIndex();
    return 0;
}
