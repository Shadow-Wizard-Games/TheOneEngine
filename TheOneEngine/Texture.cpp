#include "Texture.h"
#include <GL/glew.h>
#include <IL/il.h>
#include <gli/gli.hpp>
#include "Log.h"

#include <algorithm>

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32

Texture::Texture() : textureChannels(4), imageSize(1)
{
    //Blank texture

    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, GL_RGBA8, imageSize.x, imageSize.y);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::Texture(const std::string& newPath)
{
    path = newPath;
    if (newPath.ends_with(".dds") || newPath.ends_with(".DDS"))
        InitDDS(newPath);
    else
        Init(newPath);
}

Texture::~Texture() 
{
    if (textureID != uint32_t(-1)) {
        GLCALL(glDeleteTextures(1, &textureID));
    }
}

void Texture::Bind(uint32_t slot) const
{
    GLCALL(glBindTextureUnit(slot, textureID));
}

bool Texture::Init(const std::string& path)
{
    int w, h, ch;

    //load image data using devil
    unsigned int img = ilGenImage();
    std::string fixedPath(path.c_str());
    size_t index = fixedPath.find("\\");
    while (index != std::string::npos)
    {
        fixedPath.replace(index, 1, "/");
        index = fixedPath.find("\\", index + 1);
    }
    
    ilBindImage(img);
    if (ilLoadImage((const wchar_t*)fixedPath.c_str()) == IL_FALSE)
    {
        ilDeleteImage(img);
        return false;
    }

    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    ch = ilGetInteger(IL_IMAGE_CHANNELS);
    ILubyte* data = ilGetData();

    // Ajusta la alineación de pixel si es necesario (importante para imágenes RGB)
    if (ch == 3) {
        GLCALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    }


    GLenum format;
    switch (ch)
    {
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RGB;
        break;
    }

    //load image as a texture in VRAM
    GLCALL(glGenTextures(1, &textureID));
    GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, ch, w, h, 0, format, GL_UNSIGNED_BYTE, data));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    //now we can delete image from RAM
    ilDeleteImage(img);

    imageSize.x = w;
    imageSize.y = h;
    textureChannels = ch;

    return true;
}

bool Texture::InitDDS(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        LOG(LogType::LOG_ERROR, "Image at path %s doesn't exist", path);
        return false;
    }
    int w, h, ch;

    gli::texture texture = gli::load_dds(path);
    gli::texture::extent_type extent = texture.extent();
    w = extent.x;
    h = extent.y;
    ch = 4;

    if (texture.empty()) {
        return false;
    }

    GLCALL(glGenTextures(1, &textureID));
    GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));

    //GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, PicType, w, h, 0, PicType, GL_UNSIGNED_BYTE, texture.data());
    gli::gl gli(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = gli.translate(texture.format(), texture.swizzles());

    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, &Format.Swizzles[0]));

   GLCALL(glCompressedTexImage2D(GL_TEXTURE_2D, 0, Format.Internal, w, h, 0, (GLsizei)texture.size(), texture.data()));
   GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

    imageSize.x = w;
    imageSize.y = h;
    textureChannels = ch;

    return true;
}

bool Texture::CreateCheckerTexture()
{
    GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }

    GLCALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GLCALL(glGenTextures(1, &textureID));
    GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
        0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage));

    return true;
}

void Texture::SetData(void* data, uint32_t size)
{
    uint32_t bpp = textureChannels == 4 ? 4 : 3;
    assert(size == imageSize.x * imageSize.y * bpp, "Data must be entire texture!");
    GLCALL(glTextureSubImage2D(textureID, 0, 0, 0, imageSize.x, imageSize.y, textureChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data));
}

//============================== DDS MANAGEMENT ==============================
#include <stdio.h>
#include <memory.h>

#define STB_DXT_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4244) // conversion from 'int' to 'unsigned char', possible loss of data
#include "stb_dxt.h"
#pragma warning(pop)
#undef STB_DXT_IMPLEMENTATION

//
// we don't use much of the struct below, just need the correct header representation

#define DDSF_FOURCC 0x00000004l

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |   \
                ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)
#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))

typedef struct _DDCOLORKEY
{
    unsigned int       dwColorSpaceLowValue;
    unsigned int       dwColorSpaceHighValue;
} DDCOLORKEY;

