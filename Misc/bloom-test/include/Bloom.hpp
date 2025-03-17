#ifndef BLOOM_HPP
#define BLOOM_HPP

#include "rendering/PostProcessingEffect.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture2D.hpp"
#include <glad/glad.h>

namespace FrameEngine {

class BloomEffect : public PostProcessingEffect {
public:
  BloomEffect();
  virtual ~BloomEffect();

  virtual void init(int width, int height) override;

  virtual void resize(int width, int height) override;

  virtual void apply(GLuint inputTexture) override;

  virtual GLuint getOutputTexture() const override;

  void setBloomIntensity(float intensity);
  void setThreshold(float threshold);
  void setSoftThreshold(float softThreshold);

private:
  int width, height;

  GLuint fboBright; // For bright pass extraction.
  GLuint texBright; // Downsampled version of one above.

  // Two ping-pong framebuffers for blur passes.
  GLuint fboBlur[2];
  GLuint texBlur[2];

  // Framebuffer and texture for final composite.
  GLuint fboComposite;
  GLuint texComposite;

  // Shaders for each pass.
  Shader *brightPassShader;
  Shader *blurHShader;
  Shader *blurVShader;
  Shader *compositeShader;

  float bloomIntensity;
  float threshold;
  float softThreshold;

  // Initialize framebuffer and texture for a given width/height.
  void initFramebuffer(GLuint &fbo, GLuint &tex, int width, int height);

  // Render a full screen quad.
  void renderQuad();

  // Internal quad VAO/VBO (created on first use)
  static GLuint quadVAO;
  static GLuint quadVBO;
};

} // namespace FrameEngine

#endif // BLOOM_HPP
