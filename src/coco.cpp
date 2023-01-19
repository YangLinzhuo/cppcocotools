//
// Created by YangLinzhuo on 2023/1/19.
//
#include "coco.h"
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <chrono>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

using nlohmann::json;

COCO::COCO(std::string& annotation_file_path) {
    fmt::print("[INFO] Loading annotations into memory...\n");
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream annotation_file(annotation_file_path);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
//    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
//    fmt::print("Done (t={:.2f}ms)\n", duration.count());
    fmt::print("Done (t={})\n", ms);
    this->dataset = json::parse(annotation_file);
    fmt::print("Dataset size: {}\n", this->dataset.size());
//    fmt::print("Dataset info: {}", this->dataset["info"]);
    std::cout << "Dataset info: " << this->dataset["info"] << std::endl;
}


void COCO::createIndex() {
    fmt::print("[INFO] Creating index...\n");
    if (this->dataset.contains("annotations")) {
        fmt::print("[INFO] Creating annotations index...\n");
        std::map<int64_t, json> anns;
        auto annotations = this->dataset["annotations"];
        for (auto& ann : annotations) {
            this->img2ann[ann["image_id"]].emplace_back(ann);
            anns[ann["id"]] = ann;
        }
    }

    if (this->dataset.contains("images")) {
        fmt::print("[INFO] Creating images index...\n");
        std::map<int64_t, json> imgs;
        auto images = this->dataset["images"];
        for (auto& img : images) {
            imgs[img["id"]] = img;
        }
    }
    fmt::print("[INFO] Index created.\n");
}