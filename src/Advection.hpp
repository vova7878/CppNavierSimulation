#ifndef ADVECTION_HPP
#define ADVECTION_HPP

class Advection final {
private:
    Advection(const Advection& orig);
    GLuint programID;
    GLint renderSizeLocation;
    GLint inData1Location;
    GLint inData2Location;
    GLint invertEdgesLocation;
    GLint dtLocation;
    void setEdgeInversion(bool b1, bool b2, bool b3, bool b4);
    void setInputs(GLuint tex1ID, GLuint tex2ID);
    void setOutput(GLuint texID);
public:
    Advection(int vertex_shader);
    ~Advection() = default;
    void apply(GLuint& dataTextureID, GLuint vTextureID, GLuint& tmpTextureID,
            int width, int height, float dt,
            bool invert1X, bool invert1Y, bool invert2X, bool invert2Y);
};

#endif /* ADVECTION_HPP */

