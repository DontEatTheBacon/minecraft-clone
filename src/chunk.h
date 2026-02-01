#ifndef CHUNK_H
#define CHUNK_H

#include <iostream>
#include <array>
#include <glm/glm.hpp>
#include "PerlinNoise.hpp"

namespace Random
{
    inline const unsigned seed = 1;

    inline const siv::PerlinNoise perlin { seed };
}

namespace ChunkDimensions
{
    inline constexpr int WIDTH { 16 };
    inline constexpr int DEPTH { 16 };
    inline constexpr int HEIGHT { 255 };
}

class Block
{
private:
    // texture atlas coordinates
    glm::vec2 atlasCoord {};

    bool render {};

public:
    // coordinates relative to chunk
    float x {};
    float y {};
    float z {};

    bool isRendered() const
    {
        return render;
    }

    void setRenderBit(bool b) 
    {
        render = b;
    }

    void setAtlasCoord(int x, int y)
    {
        atlasCoord.x = static_cast<float>(x);
        atlasCoord.y = static_cast<float>(y);
    }

    const glm::vec2& getAtlasCoord() const
    {
        return atlasCoord;
    }

    Block()
    {
    }
};

class Chunk
{
private:
    std::array<Block, ChunkDimensions::WIDTH * ChunkDimensions::DEPTH * ChunkDimensions::HEIGHT> m_blocks {};

    glm::vec2 m_chunkCoord {};

public:
    Chunk()
    {
        for (int x = 0; x < ChunkDimensions::WIDTH; x++)
        {
            for (int y = 0; y < ChunkDimensions::HEIGHT; y++)
            {
                for (int z = 0; z < ChunkDimensions::DEPTH; z++)
                {
                    Block& block = m_blocks[x + (y * ChunkDimensions::WIDTH) + (z * ChunkDimensions::WIDTH * ChunkDimensions::HEIGHT)];
                    block.x = static_cast<float>(x);
                    block.y = static_cast<float>(y);
                    block.z = static_cast<float>(z);

                    double noiseVal { Random::perlin.noise3D_01(x / 16.0f, y / 16.0f, z / 16.0f) };

                    // if block above
                    if (Random::perlin.noise3D_01(x / 16.0f, (y + 1.0f) / 16.0f, z / 16.0f) > 0.5)
                    {
                        block.setAtlasCoord(21, 2);
                    } 
                    else
                    {
                        block.setAtlasCoord(22, 10);
                    }

                    if (noiseVal > 0.5)
                    {
                        block.setRenderBit(true);
                    }
                }
            }
        }
    }

    int size() const
    {
        return ChunkDimensions::WIDTH * ChunkDimensions::DEPTH * ChunkDimensions::HEIGHT;
    }

    const std::array<Block, ChunkDimensions::WIDTH * ChunkDimensions::DEPTH * ChunkDimensions::HEIGHT>& blocks() const
    {
        return m_blocks;
    } 

    const Block& get(int x, int y, int z) const
    {
        int index { x + (y * ChunkDimensions::WIDTH) + (z * ChunkDimensions::WIDTH * ChunkDimensions::HEIGHT) };
        return m_blocks[index];
    }
};

#endif