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
          mShader(std::filesystem::path{"main_sfml.vert"}, std::filesystem::path{"main.frag"}) {
        mShader.setUniform("texture", sf::Shader::CurrentTexture);
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
        sf::Vector2u size = mAtlas.getSizePixels(textureID);

        sf::Vertex botLeft{{0.0f, 0.0f}, sf::Color::White, {u0, v0}};
        sf::Vertex botRight{{size.x, 0.0f}, sf::Color::White, {u1, v0}};
        sf::Vertex topLeft{{0.0f, size.y}, sf::Color::White, {u0, v1}};
        sf::Vertex topRight{{size.x, size.y}, sf::Color::White, {u1, v1}};

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

    void setTexture(SpriteID sprite, TextureID newTexture) {
        sf::FloatRect newUV = mAtlas.getBoundsUV(newTexture);
        float u0 = newUV.position.x;
        float v0 = newUV.position.y;
        float u1 = u0 + newUV.size.x;
        float v1 = v0 + newUV.size.y;

        sf::Vector2u size = mAtlas.getSizePixels(newTexture);

        // Create new vertices based on new size and uv bounds
        sf::Vertex newBotLeft{{0.0f, 0.0f}, sf::Color::White, {u0, v0}};
        sf::Vertex newBotRight{{size.x, 0.0f}, sf::Color::White, {u1, v0}};
        sf::Vertex newTopLeft{{0.0f, size.y}, sf::Color::White, {u0, v1}};
        sf::Vertex newTopRight{{size.x, size.y}, sf::Color::White, {u1, v1}};

        size_t firstVertex = sprite * 6; // 6 vertices for each sprite
        // Follow the same order as the vertices were set in createSprite
        mVertices[firstVertex] = newBotLeft;
        mVertices[firstVertex + 1] = newBotRight;
        mVertices[firstVertex + 2] = newTopRight;

        mVertices[firstVertex + 3] = newBotLeft;
        mVertices[firstVertex + 4] = newTopRight;
        mVertices[firstVertex + 5] = newTopLeft;
    };

    // TODO
    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::VertexArray temp = mVertices;
        for (size_t i = 0; i < temp.getVertexCount(); i++) {
            const BasicTransform transform = mTransforms[i / 6];
            temp[i].position = transform.applyPoint(mVertices[i].position);
        }

        states.shader = &mShader;
        states.texture = &mAtlas.getTexture();
        target.draw(temp, states);
    };

    // TODO
    void drawGL() {};

    // TODO
    BasicTransform& getTransform(SpriteID id) {
        return mTransforms[id];
    }

    // TODO
    const TextureAtlas& getAtlas() const {
        return mAtlas;
    };

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
