#include "simplerendering.hpp"
#include <glutils/baseshader.hpp>
#include <glutils/uniform.hpp>
#include <gl_core_3_3.h>
#include <iostream>
#include <glutils/projection.hpp>
#include <GLFW/glfw3.h>
#include <window.hpp>
#include "events.hpp"
#include <glutils/textureloader.hpp>
#include <gimgui/logic/boundarypropagator.hpp>

SimpleRendering::SimpleRendering(const Vec2& viewSize):
    mQuit(false),
    mTriangles(Buffer::ARRAY_BUFFER),
    mColors(Buffer::ARRAY_BUFFER),
    mTexCoords(Buffer::ARRAY_BUFFER),
    mGui(
    {
        gim::Element({"container"},
        {
            {"color",    Color(140, 35, 24)},
            {"position", Vec2({200, 150})},
            {"size",     Vec2({256, 256})},
            {"stretch_mode", gim::StretchMode::STRETCHED},
            {"image_id", 0},
            {"image_coords", gim::Rectangle<Vec2>(Vec2({0, 0}), Vec2({64, 64}))}
        },
        {
            gim::Element({"child"},
            {
                {"color",    Color(94, 140, 106)},
                {"position", Vec2({20, 20})},
                {"size",     Vec2({64, 64})},
                {"image_id", 0},
                {"image_coords", gim::Rectangle<Vec2>(Vec2({0, 0}), Vec2({64, 64}))}
            }),
            gim::Element({"child"},
            {
                {"color",    Color(136, 166, 94)},
                {"position", Vec2({90, 20})},
                {"size",     Vec2({64, 64})},
                {"image_id", 0},
                {"image_coords", gim::Rectangle<Vec2>(Vec2({0, 0}), Vec2({64, 64}))}
            }),
            gim::Element({"child"},
            {
                {"color",    Color(191, 179, 90)},
                {"position", Vec2({20, 90})},
                {"size",     Vec2({64, 64})},
                {"image_id", 0},
                {"image_coords", gim::Rectangle<Vec2>(Vec2({0, 0}), Vec2({64, 64}))}
            })
        })
    })
{
    //load textures

    mTextures.emplace(0, loadTexture("examples/resources/xpattern.png"));

    mRenderDataGenerator.registerImageInfo(0, {64, 64});
    //rendering
    mVao.bind();

    mTriangles.bind();

    mTriangles.setData(std::vector<float>(
    {
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    }));

    mColors.bind();

    mColors.setData(std::vector<float>(
    {
         1.0f,  1.0f, 0.0f,
         1.0f,  0.0f, 1.0f,
         0.0f,  1.0f, 1.0f 
    }));

    mVao.setVertexAttribute(0, 3, mTriangles);
    mVao.setVertexAttribute(1, 3, mColors);
    mVao.setVertexAttribute(2, 2, mTexCoords);

    mVao.unbind();

    mBaseShader.setSource(BaseShader::vertexSource, BaseShader::fragmentSource);
    mBaseShader.compile();

    Projection proj;
    mProjection = proj.createOrthoProjection(0.0f, (GLfloat)viewSize.x, 0.0f, (GLfloat)viewSize.y, 0.000000001f, 100.0f);
}

void SimpleRendering::loop()
{
    gim::AllPropagator allPropagator(mGui.root());
    mGui.sendEvent(randomColorEvent(), allPropagator);

    //rendering
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    mVao.bind();
    mBaseShader.activate();
    mBaseShader.setUniform("projection", UniformType::MAT4X4, &mProjection[0]);

    auto renderDatas = mRenderDataGenerator.generate(mGui.root());

    for(auto renderData : renderDatas)
    {
        GIM_ASSERT(mTextures.count(renderData.imageId) != 0, "invalid texture given");
        mBaseShader.setUniform("texture", UniformType::TEXTURE, &mTextures.at(renderData.imageId));
        mTriangles.setData(renderData.positions);
        mColors.setData(renderData.colors);
        mTexCoords.setData(renderData.texCoords);

        glDrawArrays(GL_TRIANGLES, 0, mTriangles.getElementAmount());
    }
}

void SimpleRendering::setViewSize(const Vec2& viewSize)
{
    Projection proj;
    mProjection = proj.createOrthoProjection(0.0f, (GLfloat)viewSize.x, 0.0f, (GLfloat)viewSize.y, 0.000000001f, 100.0f);
}

void SimpleRendering::keyEvent(int32_t key)
{
    if(key == GLFW_KEY_ESCAPE)
        quit();
}

bool SimpleRendering::isTerminated() const
{
    return mQuit;
}

void SimpleRendering::handleEvents(const Events& events)
{
    for(auto newSize : events.resizeEvents)
    {
        setViewSize(newSize);
    }

    for(int32_t key : events.keyEvents)
    {
        if(key == GLFW_KEY_ESCAPE)
            quit();
        else if(key == GLFW_KEY_D)
        {
            Vec2 pos = mGui.root().getAttribute<Vec2>("position") + Vec2({20, 20});
            mGui.root().setAttribute("position", pos);
        }
    }

    for(const Vec2& position : events.cursorPositionEvents)
    {
        gim::AllPropagator allPropagator(mGui.root());
        mGui.sendEvent(nonFocusAllEvent(), allPropagator);
        gim::BoundaryPropagator<Vec2> boundaryPropagator(mGui.root(), position);
        mGui.sendEvent(focusEvent(position.x, position.y), boundaryPropagator);
    }
}

void SimpleRendering::quit()
{
    mQuit = true;
}
