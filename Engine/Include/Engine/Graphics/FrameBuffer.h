#pragma once
#include <memory>

enum class FrameBufferFormat {
    ColorWithDepthRBO, // Post Process
    DepthOnlyTexture   // Shadow Mapping
};

struct FrameBufferSpecification {
    int width;
    int height;
    FrameBufferFormat format;
};

class Texture;

class FrameBuffer {
public:
    explicit FrameBuffer(const FrameBufferSpecification& spec);
    ~FrameBuffer();

    void Bind() const;
    static void Unbind();

    [[nodiscard]] Texture* GetTexture() const { return mTexture.get(); }

private:
    void Reset();
    void CleanUpOnFailure();

    unsigned int mFBO{0};
    unsigned int mRBO{0};

    FrameBufferSpecification mSpec;
    std::unique_ptr<Texture> mTexture;
};