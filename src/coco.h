//
// Created by YangLinzhuo on 2023/1/19.
//

#ifndef COCOTOOLS_COCO_H
#define COCOTOOLS_COCO_H

#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <fmt/core.h>
#include <fmt/chrono.h>

using nlohmann::json;

class COCO {
public:
    explicit COCO(std::string& annotation_file_path);
    void createIndex();
private:
    json dataset;
    std::map<int64_t, std::vector<json>> img2ann;
};

#endif //COCOTOOLS_COCO_H
