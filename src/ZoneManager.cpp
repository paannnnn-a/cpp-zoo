#include "ZoneManager.h"

#include <fstream>
#include <set>
#include <stdexcept>
#include <utility>

bool ZoneManager::zoneExists(const std::string& id) const {
    for (const auto& zone : zones) {
        if (zone.getId() == id) return true;
    }
    return false;
}

bool ZoneManager::addZone(const Zone& zone) {
    lastError.clear();
    if (zoneExists(zone.getId())) {
        lastError = "ZoneID 重复：" + zone.getId();
        return false;
    }
    zones.push_back(zone);
    return true;
}

std::vector<ZoneInfo> ZoneManager::getAllZones() const {
    std::vector<ZoneInfo> result;
    result.reserve(zones.size());
    for (const auto& zone : zones) result.push_back(zone.toInfo());
    return result;
}

bool ZoneManager::getZoneInfo(const std::string& id, ZoneInfo& outInfo) const {
    for (const auto& zone : zones) {
        if (zone.getId() == id) {
            outInfo = zone.toInfo();
            return true;
        }
    }
    return false;
}

const std::string& ZoneManager::getLastError() const { return lastError; }

bool ZoneManager::loadFromFile(const std::string& filename) {
    lastError.clear();
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        lastError = "无法打开园区文件：" + filename;
        return false; // 不自动创建示例，避免把缺失数据误当作真实园区。
    }

    // 临时容器：任何一行失败都不覆盖之前的完整列表。
    std::vector<Zone> loaded;
    std::set<std::string> ids;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        // 兼容 Windows CRLF，以及某些 Windows 编辑器写出的 UTF-8 BOM。
        if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        const auto first = line.find('|');
        const auto second = first == std::string::npos ? std::string::npos : line.find('|', first + 1);
        if (first == std::string::npos || second == std::string::npos ||
            line.find('|', second + 1) != std::string::npos) {
            lastError = "第 " + std::to_string(lineNumber) + " 行：必须恰好为 ZoneID|ZoneName|Description";
            return false;
        }
        const auto id = line.substr(0, first);
        try {
            Zone zone(id, line.substr(first + 1, second - first - 1), line.substr(second + 1));
            if (!ids.insert(id).second) {
                lastError = "第 " + std::to_string(lineNumber) + " 行：重复 ZoneID " + id;
                return false;
            }
            loaded.push_back(std::move(zone));
        } catch (const std::invalid_argument& error) {
            lastError = "第 " + std::to_string(lineNumber) + " 行：" + error.what();
            return false;
        }
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        lastError = "读取园区文件失败：" + filename;
        return false;
    }
    zones.swap(loaded); // 全部验证通过后一次提交；重复加载不会累加园区。
    return true;
}

bool ZoneManager::saveToFile(const std::string& filename) const {
    lastError.clear();
    std::ofstream output(filename, std::ios::binary | std::ios::trunc);
    if (!output) {
        lastError = "无法写入园区文件（请检查路径或权限）：" + filename;
        return false;
    }
    for (const auto& zone : zones) {
        output << zone.getId() << '|' << zone.getName() << '|' << zone.getDescription() << '\n';
    }
    // 必须检查 close 后的状态，不能只检查文件是否成功打开。
    output.close();
    if (!output) {
        lastError = "园区文件写入失败：" + filename;
        return false;
    }
    return true;
}
