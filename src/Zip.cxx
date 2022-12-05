#include "Zip.hxx"
#include "boost/endian/conversion.hpp"

#include <boost/endian.hpp>

#include <filesystem>
#include <cstdio>
#include <stdio.h>

namespace zip_internal {
    using namespace boost;
    int lfh::from_file(FILE* file) {
        size_t offset = 0;
        unsigned char buf[256];
        size_t ret = fread(buf, 1, 4, file);
        if(ret != 4) {
            perror("Failed reading magic");
            return -1;
        }
        uint32_t magic = endian::load_little_u32(buf);
        if(magic != lfh::MAGIC) {
            return -1;
        }
        ret = fread(buf, 1, 26, file);
        if(ret != 26) {
            return -2;
        }
        version = endian::load_little_u16(buf);
        flag = endian::load_little_u16(buf + 2);
        compression_method = endian::load_little_u16(buf + 4);
        modtime = endian::load_little_u16(buf + 6);
        moddate = endian::load_little_u16(buf + 8);
        crc = endian::load_little_u32(buf + 10);
        compressed_size = endian::load_little_u32(buf + 14);
        uncompressed_size = endian::load_little_u32(buf + 18);
        size_t file_length = endian::load_little_u16(buf + 22);
        size_t extra_length = endian::load_little_u16(buf + 24);
        if(file_length > 0) {
            name.resize(file_length);
            ret = fread(name.data(), 1, file_length, file);
            if(feof(file))
                return -1;
        }
        if(extra_length > 0) {
            extra.resize(extra_length);
            ret = fread(extra.data(), 1, extra_length, file);
            if(feof(file))
                return -1;
        }
        // Check for zip64
        if(compressed_size == 0xFFFFFFFF || uncompressed_size == 0xFFFFFFFF) {
            size_t z64_off = 0;
            for(z64_off = 0; z64_off < extra.size(); z64_off += 2) {
                if(endian::load_little_u16(extra.data() + z64_off) == 0x0001) {
                    uint16_t sz = endian::load_little_u16(extra.data() + z64_off + 2);
                    switch(sz) {
                        case 16:
                            compressed_size = endian::load_little_u64(extra.data() + z64_off + 16);
                            uncompressed_size = endian::load_little_u64(extra.data() + z64_off + 8);
                            break;
                        case 8:
                            uncompressed_size = endian::load_little_u64(extra.data() + z64_off + 8);
                            break;
                    }
                }
            }
        }
        return 0;
    }

    int cd_ent::from_file(FILE* file) {
        size_t offset = 0; 
        unsigned char buf[256];
        size_t ret = fread(buf, 1, 4, file);
        if(ret != 4 || endian::load_little_u32(buf) != cd_ent::MAGIC) {
            return -1;
        }
        ret = fread(buf, 1, 42, file);
        if(ret != 42) {
            return -1;
        }
        version = endian::load_little_u16(buf);
        extract_version = endian::load_little_u16(buf + 2);
        flag = endian::load_little_u16(buf + 4);
        compression_method = endian::load_little_u16(buf + 6);
        modtime = endian::load_little_u16(buf + 8);
        moddate = endian::load_little_u16(buf + 10);
        crc = endian::load_little_u32(buf + 12);
        compressed_size = endian::load_little_u32(buf + 16);
        uncompressed_size = endian::load_little_u32(buf + 20);
        size_t file_length = endian::load_little_u16(buf + 24);
        size_t extra_length = endian::load_little_u16(buf + 26);
        size_t comment_length = endian::load_little_u16(buf + 28);
        disk = endian::load_little_u16(buf + 30);
        int_attr = endian::load_little_u16(buf + 32);
        ext_attr = endian::load_little_u32(buf + 34);
        lfh_offset = endian::load_little_u32(buf + 38);
        if(file_length > 0) {
            name.resize(file_length);
            ret = fread(name.data(), 1,file_length, file);
            if(feof(file))
                return -1;
        }
        if(extra_length > 0) {
            extra.resize(extra_length);
            ret = fread(extra.data(), 1, extra_length, file);
            if(feof(file))
                return -1;
        }
        if(comment_length > 0) {
            comment.resize(comment_length);
            ret = fread(comment.data(), 1, comment_length, file);
            if(feof(file)) {
                return -1;
            }
        }
        if(compressed_size == 0xFFFFFFFF || uncompressed_size == 0xFFFFFFFF || offset == 0xFFFFFFFF || disk == 0xFFFF) {
            size_t z64_off = 0;
            for(z64_off = 0; z64_off < extra.size(); z64_off += 2) {
                if(endian::load_little_u16(extra.data() + z64_off) == 0x0001) {
                    uint16_t sz = endian::load_little_u16(extra.data() + z64_off + 2);
                    switch(sz) {
                        case 28:
                            disk = endian::load_little_u32(extra.data() + z64_off + 28);
                            offset = endian::load_little_u64(extra.data() + z64_off + 20);
                            compressed_size = endian::load_little_u64(extra.data() + z64_off + 12);
                            uncompressed_size = endian::load_little_u64(extra.data() + z64_off + 4);
                        case 24:
                            offset = endian::load_little_u64(extra.data() + z64_off + 20);
                            compressed_size = endian::load_little_u64(extra.data() + z64_off + 12);
                            uncompressed_size = endian::load_little_u64(extra.data() + z64_off + 4);
                            break;
                        case 16:
                            compressed_size = endian::load_little_u64(extra.data() + z64_off + 12);
                            uncompressed_size = endian::load_little_u64(extra.data() + z64_off + 4);
                            break;
                        case 8:
                            uncompressed_size = endian::load_little_u64(extra.data() + z64_off + 4);
                            break;
                    }
                }
            }
        }
        return 0;
    }

