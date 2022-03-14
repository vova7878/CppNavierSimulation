#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Visualization.hpp"
#include "Addition.hpp"
#include "Advection.hpp"
#include "Divergence.hpp"
#include "Pressure.hpp"
#include "Projection.hpp"
#include "Diffusion.hpp"

class Renderer final {
private:
    GLuint fboID;
    GLuint vaoID;
    GLuint vboID;
    GLint positionLocation;

    GLuint displayTextureID;
    GLuint cTextureIDs[2]; //поля "веществ"
    GLuint sTextureIDs[2]; //дельта цвета
    GLuint vTextureID; //скорость
    GLuint fTextureID; //дельта скорости
    GLuint pTextureID; //давление
    GLuint tmpTextureIDs[2];

    float dt, diffusion, viscosity;

    int widthR, heightR;
    int widthS, heightS;

    Visualization* vis;
    Diffusion* diff;
    Advection* adv;
    Addition* add;
    //Scaling* scale;
    Divergence* div;
    Pressure* press;
    Projection* project;

    void drawStages();

    Renderer(const Renderer& orig);
public:
    Renderer() = default;
    ~Renderer() = default;
    void onCreate();
    void onDraw();
    void onChangeSize(GLFWwindow* window, int width, int height);
    void onChangeCursorPos(GLFWwindow* window, double xpos, double ypos);
    void onMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
    void onScroll(GLFWwindow* window, double xoffset, double yoffset);
    void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    void onDispose();
};

#endif /* RENDERER_HPP */