typedef struct _DDPIXELFORMAT
{
    unsigned int       dwSize;                 // size of structure
    unsigned int       dwFlags;                // pixel format flags
    unsigned int       dwFourCC;               // (FOURCC code)
    union
    {
        unsigned int   dwRGBBitCount;          // how many bits per pixel
        unsigned int   dwYUVBitCount;          // how many bits per pixel
        unsigned int   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
        unsigned int   dwAlphaBitDepth;        // how many bits for alpha channels
        unsigned int   dwLuminanceBitCount;    // how many bits per pixel
        unsigned int   dwBumpBitCount;         // how many bits per "buxel", total
        unsigned int   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture format list and if DDPF_D3DFORMAT is set
    };
    union
    {
        unsigned int   dwRBitMask;             // mask for red bit
        unsigned int   dwYBitMask;             // mask for Y bits
        unsigned int   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        unsigned int   dwLuminanceBitMask;     // mask for luminance bits
        unsigned int   dwBumpDuBitMask;        // mask for bump map U delta bits
        unsigned int   dwOperations;           // DDPF_D3DFORMAT Operations
    };
    union
    {
        unsigned int   dwGBitMask;             // mask for green bits
        unsigned int   dwUBitMask;             // mask for U bits
        unsigned int   dwZBitMask;             // mask for Z bits
        unsigned int   dwBumpDvBitMask;        // mask for bump map V delta bits
        struct
        {
            unsigned short    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
            unsigned short    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
        } MultiSampleCaps;

    };
    union
    {
        unsigned int   dwBBitMask;             // mask for blue bits
        unsigned int   dwVBitMask;             // mask for V bits
        unsigned int   dwStencilBitMask;       // mask for stencil bits
        unsigned int   dwBumpLuminanceBitMask; // mask for luminance in bump map
    };
    union
    {
        unsigned int   dwRGBAlphaBitMask;      // mask for alpha channel
        unsigned int   dwYUVAlphaBitMask;      // mask for alpha channel
        unsigned int   dwLuminanceAlphaBitMask;// mask for alpha channel
        unsigned int   dwRGBZBitMask;          // mask for Z channel
        unsigned int   dwYUVZBitMask;          // mask for Z channel
    };
} DDPIXELFORMAT;

typedef struct _DDSURFACEDESC2
{
    unsigned int               dwSize;                 // size of the DDSURFACEDESC structure
    unsigned int               dwFlags;                // determines what fields are valid
    unsigned int               dwHeight;               // height of surface to be created
    unsigned int               dwWidth;                // width of input surface
    union
    {
        int					   lPitch;                 // distance to start of next line (return value only)
        unsigned int           dwLinearSize;           // Formless late-allocated optimized surface size
    };
    union
    {
        unsigned int           dwBackBufferCount;      // number of back buffers requested
        unsigned int           dwDepth;                // the depth if this is a volume texture 
    };
    union
    {
        unsigned int           dwMipMapCount;          // number of mip-map levels requestde
                                                // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
        unsigned int           dwRefreshRate;          // refresh rate (used when display mode is described)
        unsigned int           dwSrcVBHandle;          // The source used in VB::Optimize
    };
    unsigned int               dwAlphaBitDepth;        // depth of alpha buffer requested
    unsigned int               dwReserved;             // reserved
    unsigned int                lpSurface;              // pointer to the associated surface memory
    union
    {
        DDCOLORKEY			   ddckCKDestOverlay;      // color key for destination overlay use
        unsigned int           dwEmptyFaceColor;       // Physical color for empty cubemap faces
    };
    DDCOLORKEY                 ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY                 ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY                 ddckCKSrcBlt;           // color key for source blt use
    union
    {
        DDPIXELFORMAT		   ddpfPixelFormat;        // pixel format description of the surface
        unsigned int           dwFVF;                  // vertex format description of vertex buffers
    };

    struct _DDSCAPS2
    {
        unsigned int       dwCaps;
        unsigned int       dwCaps2;
        unsigned int       dwCaps3;
        union
        {
            unsigned int       dwCaps4;
            unsigned int       dwVolumeDepth;
        };
    } ddsCaps;// direct draw surface capabilities

    unsigned int               dwTextureStage;         // stage in multitexture cascade
} DDSURFACEDESC2;


