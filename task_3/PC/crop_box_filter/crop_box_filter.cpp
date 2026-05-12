#include "crop_box_filter.hpp"

#include <cmath>
#include <string>

namespace pointcloud_preprocessor {
    CropBoxFilter::CropBoxFilter()
        : Filter("CropBoxFilter") {
    }

    std::unique_ptr<PointCloud> CropBoxFilter::Apply(const PointCloud &pc) {
        auto output_pc = std::make_unique<PointCloud>();
        output_pc->pointcloud_type_ = pc.pointcloud_type_;
        output_pc->point_size_ = pc.point_size_;

        output_pc->points_.reserve(pc.points_.size());

        size_t output_points_count = 0;

        for (size_t i = 0; i + 2 < pc.points_.size(); i += pc.point_size_) {
            double x = pc.points_[i + 0];
            double y = pc.points_[i + 1];
            double z = pc.points_[i + 2];

            if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
                logger_.log("Ignoring point containing NaN values");
                continue;
            }

            bool point_is_inside = (x > param_.min_x && x < param_.max_x) &&
                                   (y > param_.min_y && y < param_.max_y) &&
                                   (z > param_.min_z && z < param_.max_z);

            if (point_is_inside != param_.negative) {
                for (size_t j = 0; j < pc.point_size_; ++j) {
                    output_pc->points_.push_back(pc.points_[i + j]);
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
