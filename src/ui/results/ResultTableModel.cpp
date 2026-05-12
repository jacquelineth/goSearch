// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "ResultTableModel.hpp"

#include <chrono>
#include <format>

namespace javatb {

ResultTableModel::ResultTableModel(QObject* parent) : QAbstractTableModel(parent) {}

int ResultTableModel::rowCount(const QModelIndex&) const { return static_cast<int>(results_.size()); }
int ResultTableModel::columnCount(const QModelIndex&) const { return COLUMN_COUNT; }

QVariant ResultTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    if (role != Qt::DisplayRole && role != Qt::UserRole) return {};

    auto row = index.row();
    if (row < 0 || row >= static_cast<int>(results_.size())) return {};
    const auto& elt = *results_[static_cast<std::size_t>(row)];

    if (role == Qt::UserRole) {
        // Return raw long long for SIZE/COMPRESSED_SIZE for sorting
        switch (index.column()) {
            case SIZE:            return QVariant(elt.getSize());
            case COMPRESSED_SIZE: return QVariant(elt.getCompressedSize());
            default: break;
        }
    }

    switch (index.column()) {
        case PATH:            return QString::fromStdString(elt.toString());
        case SIZE:            return formatSize(elt.getSize());
        case COMPRESSED_SIZE: return formatSize(elt.getCompressedSize());
        case DATE:            return formatDate(elt);
        case ATTRIBUTES:      return QString::fromStdString(elt.getAttributes());
        default:              return {};
    }
}

QVariant ResultTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case PATH:            return "Path";
        case SIZE:            return "Size";
        case COMPRESSED_SIZE: return "Compressed";
        case DATE:            return "Date";
        case ATTRIBUTES:      return "Attr";
        default:              return {};
    }
}

void ResultTableModel::addElement(std::shared_ptr<SearchElement> element) {
    int row = static_cast<int>(results_.size());
    beginInsertRows({}, row, row);
    results_.push_back(std::move(element));
    endInsertRows();
}

std::shared_ptr<SearchElement> ResultTableModel::elementAt(int row) const {
    if (row < 0 || row >= static_cast<int>(results_.size())) return {};
    return results_[static_cast<std::size_t>(row)];
}

void ResultTableModel::clear() {
    beginResetModel();
    results_.clear();
    endResetModel();
}

QString ResultTableModel::formatSize(long long bytes) {
    if (bytes < 0) return {};
    static constexpr double KB = 1024.0;
    static constexpr double MB = KB * 1024.0;
    static constexpr double GB = MB * 1024.0;

    if (bytes < static_cast<long long>(KB))
        return QString::fromStdString(std::format("{} b", bytes));
    else if (bytes < static_cast<long long>(MB))
        return QString::fromStdString(std::format("{:.1f} k", bytes / KB));
    else if (bytes < static_cast<long long>(GB))
        return QString::fromStdString(std::format("{:.1f} m", bytes / MB));
    else
        return QString::fromStdString(std::format("{:.1f} g", bytes / GB));
}

QString ResultTableModel::formatDate(const SearchElement& elt) {
    if (!elt.hasLastModified()) return {};
    auto tp = elt.getLastModified();
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &tt);
#else
    localtime_r(&tt, &tm_buf);
#endif
    return QString::fromStdString(std::format(
        "{:04d}/{:02d}/{:02d} {:02d}:{:02d}:{:02d}",
        tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
        tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec));
}

} // namespace javatb
