#pragma once

struct BlockPos {
    const long x;
    const long y;
    const long z;

    BlockPos(const long x, const long y, const long z) 
        : x(x), y(y), z(z) {}
};

