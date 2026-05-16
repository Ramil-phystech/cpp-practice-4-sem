#ifndef POINTCLOUD_H_
#define POINTCLOUD_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>

enum class PointType {
    XYZIR,
    XYZIRDAT
};

inline std::string ToString(PointType type) {
    switch (type) {
        case PointType::XYZIR: return "XYZIR";
        case PointType::XYZIRDAT: return "XYZIRDAT";
        default: return "Unknown";
    }
}

struct PointLayout {
    size_t point_size;
    size_t x_offset;
    size_t y_offset;
    size_t z_offset;
    int distance_offset;
    int azimuth_offset;
};

inline PointLayout GetPointLayout(PointType type) {
    switch (type) {
        case PointType::XYZIR:
            return {5, 0, 1, 2, -1, -1};
        case PointType::XYZIRDAT:
            return {8, 0, 1, 2, 5, 6};
        default:
            throw std::invalid_argument("Unknown PointType");
    }
}

class PointCloud {
public:
    void Init(PointType pc_type, size_t points_count) {
        type_ = pc_type;
        layout_ = GetPointLayout(type_);
        size_ = points_count;
        points_.assign(size_ * layout_.point_size, 0.0);
    }

    void AddPoint(const std::vector<double> &point, size_t ind) {
        if (point.size() != layout_.point_size) {
            throw std::runtime_error("Point has unsupported size for type " + ToString(type_));
        }

        size_t offset = layout_.point_size * ind;
        for (size_t i = 0; i < layout_.point_size; ++i) {
            points_[offset + i] = point[i];
        }
    }

    double GetX(size_t index) const {
        return points_[index * layout_.point_size + layout_.x_offset];
    }

    double GetY(size_t index) const {
        return points_[index * layout_.point_size + layout_.y_offset];
    }

    double GetZ(size_t index) const {
        return points_[index * layout_.point_size + layout_.z_offset];
    }

    double GetDistance(size_t index) const {
        if (layout_.distance_offset != -1) {
            return points_[index * layout_.point_size + layout_.distance_offset];
        }
        return std::hypot(GetX(index), GetY(index), GetZ(index));
    }

    double GetAzimuth(size_t index) const {
        if (layout_.azimuth_offset != -1) {
            return points_[index * layout_.point_size + layout_.azimuth_offset];
        }
        return std::atan2(GetY(index), GetX(index));
    }

    PointType type_{PointType::XYZIR};
    PointLayout layout_{0, 0, 0, 0, -1, -1};
    size_t size_ = 0;
    std::vector<double> points_;
};

// For tests
inline void FillPointCloud(PointCloud *pc, size_t points_count, PointType point_type, const std::vector<double> &data) {
    if (!pc) return;

    pc->Init(point_type, points_count);
    std::vector<double> temp_point(pc->layout_.point_size);

    for (size_t ind = 0; ind < points_count; ++ind) {
        size_t offset = ind * pc->layout_.point_size;
        for (size_t i = 0; i < pc->layout_.point_size; ++i) {
            temp_point[i] = data[offset + i];
        }
        pc->AddPoint(temp_point, ind);
    }
}

#endif //POINTCLOUD_H_
