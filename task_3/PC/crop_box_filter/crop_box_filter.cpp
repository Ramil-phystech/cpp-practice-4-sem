#include "crop_box_filter.hpp"

#include <cmath>
#include <string>

namespace pointcloud_preprocessor {
    CropBoxFilter::CropBoxFilter()
        : Filter("CropBoxFilter") {
    }

    std::unique_ptr<PointCloud> CropBoxFilter::Apply(const PointCloud &pc) {
        auto output_pc = std::make_unique<PointCloud>();
        output_pc->Init(pc.type_, 0);

        output_pc->points_.reserve(pc.points_.size());

        size_t output_points_count = 0;

        for (size_t i = 0; i < pc.size_; ++i) {
            double x = pc.GetX(i);
            double y = pc.GetY(i);
            double z = pc.GetZ(i);

            if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
                logger_.log("Ignoring point containing NaN values");
                continue;
            }

            bool point_is_inside = (x > param_.min_x && x < param_.max_x) &&
                                   (y > param_.min_y && y < param_.max_y) &&
                                   (z > param_.min_z && z < param_.max_z);

            if (point_is_inside != param_.negative) {
                size_t offset = i * pc.layout_.point_size;
                for (size_t j = 0; j < pc.layout_.point_size; ++j) {
                    output_pc->points_.push_back(pc.points_[offset + j]);
                }
                output_points_count++;
            }
        }

        output_pc->size_ = output_points_count;
        return output_pc;
    }

    void CropBoxFilter::SetParams(const FilterParameter &param) {
        CropBoxParam new_param{};

        new_param.min_x = param.GetParam("min_x", param_.min_x);
        new_param.max_x = param.GetParam("max_x", param_.max_x);
        new_param.min_y = param.GetParam("min_y", param_.min_y);
        new_param.max_y = param.GetParam("max_y", param_.max_y);
        new_param.min_z = param.GetParam("min_z", param_.min_z);
        new_param.max_z = param.GetParam("max_z", param_.max_z);
        new_param.negative = static_cast<bool>(param.GetParam("negative", param_.negative ? 1.0 : 0.0));

        if (param_.min_x != new_param.min_x || param_.max_x != new_param.max_x ||
            param_.min_y != new_param.min_y || param_.max_y != new_param.max_y ||
            param_.min_z != new_param.min_z || param_.max_z != new_param.max_z ||
            param_.negative != new_param.negative) {
            logger_.log("[paramCallback] Setting the minimum point to: " +
                        std::to_string(new_param.min_x) + " " + std::to_string(new_param.min_y) + " " + std::to_string(
                            new_param.min_z));
            logger_.log("[paramCallback] Setting the maximum point to: " +
                        std::to_string(new_param.max_x) + " " + std::to_string(new_param.max_y) + " " + std::to_string(
                            new_param.max_z));

            logger_.log(
                std::string("[paramCallback] Setting the filter negative flag to: ") + (new_param.negative
                        ? "true"
                        : "false"));

            param_ = new_param;
        }
        Filter::SetParams(param);
    }
} // namespace pointcloud_preprocessor
