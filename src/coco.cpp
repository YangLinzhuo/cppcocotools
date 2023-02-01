//
// Created by YangLinzhuo on 2023/1/19.
//
#include "coco.h"
#include <nlohmann/json.hpp>
#include <string>
#include <set>
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
              {"id", a.id}, {"ignore", a.ignore} };
}

void from_json(const json& j, Annotation& a) {
//    if (j.contains("segmentation")) {
//        fmt::print("Get segmentation.\n");
//        j.at("segmentation").get_to(a.segmentation);
//    }
    j.contains("area") ? j.at("area").get_to(a.area) : a.area = -1.0;
    j.contains("is_crowd") ? j.at("is_crowd").get_to(a.is_crowd) : a.is_crowd = 0;
    j.contains("image_id") ? j.at("image_id").get_to(a.image_id) : a.image_id = -1;
    if (j.contains("bbox")) {
//        fmt::print("Get bbox.\n");
        j.at("bbox").get_to(a.bbox);
    }
    j.contains("category_id") ? j.at("category_id").get_to(a.category_id) : a.category_id = -1;
    j.contains("id") ? j.at("id").get_to(a.id) : a.id = -1;
    j.contains("ignore") ? j.at("ignore").get_to(a.ignore) : a.ignore = 0;
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
    dataset = json::parse(annotation_file);
    fmt::print("Dataset size: {}\n", dataset.size());
//    fmt::print("Dataset info: {}", dataset["info"]);
    std::cout << "Dataset info: " << dataset["info"] << std::endl;
}


void COCO::createIndex() {
    fmt::print("[INFO] Creating index...\n");
    auto start = std::chrono::high_resolution_clock::now();

    if (dataset.contains("annotations")) {
        fmt::print("[INFO] Creating annotations index...\n");
        int64_t idx = 0;
        for (auto& annotation : dataset["annotations"]) {
            annotations.emplace_back(annotation);
            annotations_map[annotation["id"]] = idx;
            img2ann[annotation["image_id"]].emplace_back(annotation["id"]);
            idx++;
        }
    } else if (!annotations.empty()) {
        for (auto& ann : annotations) {
            img2ann[ann.image_id].emplace_back(ann.id);
        }
    }

    if (dataset.contains("images")) {
        fmt::print("[INFO] Creating images index...\n");
        int64_t idx = 0;
        for (auto& image : dataset["images"]) {
            images.emplace_back(image);
            images_map[image["id"]] = idx;
            idx++;
        }
    }

    if (dataset.contains("categories")) {
        fmt::print("[INFO] Creating categories index...\n");
        int64_t idx = 0;
        for (auto& category : dataset["categories"]) {
            categories.emplace_back(category);
            categories_map[category["id"]] = idx;
            idx++;
        }
    }

    if (dataset.contains("annotations") && dataset.contains("categories")) {
        for (auto& annotation : dataset["annotations"]) {
            cat2img[annotation["category_id"]].emplace_back(annotation["image_id"]);
        }
    } else if (!annotations.empty() and !categories.empty()) {
        for (auto& ann : annotations) {
            cat2img[ann.category_id].emplace_back(ann.image_id);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    fmt::print("[INFO] Index created.\n");
    std::chrono::duration<float> duration = end - start;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    fmt::print("Done (t={})\n", ms);
}


void COCO::info() {
    for (auto& [key, value] : dataset["info"].items())
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
        ann_ids = getAllAnnIds();
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
            ann_ids = getAllAnnIds();
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


std::vector<Annotation> COCO::loadAnns(const std::vector<int64_t> &ann_ids) {
    std::vector<Annotation> anns;
    for (auto id : ann_ids) {
        anns.emplace_back(annotations[annotations_map[id]]);
    }
    return anns;
}


std::vector<Category> COCO::loadCats(const std::vector<int64_t> &cat_ids) {
    std::vector<Category> cats;
    for (auto id : cat_ids) {
        cats.emplace_back(categories[categories_map[id]]);
    }
    return cats;
}


std::vector<Image> COCO::loadImgs(const std::vector<int64_t> &img_ids) {
    std::vector<Image> imgs;
    for (auto id : img_ids) {
        imgs.emplace_back(images[images_map[id]]);
    }
    return imgs;
}


bool COCO::checkImgIds(const std::vector<int64_t> &annsImgIds) {
    std::set<int64_t> res_img_ids;
    res_img_ids.insert(annsImgIds.begin(), annsImgIds.end());
    std::vector<int64_t> img_ids = getAllImgIds();
    std::set<int64_t> gt_img_ids;
    gt_img_ids.insert(img_ids.begin(), img_ids.end());
    std::set<int64_t> intersection;
    std::set_intersection(res_img_ids.begin(), res_img_ids.end(),
                          gt_img_ids.begin(), gt_img_ids.end(),
                          std::inserter(intersection, intersection.begin()));
    return res_img_ids == intersection;
}


COCO COCO::loadRes(const std::string &resFile) {
    COCO res = COCO();
    // copy images related data
    res.images = images;
    res.images_map = images_map;
    fmt::print("[INFO] Loading and preparing results...\n");
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream annotation_file(resFile);
    json annotation_list = json::parse(annotation_file);
    std::vector<Annotation>& anns = res.annotations;
    for (auto& ann : annotation_list) {
        anns.emplace_back(ann);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    fmt::print("Get all anns Done (t={})\n", ms);
    start = std::chrono::high_resolution_clock::now();
    std::vector<int64_t> annsImgIds;
    for (auto& ann : anns) {
        annsImgIds.emplace_back(ann.image_id);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    fmt::print("Get all ann ids Done (t={})\n", ms);
    start = std::chrono::high_resolution_clock::now();
    if (!checkImgIds(annsImgIds)) {
        fmt::print("[ERROR] Results do not correspond to current coco set\n");
        std::terminate();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    fmt::print("Check img ids Done (t={})\n", ms);

    start = std::chrono::high_resolution_clock::now();

    res.categories = categories;
    res.categories_map = categories_map;
    std::map<int64_t, int64_t>& anns_map = res.annotations_map;
    int64_t i = 0;
    for (auto& ann : anns) {
        auto& bb = ann.bbox;
        float x1 = bb[0], x2 = bb[0] + bb[2], y1 = bb[1], y2 = bb[1] + bb[3];
        ann.segmentation = {{x1, y1, x1, y2, x2, y2, x2, y1}};
        ann.area = bb[2] * bb[3];
        ann.id = i + 1;
        ann.is_crowd = 0;
        anns_map[i + 1] = i;
        ++i;
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    fmt::print("Compute anns info Done (t={})\n", ms);

//    res.annotations = anns;
//    res.annotations_map = anns_map;
    res.createIndex();
    return res;
}