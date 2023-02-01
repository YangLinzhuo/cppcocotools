//
// Created by YangLinzhuo on 2023/1/29.
//

#include "cocoeval.h"
#include <vector>
#include <fmt/core.h>

#include <iostream>
#include <vector>
#include <algorithm>

template<typename T>
std::vector<double> linspace(T start_in, T end_in, int num_in, bool endpoint)
{

    std::vector<double> linspaced;

    auto start = static_cast<double>(start_in);
    auto end = static_cast<double>(end_in);
    auto num = static_cast<double>(num_in);

    if (num == 0) { return linspaced; }
    if (num == 1)
    {
        linspaced.push_back(start);
        return linspaced;
    }

    double delta = (end - start) / (num - 1);

    for(int i=0; i < num-1; ++i)
    {
        linspaced.push_back(start + delta * i);
    }
    if (endpoint) {
        linspaced.push_back(end);
    }
//    linspaced.push_back(end); // I want to ensure that start and end
    // are exactly the same as the input
    return linspaced;
}


struct EvaluateImgResult {
    int64_t imgId;
    int64_t catId;
    double aRng;
    int maxDet;
    std::vector<int64_t> dtIds;
    std::vector<int64_t> gtIds;
    // dtm
    // gtm
    // dtScores
    // gtIgnore
    // dtIgnore
};


struct Params {
    Params(const std::string& iouType_) {
        if (iouType_ != "bbox") {
            fmt::print("iouType not supported.\n");
            return;
        }
        iouThrs = linspace(0.5, 0.95, static_cast<int>(round((0.95 - 0.5) / 0.05)), true);
        recThrs = linspace(0.0, 1.0, static_cast<int>(round((1.0 - 0.0) / 0.01)), true);
        maxDets = {1, 10, 100};
        areaRng = {{0 * 0, 1e5 * 1e5}, {0 * 0, 32 * 32}, {32 * 32, 96 * 96}, {96 * 96, 1e5 * 1e5}};
        areaRngLbl = {"all", "small", "medium", "large"};
        useCats = 1;
        iouType = iouType_;
    }

    std::vector<int64_t> imgIds;
    std::vector<int64_t> catIds;
    std::vector<double> iouThrs;
    std::vector<double> recThrs;
    std::vector<int> maxDets;
    std::vector<std::vector<float>> areaRng;
    std::vector<std::string> areaRngLbl;
    std::string iouType;
    int useCats;
//    std::vector<float> kpt_oks_sigma;
};

class COCOeval {
public:
    COCOeval(COCO& cocoGt_, COCO& cocoDt_, const std::string iouType) : cocoGt(cocoGt_), cocoDt(cocoDt_),
    params(Params(iouType)) {
        params.imgIds = cocoGt.getImgIds();
        params.catIds = cocoGt.getCatIds();
        std::sort(params.imgIds.begin(), params.imgIds.end());
        std::sort(params.catIds.begin(), params.catIds.end());
    }
    void evaluate();
private:
    COCO &cocoGt;   // ground truth COCO API
    COCO &cocoDt;   // detections COCO API
    Params params;
    std::map<std::pair<int64_t, int64_t>, std::vector<Annotation>> __gts;
    std::map<std::pair<int64_t, int64_t>, std::vector<Annotation>> __dts;

    void prepare();
    void compute_iou(int64_t img_id, int64_t cat_id);
};

void COCOeval::prepare() {
    /*
    Prepare ._gts and ._dts for evaluation based on params
    :return: None
    */
    std::vector<Annotation> gts;
    std::vector<Annotation> dts;
    if (params.useCats) {
        gts = cocoGt.loadAnns(cocoGt.getAnnIds(params.imgIds, params.catIds));
        dts = cocoDt.loadAnns(cocoDt.getAnnIds(params.imgIds, params.catIds));
    } else {
        gts = cocoGt.loadAnns(cocoGt.getAnnIds(params.imgIds));
        dts = cocoDt.loadAnns(cocoDt.getAnnIds(params.imgIds));
    }

    // Set ignore flag
    for (auto& gt : gts) {
        gt.ignore = gt.is_crowd;
    }

    for (auto& gt : gts) {
        __gts[std::make_pair(gt.image_id, gt.category_id)].emplace_back(gt);
    }
    for (auto& dt : dts) {
        __dts[std::make_pair(dt.image_id, dt.category_id)].emplace_back(dt);
    }
}


void COCOeval::evaluate() {
    auto start = std::chrono::high_resolution_clock::now();
    fmt::print("Running per image evaluation...");
    fmt::print("Evaluate annotation type *{}*", params.iouType);
    auto& imgIds = params.imgIds;
    // Remove duplicate img ids
    auto last_img_it = std::unique(imgIds.begin(), imgIds.end());
    imgIds.erase(last_img_it, imgIds.end());
    // Remove duplicate category ids
    if (params.useCats) {
        auto& catIds = params.catIds;
        auto last_cat_it = std::unique(catIds.begin(), catIds.end());
        catIds.erase(last_cat_it, catIds.end());
    }
    std::sort(params.maxDets.begin(), params.maxDets.end());

    prepare();
    auto& catIds = params.catIds;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
//    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
//    fmt::print("Done (t={:.2f}ms)\n", duration.count());
    fmt::print("Done (t={})\n", ms);
}


void COCOeval::compute_iou(int64_t img_id, int64_t cat_id) {
    auto& gt = __gts[std::make_pair(img_id, cat_id)];
    auto& dt = __dts[std::make_pair(img_id, cat_id)];
    if (gt.empty() and dt.empty()) {
        return;
    }
}