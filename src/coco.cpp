//
// Created by YangLinzhuo on 2023/1/19.
//
#include "coco.h"
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <chrono>
#include <iterator>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

using nlohmann::json;


void to_json(json& j, const Annotation& a) {
    j = json{ {"segmentation", a.segmentation}, {"area", a.area}, {"is_crowd", a.is_crowd},
              {"image_id", a.image_id}, {"bbox", a.bbox}, {"category_id", a.category_id},
              {"id", a.id} };
}

void from_json(const json& j, Annotation& a) {
//    if (j.contains("segmentation")) {
//        fmt::print("Get segmentation.\n");
//        j.at("segmentation").get_to(a.segmentation);
//    }
    j.contains("area") ? j.at("area").get_to(a.area) : a.area = -1.0;
    j.contains("is_crowd") ? j.at("is_crowd").get_to(a.is_crowd) : a.is_crowd = -1;
    j.contains("image_id") ? j.at("image_id").get_to(a.image_id) : a.image_id = -1;
    if (j.contains("bbox")) {
//        fmt::print("Get bbox.\n");
        j.at("bbox").get_to(a.bbox);
    }
    j.contains("category_id") ? j.at("category_id").get_to(a.category_id) : a.category_id = -1;
    j.contains("id") ? j.at("id").get_to(a.id) : a.id = -1;
}

void to_json(json& j, const Image& a) {
    j = json{ {"license", a.license}, {"coco_url", a.coco_url}, {"file_name", a.file_name},
              {"height", a.height}, {"width", a.width}, {"date_captured", a.date_captured},
              {"flickr_url", a.flickr_url}, {"id", a.id} };
}

void from_json(const json& j, Image& a) {
    if (j.contains("license")) {
        j.at("license").get_to(a.license);
    }
    if (j.contains("coco_url")) {
        j.at("coco_url").get_to(a.coco_url);
    }
    if (j.contains("file_name")) {
        j.at("file_name").get_to(a.file_name);
    }
    j.contains("height") ? j.at("height").get_to(a.height) : a.height = -1;
    j.contains("width") ? j.at("width").get_to(a.width) : a.width = -1;
    if (j.contains("date_captured")) {
        j.at("date_captured").get_to(a.date_captured);
    }
    if (j.contains("flickr_url")) {
        j.at("flickr_url").get_to(a.flickr_url);
    }
    j.contains("id") ? j.at("id").get_to(a.id) : a.id = -1;
}

void to_json(json& j, const Category& a) {
    j = json{ {"supercategory", a.super_category}, {"id", a.id}, {"name", a.name} };
}
void from_json(const json& j, Category& a) {
    if (j.contains("supercategory")) {
        j.at("supercategory").get_to(a.super_category);
    }
    j.contains("id") ? j.at("id").get_to(a.id) : a.id = -1;
    if (j.contains("name")) {
        j.at("name").get_to(a.name);
    }
}

