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

struct Annotation {
    std::vector<std::vector<float>> segmentation;
    float area;
    int is_crowd;
    int ignore;
    int64_t image_id;
    std::vector<float> bbox;
    int64_t category_id;
    int64_t id;
};

struct Image {
    int license;
    std::string file_name;
    std::string coco_url;
    int height;
    int width;
    std::string date_captured;
    std::string flickr_url;
    int64_t id;
};


struct Category {
    std::string super_category;
    int64_t id;
    std::string name;
};


void to_json(json& j, const Annotation& a);
void from_json(const json& j, Annotation& a);
void to_json(json& j, const Image& a);
void from_json(const json& j, Image& a);
void to_json(json& j, const Category& a);
void from_json(const json& j, Category& a);

class COCO {
public:
    explicit COCO(const std::string& annotation_file_path = "");
    void createIndex();
    void info();
    std::vector<int64_t> getAnnIds(const std::vector<int64_t> &img_ids = {}, const std::vector<int64_t> &cat_ids = {},
                                   const std::vector<float> &area_rng = {}, int is_crowd = -1);
    std::vector<int64_t> getCatIds(const std::vector<std::string> &cat_names = {},
                                   const std::vector<std::string> &super_category_names = {},
                                   const std::vector<int64_t> &cat_ids = {});
    std::vector<int64_t> getImgIds(const std::vector<int64_t> &img_ids = {}, const std::vector<int64_t> &cat_ids = {});
    std::vector<Annotation> loadAnns(const std::vector<int64_t> &ann_ids = {});
    std::vector<Category> loadCats(const std::vector<int64_t> &cat_ids = {});
    std::vector<Image> loadImgs(const std::vector<int64_t> &img_ids = {});
    void showAnns();    // TODO: implements function
    COCO loadRes(const std::string &resFile);

private:
    json dataset;
    std::vector<Annotation> annotations;
    std::vector<Image> images;
    std::vector<Category> categories;
    std::map<int64_t, std::vector<int64_t>> img2ann;    // real id -> real id
    std::map<int64_t, std::vector<int64_t>> cat2img;    // real id -> real id
    std::map<int64_t, int64_t> annotations_map; // real id -> logical id
    std::map<int64_t, int64_t> images_map;  // real id -> logical id
    std::map<int64_t, int64_t> categories_map;  // real id -> logical id

    std::vector<int64_t> getAllAnnIds();
    std::vector<int64_t> getAllCatIds();
    std::vector<int64_t> getAllImgIds();
    bool checkImgIds(const std::vector<int64_t> &annsImgIds);
};

#endif //COCOTOOLS_COCO_H