    int z64_cd_end::from_file(FILE* file) {
        unsigned char buf[256];
        size_t ret = fread(buf, 1, 4, file);
        if(ret != 4 || endian::load_little_u32(buf) != z64_cd_end::MAGIC) {
            return -1;
        }
        ret = fread(buf, 1, 52, file);
        if(ret != 52) {
            return -1;
        }
        size = endian::load_little_u64(buf);
        size_t comment_length = size - 40;
        version = endian::load_little_u16(buf + 8);
        version_extract = endian::load_little_u16(buf + 10);
        disk = endian::load_little_u32(buf + 14);
        start_disk = endian::load_little_u32(buf + 18);
        disk_amount = endian::load_little_u64(buf + 22);
        amount = endian::load_little_u64(buf + 30);
        cd_size = endian::load_little_u64(buf + 38);
        cd_offset = endian::load_little_u64(buf + 46);
        if(comment_length > 0) {
            comment.resize(comment_length);
            ret = fread(comment.data(), 1, comment_length, file);
            if(feof(file)) {
                return -1;
            }
        }
        return 0;
    }

    int z64_cd_loc::from_file(FILE* file) {
        unsigned char buf[256];
        size_t ret = fread(buf, 1, 4, file);
        if(ret != 4 || endian::load_little_u32(buf) != z64_cd_loc::MAGIC) {
            return -1;
        }
        ret = fread(buf, 1, 16, file);
        if(ret != 16) {
            return -1;
        }
        disk = endian::load_little_u32(buf);
        offset = endian::load_little_u64(buf + 4);
        disks = endian::load_little_u32(buf + 8);
        return 0;
    }

    int cd_end::from_file(FILE* file) {
        unsigned char buf[256];
        size_t ret = fread(buf, 1, 4, file);
        if(ret != 4 || endian::load_little_u32(buf) != cd_end::MAGIC) {
            return -1;
        }
        ret = fread(buf, 1, 18, file);
        if(ret != 18) {
            return -1;
        }
        disk = endian::load_little_u16(buf);
        cd_disk = endian::load_little_u16(buf + 2);
        entries = endian::load_little_u16(buf + 4);
        total_entries = endian::load_little_u16(buf + 6);
        size = endian::load_little_u32(buf + 8);
        offset = endian::load_little_u32(buf + 12);
        size_t comment_length = endian::load_little_u16(buf + 16);
        if(comment_length > 0) {
            comment.resize(comment_length);
            ret = fread(comment.data(), 1, comment_length, file);
            if(ret != comment_length) {
                return -2;
            }
        }
        return 0;
    }
}

using namespace zip_internal;
using namespace boost;

int Zip::Open() {
    FILE* fp = fopen(path.c_str(), "rb");
    size_t size = std::filesystem::file_size(path);
    size_t ret = 0;
    unsigned char buf[4];
    memset(buf, 0, sizeof(buf));
    size_t end_cd = 0;
    for(size_t max = size - 4; max > 0; max--) {
        fseek(fp, max, SEEK_SET);
        fread(buf, sizeof(buf), 1, fp);
        if(boost::endian::load_little_u32(buf) == zip_internal::cd_end::MAGIC) {
            end_cd = max;
            break;
        }
    }
    cd_end end;
    fseek(fp, end_cd, SEEK_SET);
    end.from_file(fp);
    size_t z64_loc = 0;
    if(end.offset == 0xFFFFFFFF) {
        for(size_t max = end_cd - 4; max > 0; max--) {
            fseek(fp, max, SEEK_SET);
            fread(buf, sizeof(buf), 1, fp);
            if(boost::endian::load_little_u32(buf) == zip_internal::z64_cd_loc::MAGIC) {
                z64_loc = max;
                break;
            }
        }
        fseek(fp, z64_loc, SEEK_SET);
        z64_cd_loc loc;
        loc.from_file(fp);
        end_cd = loc.offset;
    }
    z64_cd_end end64;
    fseek(fp, end_cd, SEEK_SET);
    size_t count = 0;
    if(z64_loc != 0) {
        end64.from_file(fp);
        fseek(fp, end64.cd_offset, SEEK_SET);
        count = end64.amount;
    } else {
        fseek(fp, end.offset, SEEK_SET);
        count = end.entries;
    }
    for(size_t i = 0; i < count; i++) {
        cd_ent ent;
        ent.from_file(fp);
        central_directory.push_back(ent);
    }
    for(auto& a : central_directory) {
        fseek(fp, a.lfh_offset, SEEK_SET);
        lfh fh;
        fh.from_file(fp);
        local_file_headers.push_back(fh);
    }
    return 0;
}

Zip::FileEntry Zip::Find(std::string path) {
    size_t index = 0;
    for(auto &entry : central_directory) {
        if(path == entry.name) { 
            FileType mode = FileType::Regular;
            if(entry.name.ends_with("/")) {
                mode = FileType::Directory;
            }
            return Zip::FileEntry(mode, entry.name, index);
        }
        index++;
    }
    return Zip::FileEntry(FileType::Regular, "empty", -1);
}