COCO::COCO(const std::string& annotation_file_path) {
    if (annotation_file_path.empty()) {
        return;
    }
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
    auto start = std::chrono::high_resolution_clock::now();

    if (this->dataset.contains("annotations")) {
        fmt::print("[INFO] Creating annotations index...\n");
        int64_t idx = 0;
        for (auto& annotation : this->dataset["annotations"]) {
            this->annotations.emplace_back(annotation);
            this->annotations_map[annotation["id"]] = idx;
            this->img2ann[annotation["image_id"]].emplace_back(annotation["id"]);
            idx++;
        }
    }

    if (this->dataset.contains("images")) {
        fmt::print("[INFO] Creating images index...\n");
        int64_t idx = 0;
        for (auto& image : this->dataset["images"]) {
            this->images.emplace_back(image);
            this->images_map[image["id"]] = idx;
            idx++;
        }
    }

    if (this->dataset.contains("categories")) {
        fmt::print("[INFO] Creating categories index...\n");
        int64_t idx = 0;
        for (auto& category : this->dataset["categories"]) {
            this->categories.emplace_back(category);
            this->categories_map[category["id"]] = idx;
            idx++;
        }
    }

    if (this->dataset.contains("annotations") && this->dataset.contains("categories")) {
        for (auto& annotation : this->dataset["annotations"]) {
            this->cat2img[annotation["category_id"]].emplace_back(annotation["image_id"]);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    fmt::print("[INFO] Index created.\n");
    std::chrono::duration<float> duration = end - start;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    fmt::print("Done (t={})\n", ms);
}


void COCO::info() {
    for (auto& [key, value] : this->dataset["info"].items())
    {
        std::cout << key << ": " << value << std::endl;
    }
}

std::vector<int64_t> COCO::getAllAnnIds() {
    std::vector<int64_t> ids;
    for (auto& pair : annotations_map) {
        ids.emplace_back(pair.first);
    }
    return ids;
}


std::vector<int64_t> COCO::getAllCatIds() {
    std::vector<int64_t> ids;
    for (auto& pair : categories_map) {
        ids.emplace_back(pair.first);
    }
    return ids;
}


std::vector<int64_t> COCO::getAllImgIds() {
    std::vector<int64_t> ids;
    for (auto& pair : images_map) {
        ids.emplace_back(pair.first);
    }
    return ids;
}


std::vector<int64_t> COCO::getAnnIds(
        const std::vector<int64_t> &img_ids, const std::vector<int64_t> &cat_ids,
        const std::vector<float> &area_rng, int is_crowd
) {
//    if img_ids.s == len(catIds) == len(areaRng) == 0:
//    ann_ids = self.dataset['annotations']
    std::vector<int64_t> ann_ids;
    if (img_ids.empty() && cat_ids.empty() && area_rng.empty()) {
        ann_ids = this->getAllAnnIds();
    } else {
        if (!img_ids.empty()) {
            for (auto& img_id : img_ids) {
                if (img2ann.find(img_id) != img2ann.end()) {
                    auto& img_anns = img2ann[img_id];
                    ann_ids.insert(ann_ids.end(),
                                   img_anns.begin(),
                                   img_anns.end());
                }
            }
        } else {
            ann_ids = this->getAllAnnIds();
        }
        auto end = ann_ids.end();
        if (!cat_ids.empty()) {
            end = std::remove_if(ann_ids.begin(), end, [&cat_ids, this](int64_t id){
                auto cat = annotations[annotations_map[id]].category_id;
                return std::find(cat_ids.begin(), cat_ids.end(), cat) != cat_ids.end();
            });
        }
        if (!area_rng.empty()) {
            end = std::remove_if(ann_ids.begin(), end, [&area_rng, this](int64_t id){
                auto area = annotations[annotations_map[id]].area;
                return area <= area_rng[0] || area >= area_rng[1];
            });
        }
        ann_ids.erase(end, ann_ids.end());
    }

    if (is_crowd != -1) {
        auto end = std::remove_if(ann_ids.begin(), ann_ids.end(), [this, is_crowd](int64_t id) {
            auto crowd = annotations[annotations_map[id]].is_crowd;
            return crowd != is_crowd;
        });
        ann_ids.erase(end, ann_ids.end());
    }
    return ann_ids;
}


std::vector<int64_t> COCO::getCatIds(const std::vector<std::string> &cat_names,
                                     const std::vector<std::string> &super_category_names,
                                     const std::vector<int64_t> &cat_ids) {
    std::vector<int64_t> ids;
    if (cat_names.empty() && super_category_names.empty() && cat_ids.empty()) {
        ids = getAllCatIds();
    } else {
        ids = getAllCatIds();
        auto end = ids.end();
        if (!cat_names.empty()) {
            end = std::remove_if(ids.begin(), end, [&cat_names, this](int64_t id){
                auto& name = categories[categories_map[id]].name;
                return std::find(cat_names.begin(), cat_names.end(), name) != cat_names.end();
            });
        }
        if (!super_category_names.empty()) {
            end = std::remove_if(ids.begin(), end, [&super_category_names, this](int64_t id) {
                auto& name = categories[categories_map[id]].super_category;
                return std::find(super_category_names.begin(), super_category_names.end(), name) != super_category_names.end();
            });
        }
        if (!cat_ids.empty()) {
            end = std::remove_if(ids.begin(), end, [&cat_ids, this](int64_t id){
                auto cat = categories[categories_map[id]].id;
                return std::find(cat_ids.begin(), cat_ids.end(), cat) != cat_ids.end();
            });
        }
        ids.erase(end, ids.end());
    }
    return ids;
}


std::vector<int64_t> COCO::getImgIds(const std::vector<int64_t> &img_ids, const std::vector<int64_t> &cat_ids) {
    std::vector<int64_t> ids;
    if (img_ids.empty() && cat_ids.empty()) {
        ids = getAllImgIds();
    } else {
        std::set<int64_t> tmp;
        for (auto id : cat_ids) {
            auto& imgs_ids = cat2img[id];
            tmp.insert(imgs_ids.begin(), imgs_ids.end());
        }
        ids.insert(ids.end(), tmp.begin(), tmp.end());
    }
    return ids;
}