#include "Bloom.hpp"
#include "Logger.hpp"
#include <cmath>
#include <iostream>

namespace FrameEngine {

// Static quad VAO/VBO initializers.
GLuint BloomEffect::quadVAO = 0;
GLuint BloomEffect::quadVBO = 0;

BloomEffect::BloomEffect()
    : width(0), height(0), fboBright(0), texBright(0), fboComposite(0),
      texComposite(0), brightPassShader(nullptr), blurHShader(nullptr),
      blurVShader(nullptr), compositeShader(nullptr), bloomIntensity(1.0f),
      threshold(1.0f), softThreshold(0.2f) {
  fboBlur[0] = fboBlur[1] = 0;
  texBlur[0] = texBlur[1] = 0;
}

BloomEffect::~BloomEffect() {
  glDeleteFramebuffers(1, &fboBright);
  glDeleteTextures(1, &texBright);
  glDeleteFramebuffers(1, &fboBlur[0]);
  glDeleteFramebuffers(1, &fboBlur[1]);
  glDeleteTextures(1, &texBlur[0]);
  glDeleteTextures(1, &texBlur[1]);
  glDeleteFramebuffers(1, &fboComposite);
  glDeleteTextures(1, &texComposite);

  delete brightPassShader;
  delete blurHShader;
  delete blurVShader;
  delete compositeShader;
}

void BloomEffect::init(int w, int h) {
  width = w;
  height = h;

  brightPassShader = new Shader("shaders/postProcessing/quad.vs",
                                "shaders/postProcessing/bloom_extract.fs");
  blurHShader = new Shader("shaders/postProcessing/quad.vs",
                           "shaders/postProcessing/bloom_blur_h.fs");
  blurVShader = new Shader("shaders/postProcessing/quad.vs",
                           "shaders/postProcessing/bloom_blur_v.fs");
  compositeShader = new Shader("shaders/postProcessing/quad.vs",
                               "shaders/postProcessing/bloom_composite.fs");

  // For performance I use a downsampled resolution.
  int dsWidth = width / 2;
  int dsHeight = height / 2;

  initFramebuffer(fboBright, texBright, dsWidth, dsHeight);
  initFramebuffer(fboBlur[0], texBlur[0], dsWidth, dsHeight);
  initFramebuffer(fboBlur[1], texBlur[1], dsWidth, dsHeight);
  initFramebuffer(fboComposite, texComposite, width, height);
}

void BloomEffect::resize(int w, int h) {
  width = w;
  height = h;
  int dsWidth = width / 2;
  int dsHeight = height / 2;

  glBindTexture(GL_TEXTURE_2D, texBright);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dsWidth, dsHeight, 0, GL_RGBA,
               GL_FLOAT, nullptr);

  for (int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, texBlur[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dsWidth, dsHeight, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
  }

  glBindTexture(GL_TEXTURE_2D, texComposite);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  glViewport(0, 0, width, height);
}

void BloomEffect::apply(GLuint inputTexture) {
  int dsWidth = width / 2;
  int dsHeight = height / 2;

  // === 1. Bright Pass Extraction ===
  glBindFramebuffer(GL_FRAMEBUFFER, fboBright);
  glViewport(0, 0, dsWidth, dsHeight);
  glClear(GL_COLOR_BUFFER_BIT);

  brightPassShader->bind();
  brightPassShader->setUniformFloat("threshold", threshold);
  brightPassShader->setUniformFloat("softThreshold", softThreshold);

  // Render input texture.
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, inputTexture);
  brightPassShader->setUniformInt("sceneTexture", 0);
  renderQuad();
  brightPassShader->unbind();

  // === 2. Separable Blur ===
  // Horizontal blur.
  glBindFramebuffer(GL_FRAMEBUFFER, fboBlur[0]);
  glViewport(0, 0, dsWidth, dsHeight);
  glClear(GL_COLOR_BUFFER_BIT);
  blurHShader->bind();
  blurHShader->setUniformFloat("texelWidth", 1.0f / dsWidth);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texBright);
  blurHShader->setUniformInt("image", 0);
  renderQuad();
  blurHShader->unbind();

  // Vertical blur.
  glBindFramebuffer(GL_FRAMEBUFFER, fboBlur[1]);
  glClear(GL_COLOR_BUFFER_BIT);
  blurVShader->bind();
  blurVShader->setUniformFloat("texelHeight", 1.0f / dsHeight);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texBlur[0]);
  blurVShader->setUniformInt("image", 0);
  renderQuad();
  blurVShader->unbind();

  // === 3. Composite Pass ===
  // Bind composite framebuffer at full resolution.
  glBindFramebuffer(GL_FRAMEBUFFER, fboComposite);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  compositeShader->bind();
  compositeShader->setUniformFloat("bloomIntensity", bloomIntensity);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, inputTexture); // Original scene
  compositeShader->setUniformInt("sceneTexture", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texBlur[1]); // Blurred bloom
  compositeShader->setUniformInt("bloomTexture", 1);
  renderQuad();
  compositeShader->unbind();

  // Unbind framebuffer.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint BloomEffect::getOutputTexture() const { return texComposite; }

void BloomEffect::setBloomIntensity(float intensity) {
  bloomIntensity = intensity;
}
void BloomEffect::setThreshold(float t) { threshold = t; }
void BloomEffect::setSoftThreshold(float st) { softThreshold = st; }

void BloomEffect::initFramebuffer(GLuint &fbo, GLuint &tex, int w, int h) {
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         tex, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOG(ERROR, "Framebuffer not complete in BloomEffect!");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomEffect::renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    // texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

} // namespace FrameEngine