unsigned char* compress_to_dxt(const unsigned char* pData, int width, int height, int bpp, int* outDstSize)
{
    const int c = bpp / 8; // num channels
    const int blockSize = 1 << c;//bpp == 24 ? 8 : 16;
    const int dstSize = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
    unsigned char* dst = new unsigned char[dstSize];
    if (!dst)
    {
        *outDstSize = 0;
        return NULL;
    }
    *outDstSize = dstSize;

    const int stride = width * c;
    unsigned char* pd = dst;
    for (int j = 0; j < height; j += 4)
    {
        for (int i = 0; i < width; i += 4)
        {
            const unsigned char* row[4] = {
                &pData[(j + 0) * stride + i * c],
                &pData[(j + 1) * stride + i * c],
                &pData[(j + 2) * stride + i * c],
                &pData[(j + 3) * stride + i * c]
            };

            unsigned char block[16][4];
            for (int b = 0; b < 16; ++b)
            {
                block[b][0] = row[b / 4][(b % 4) * c + 0];
                block[b][1] = row[b / 4][(b % 4) * c + 1];
                block[b][2] = row[b / 4][(b % 4) * c + 2];
                block[b][3] = bpp == 24 ? 0 : row[b / 4][(b % 4) * c + 3];
            }

            stb_compress_dxt_block(pd, &block[0][0], bpp == 24 ? 0 : 1, STB_DXT_HIGHQUAL /* or STB_DXT_NORMAL */);
            pd += blockSize;
        }
    }
    return dst;
}

void Texture::raw_to_dds_file(const char* filename, const unsigned char* pData, int width, int height, int bpp)
{
    if (bpp != 24 && bpp != 32)
        return;

    /*if ((width % 4) || (height % 4))
        return;*/

    FILE* f; 
    errno_t err;
    if((err = fopen_s(&f, filename, "wb")) != 0) 
    {
        LOG(LogType::LOG_ERROR, "Cannot open file '%s'", filename);
        return;
    }

    char filecode[5] = "DDS ";
    fwrite(filecode, 4, 1, f);

    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = 0;
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;
    ddsd.lPitch = width * height;
    ddsd.dwMipMapCount = 0;
    ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
    ddsd.ddpfPixelFormat.dwFlags = DDSF_FOURCC;
    ddsd.ddpfPixelFormat.dwFourCC = bpp == 24 ? FOURCC_DXT1 : FOURCC_DXT5;
    fwrite(&ddsd, sizeof(ddsd), 1, f);

    int dstSize = 0;
    unsigned char* dst = compress_to_dxt(pData, width, height, bpp, &dstSize);
    if (dst)
    {
        fwrite(dst, dstSize, 1, f);
        delete[] dst;
    }

    fclose(f);
}

unsigned char* Texture::raw_to_dds_mem(const unsigned char* data, int width, int height, int bpp, size_t* outSize)
{
    if (bpp != 24 && bpp != 32)
        return NULL;

    /*if ((width % 4) || (height % 4))
        return;*/

    char filecode[5] = "DDS ";

    DDSURFACEDESC2 ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = 0;
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;
    ddsd.lPitch = width * height;
    ddsd.dwMipMapCount = 0;
    ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
    ddsd.ddpfPixelFormat.dwFlags = DDSF_FOURCC;
    ddsd.ddpfPixelFormat.dwFourCC = bpp == 24 ? FOURCC_DXT1 : FOURCC_DXT5;

    int dstSize = 0;
    unsigned char* dst = compress_to_dxt(data, width, height, bpp, &dstSize);
    if (dst)
    {
        *outSize = 4 + sizeof(ddsd) + dstSize;
        unsigned char* outdata = new unsigned char[*outSize];

        unsigned char* odp = outdata;
        memcpy(odp, filecode, 4); odp += 4;
        memcpy(odp, &ddsd, sizeof(ddsd)); odp += sizeof(ddsd);
        memcpy(odp, dst, dstSize);

        delete[] dst;

        return outdata;
    }

    return NULL;
}

void Texture::free_dds_mem(unsigned char* data)
{
    delete[] data;
}