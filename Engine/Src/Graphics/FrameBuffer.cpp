#include "Engine/Graphics/FrameBuffer.h"
#include <SDL2/SDL_log.h>
#include <glad/glad.h>
#include "Engine/Graphics/Resources/Texture.h"

FrameBuffer::FrameBuffer(const FrameBufferSpecification &spec)
:mSpec(spec) {
    Reset();
}

FrameBuffer::~FrameBuffer() {
    if (mFBO) glDeleteFramebuffers(1, &mFBO);
    if (mRBO) glDeleteRenderbuffers(1, &mRBO);
}

void FrameBuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Reset() {
    if (mFBO) {
        glDeleteFramebuffers(1, &mFBO);
        glDeleteRenderbuffers(1, &mRBO);
        mTexture.reset();
    }

    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    mTexture = std::make_unique<Texture>();
    bool textureBuilt = false;

    if (mSpec.format == FrameBufferFormat::ColorWithDepthRBO) {
        textureBuilt = mTexture->BuildEmpty(mSpec.width, mSpec.height);

        if (textureBuilt) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture->GetID(), 0);

            glGenRenderbuffers(1, &mRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mSpec.width, mSpec.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);
        }

    }else if (mSpec.format == FrameBufferFormat::DepthOnlyTexture) {
        textureBuilt = mTexture->BuildDepth(mSpec.width, mSpec.height);

        if (textureBuilt) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTexture->GetID(), 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }

    if (!textureBuilt) {
        SDL_Log("[FRAME BUFFER] Failed to generate framebuffer texture");
        CleanUpOnFailure();
        return;
    }

    if (const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE) {
        SDL_Log("[FRAME BUFFER] Incomplete FBO. Status GL: 0x%x", status);
        CleanUpOnFailure();
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CleanUpOnFailure() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (mFBO) {
        glDeleteFramebuffers(1, &mFBO);
        mFBO = 0;
    }
    if (mRBO) {
        glDeleteRenderbuffers(1, &mRBO);
        mRBO = 0;
    }

    mTexture.reset();
}