#ifndef ADDITION_HPP
#define ADDITION_HPP

class Addition final {
private:
    Addition(const Addition& orig);
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
    Addition(int vertex_shader);
    ~Addition() = default;
    void apply(GLuint& dataTextureID, GLuint deltaTextureID, GLuint& tmpTextureID,
            int width, int height, float dt,
            bool invert1X, bool invert1Y, bool invert2X, bool invert2Y);
};

#endif /* ADDITION_HPP */

