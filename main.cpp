#include "src/coco.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <fmt/ranges.h>

using nlohmann::json;

int main() {
//    std::cout << "Hello, World!" << std::endl;
//    std::ifstream f("/home/linn/code/cpp_numpy/instances_val2017.json");
//    json data = json::parse(f);
    std::string annotation_file_path = "/home/linn/YOLOv5/datasets/annotations/instances_val2017.json";
    std::string res_file = "/home/linn/YOLOv5/yolov5/coco2017_test/yolov5s8/yolov5s_predictions.json";
    auto coco = COCO(annotation_file_path);
    coco.createIndex();
    coco.info();
//    std::vector<int64_t> img_ids = { 397133, 37777, 252219, 87038, 174482, 403385, 6818, 480985, 458054, 331352 };
    auto start = std::chrono::high_resolution_clock::now();
//    auto ann_ids = coco.getImgIds();
    auto res = coco.loadRes(res_file);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
//    std::chrono::seconds ms = std::chrono::duration_cast<std::chrono::seconds>(duration);
    fmt::print("Get all ann ids Done (t={})\n", ms);
//    fmt::print("Annotation ids: {}", ann_ids);
    return 0;
}
