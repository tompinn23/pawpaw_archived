#pragma once

#include <stdint.h>

#include <string>
#include <vector>

namespace zip_internal {
    struct lfh {
        static constexpr int MAGIC = 0x04034b50;
        uint16_t version;
        uint16_t flag;
        uint16_t compression_method;
        uint16_t modtime;
        uint16_t moddate;
        uint32_t crc;
        uint64_t compressed_size;
        uint64_t uncompressed_size;
        std::string name;
        std::vector<uint8_t> extra;

        int from_file(FILE* file);
    };

    struct dd {
        static constexpr int MAGIC = 0x08074b50;
        uint32_t crc;
        uint64_t compressed_size;
        uint64_t uncompressed_size;
        int from_buffer(uint8_t* buff, size_t len);
    };

    struct cd_ent {
        cd_ent() : version(0), extract_version(0), flag(0),
            compression_method(0), modtime(0), moddate(0),
            crc(0), compressed_size(0), uncompressed_size(0),
            disk(0), int_attr(0), ext_attr(0), lfh_offset(0) {}
        static constexpr int MAGIC = 0x02014b50;
        uint16_t version;
        uint16_t extract_version;
        uint16_t flag;
        uint16_t compression_method;
        uint16_t modtime;
        uint16_t moddate;
        uint32_t crc;
        uint64_t compressed_size;
        uint64_t uncompressed_size;
        std::string name;
        std::vector<uint8_t> extra;
        std::vector<uint8_t> comment;
        uint32_t disk;
        uint16_t int_attr;
        uint32_t ext_attr;
        uint64_t lfh_offset;
        
        int from_file(FILE* file);
    };
    struct z64_cd_end {
        static constexpr int MAGIC = 0x06064b50;
        uint64_t size;
        uint16_t version;
        uint16_t version_extract;
        uint32_t disk;
        uint32_t start_disk;
        uint64_t disk_amount;
        uint64_t amount;
        uint64_t cd_size;
        uint64_t cd_offset;
        std::vector<uint8_t> comment;

        int from_file(FILE* file);
    };

    struct z64_cd_loc {
        static constexpr int MAGIC = 0x07064b50;
        uint32_t disk;
        uint64_t offset;
        uint32_t disks;
        int from_file(FILE* file);
    };

    struct cd_end {
        static constexpr int MAGIC = 0x06054b50;
        uint16_t disk;
        uint16_t cd_disk;
        uint16_t entries;
        uint16_t total_entries;
        uint32_t size;
        uint32_t offset;
        std::vector<uint8_t> comment;
        int from_file(FILE* file);
    };
}

enum class ZipParseError {
    SUCCESS = 0,
    MORE_DATA = -1,
    ERROR = -2
};

enum class FileType {
    Regular,
    Directory,
    Link
};

enum class CompressionType {
    Unknown = -1,
    Stored = 0,
    Shrunk = 1,
    Deflate = 8,
    Deflate64 = 9,
    Bzip2 = 12,
    Lzma = 14,
    Zstd = 93
};

class Zip {
public:
    class FileEntry {
    public:
        FileEntry(FileType mode, std::string name, int index)
            : mode(mode), name(name), index(index) {}
        std::string& Name() {
            return name;
        }
        FileType Type() {
            return mode;
        }
        bool IsEmpty() {
            return index == -1;
        }
        
    private:
        FileType mode;
        std::string name;
        int index;
    };

    Zip(std::string path) : path(path) {}
    int Open();
    FileEntry Find(std::string path);
    
private:

    std::string path;
    std::vector<zip_internal::cd_ent> central_directory;
    std::vector<zip_internal::lfh> local_file_headers;
};