#pragma once

#include <vector>

#include "Direction.hxx"
#include "BlockPos.hxx"


class Block;

struct ChunkPos {
    ChunkPos(const long x, const long z) : x(x), z(z) {}
    const long x;
    const long z;

    bool operator==(const ChunkPos& other) {
        return x == other.x && z == other.z;
    }

    ChunkPos operator+(const Direction d) {
        switch(d) {
            case Direction::North:
                return ChunkPos(x + 1, z);
            case Direction::East:
                return ChunkPos(x, z + 1);
            case Direction::West:
                return ChunkPos(x, z - 1);
            case Direction::South:
                return ChunkPos(x - 1, z);
        }
        return ChunkPos(x, z);
    }
};

/**
 * A chunk is a section of world, it contains behaviour for loading and saving
 * as well as storing current world data. Chunk classes shouldn't contain specific
 * code for e.g. rendering.
*/
class Chunk {
public:
    Chunk(long x, long z) : pos(ChunkPos(x,z)) {}
    void SetBlock(int x, int y, int z, Block block);
    void SetBlock(BlockPos pos, Block block);

    bool Save();
    void Load();
private:
    ChunkPos pos;
    std::vector<Block*> blocks;
};