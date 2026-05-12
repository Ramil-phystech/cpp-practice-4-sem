#ifndef POINTCLOUD_H_
#define POINTCLOUD_H_

#include <string>
#include <stdexcept>
#include <vector>

class PointCloud {
public:
    void Init(std::string pc_type, size_t points_count) {
        pointcloud_type_ = std::move(pc_type);
        size_ = points_count;

        if (pointcloud_type_ == "XYZIR") {
            point_size_ = 5;
        } else if (pointcloud_type_ == "XYZIRDAT") {
            point_size_ = 8;
        } else {
            throw std::runtime_error("Unknown pointcloud type: " + pointcloud_type_);
        }

        points_.assign(size_ * point_size_, 0.0);
    }

    void AddPoint(const std::vector<double>& point, size_t ind) {
        if (point.size() != point_size_) {
            throw std::runtime_error("Point has unsupported size = " +
                std::to_string(point.size()) + " for type " + pointcloud_type_);
        }

        size_t offset = point_size_ * ind;
        for (size_t i = 0; i < point_size_; ++i) {
            points_[offset + i] = point[i];
        }
    }

    std::string pointcloud_type_{""};
    size_t size_ = 0;
    size_t point_size_ = 0;
    std::vector<double> points_;
};

// For tests
inline void FillPointCloud(PointCloud* pc, size_t points_count, const std::string& point_type, const std::vector<double>& data) {
    if (!pc) return;

    pc->Init(point_type, points_count);

    std::vector<double> temp_point(pc->point_size_);

    for (size_t ind = 0; ind < points_count; ++ind) {
        size_t offset = ind * pc->point_size_;
        for (size_t i = 0; i < pc->point_size_; ++i) {
            temp_point[i] = data[offset + i];
        }
        pc->AddPoint(temp_point, ind);
    }
}

#endif //POINTCLOUD_H_
