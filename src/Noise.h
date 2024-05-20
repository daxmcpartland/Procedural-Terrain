#include <vector>
#include <glm/glm.hpp>
#include <random>
#include <numeric>
#include <chrono>
class Noise {
private: 
    std::vector<int> p = { 151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
                      140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
                      247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                       57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                       74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                       60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                       65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                      200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                       52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                      207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                      119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
                      129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                      218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                       81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                      184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                      222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180 };
public:
    Noise() 
    {
        reseed();
    }
     float perlin_noise(float x, float y, int gridSize)
    {
        float value = 0;
        float frequency = 1;
        float amplitude = 1;
        float persistance = 1.2;
        float lacunarity = 1.4;
        for (int i = 0; i < 8; ++i) {
            value += perlin(x * frequency / gridSize, y * frequency / gridSize) * amplitude;

            frequency *= persistance;
            amplitude *= lacunarity;
        }
        //value *= 3.7;
        return value;
    }
private:
    float perlin(float x, float y, float z = .001)
    {
        int X = (int)x;
        int Y = (int)y;
        int Z = (int)z;

        int ix = X & 255;
        int iy = Y & 255;
        int iz = Z & 255;

        float dx = (x - X);
        float dy = (y - Y);
        float dz = (z - Z);

        float u = fade(dx);
        float v = fade(dy);
        float w = fade(dz);

        int A = (p[ix & 255] + iy) & 255;
        int B = (p[(ix + 1) & 255] + iy) & 255;

        int AA = (p[A] + iz) & 255;
        int AB = (p[(A + 1) & 255] + iz) & 255;

        int BA = (p[B] + iz) & 255;
        int BB = (p[(B + 1) & 255] + iz) & 255;

        float p0 = grad(p[AA], dx, dy, dz);
        float p1 = grad(p[BA], dx - 1, dy, dz);
        float p2 = grad(p[AB], dx, dy - 1, dz);
        float p3 = grad(p[BB], dx - 1, dy - 1, dz);
        float p4 = grad(p[(AA + 1) & 255], dx, dy, dz - 1);
        float p5 = grad(p[(BA + 1) & 255], dx - 1, dy, dz - 1);
        float p6 = grad(p[(AB + 1) & 255], dx, dy - 1, dz - 1);
        float p7 = grad(p[(BB + 1) & 255], dx - 1, dy - 1, dz - 1);

        float q0 = lerp(p0, p1, u);
        float q1 = lerp(p2, p3, u);
        float q2 = lerp(p4, p5, u);
        float q3 = lerp(p6, p7, u);

        float r0 = lerp(q0, q1, v);
        float r1 = lerp(q2, q3, v);

        return lerp(r0, r1, w);
    }

     float grad(int hash, float x, float y, float z)
    {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    void reseed() 
    {
        std::iota(p.begin(), p.end(), 0);
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto rng = std::default_random_engine(seed);
        std::shuffle(p.begin(), p.end(), rng);
    }

     float fade(float t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
     float lerp(float a, float b, float t)
    {
        return (a + (b - a) * t);
    }
};