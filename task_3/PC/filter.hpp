#ifndef POINTCLOUD_PREPROCESSOR__FILTER_HPP_
#define POINTCLOUD_PREPROCESSOR__FILTER_HPP_

#include "Logger.h"
#include "PointCloud.h"

#include <string>
#include <cmath>
#include <unordered_map>
#include <memory>

namespace pointcloud_preprocessor {
    class FilterParameter {
    public:
        FilterParameter() = default;

        explicit FilterParameter(std::unordered_map<std::string, double> params) : params_(std::move(params)) {
        }

        void ChangeParam(const std::string &param_name, double param_val) {
            params_[param_name] = param_val;
        }

        double GetParam(const std::string &param_name) const {
            return params_.at(param_name);
        }

        double GetParam(const std::string &param_name, double default_val) const noexcept {
            auto it = params_.find(param_name);
            if (it == params_.end())
                return default_val;

            return it->second;
        }

    private:
        std::unordered_map<std::string, double> params_;
    };

    /** \brief @b Filter represents the base filter class. */
    class Filter {
    public:
        explicit Filter(const std::string &filter_name = "pointcloud_preprocessor_filter")
            : filter_name_(filter_name), logger_(filter_name_) {
        }

        virtual ~Filter() = default;

        const std::string &GetFilterName() const { return filter_name_; }

        double GetDistance(const PointCloud &pc, size_t index) const {
            if (pc.pointcloud_type_ == "XYZIR") {
                double x = pc.points_[index * pc.point_size_ + 0];
                double y = pc.points_[index * pc.point_size_ + 1];
                double z = pc.points_[index * pc.point_size_ + 2];
                return std::hypot(x, y, z);
            }
            return pc.points_[index * pc.point_size_ + 5];
        }

        double GetAzimuth(const PointCloud &pc, size_t index) const {
            if (pc.pointcloud_type_ == "XYZIR") {
                double x = pc.points_[index * pc.point_size_ + 0];
                double y = pc.points_[index * pc.point_size_ + 1];
                return std::atan2(y, x);
            }
            return pc.points_[index * pc.point_size_ + 6];
        }

        virtual std::unique_ptr<PointCloud> Apply(const PointCloud &pc) = 0;

        virtual void SetParams(const FilterParameter &param) {
            params_ = param;
        }

        const FilterParameter &GetFilterParam() const {
            return params_;
        }

    protected:
        std::string filter_name_;
        Logger logger_;
        FilterParameter params_;
    };
} // namespace pointcloud_preprocessor

#endif  // POINTCLOUD_PREPROCESSOR__FILTER_HPP_
