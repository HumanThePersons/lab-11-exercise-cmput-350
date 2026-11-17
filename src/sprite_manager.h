#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "basic_transform.h"
#include "texture_atlas.h"
#include "vector_util.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#define GL_SILENCE_DEPRECATION  // silences macOS OpenGL deprecation warnings

#define glCheck(expr)                                                                              \
    expr;                                                                                          \
    if (int e = glGetError()) {                                                                    \
        std::cerr << "OpenGL error: " << e << " in file " __FILE__ << ", line number " << __LINE__ \
                  << '\n';                                                                         \
        abort();                                                                                   \
    }

using SpriteID = size_t;

class SpriteManager {
public:
    SpriteManager(const sf::Vector2u& atlasSize, uint padding)
        : mVertices(sf::PrimitiveType::Triangles),
          mNumSprites{0},
          mAtlas(atlasSize, padding),
          mShader(std::filesystem::path{"main.vert"}, std::filesystem::path{"main.frag"}) {
        // TODO
    }

    TextureID registerTexture(const std::string& filepath) {
        return mAtlas.registerTexture(filepath);
    };

    SpriteID createSprite(TextureID textureID) {
        sf::FloatRect uvBounds = mAtlas.getBoundsUV(textureID);
        float u0 = uvBounds.position.x;
        float v0 = uvBounds.position.y;
        float u1 = u0 + uvBounds.size.x;
        float v1 = v0 + uvBounds.size.y;

        // Size in world units
        sf::Vector2u atlasSize = mAtlas.getTexture().getSize();
        float width = uvBounds.size.x * atlasSize.x;
        float height = uvBounds.size.y * atlasSize.y;

        sf::Vertex botLeft{{0.0f, 0.0f}, sf::Color::White, {u0, v0}};
        sf::Vertex botRight{{width, 0.0f}, sf::Color::White, {u1, v0}};
        sf::Vertex topLeft{{0.0f, height}, sf::Color::White, {u0, v1}};
        sf::Vertex topRight{{width,height}, sf::Color::White, {u1, v1}};

        // First triangle
        mVertices.append(botLeft);
        mVertices.append(botRight);
        mVertices.append(topRight);

        // Second triangle
        mVertices.append(botLeft);
        mVertices.append(topRight);
        mVertices.append(topLeft);

        mTransforms.push_back(BasicTransform());
        SpriteID newID = mNumSprites;
        mNumSprites++;  // increment total number of sprites
        return newID;
    };

    // TODO
    void setTexture(SpriteID sprite, TextureID newTexture) {};

    // TODO
    void draw(sf::RenderTarget& target, sf::RenderStates states) const {};

    // TODO
    void drawGL() {};

    // TODO
    BasicTransform& getTransform(SpriteID id) {}

    // TODO
    const TextureAtlas& getAtlas() const {};

private:
    sf::VertexArray mVertices;
    std::vector<BasicTransform> mTransforms;
    size_t mNumSprites;
    TextureAtlas mAtlas;
    mutable sf::Shader mShader;

    // Use the below only in Batched rendering (part 2):
    std::vector<float> mSpriteNumbers;

    GLint mTextureUniformLocation;
    GLint mTransformsUniformLocation;
    GLint mNumTransformsUniformLocation;

    GLint mPositionAttribLocation;
    GLint mTexCoordsAttribLocation;
    GLint mSpriteNumberAttribLocation;

    GLuint mVerticesVBO;
    GLuint mSpriteNumberVBO;
    GLuint mTransformsTexture;
};

#endif  // SPRITE_MANAGER_H
