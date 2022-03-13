#ifndef PROJECTION_HPP
#define PROJECTION_HPP

class Projection final {
private:
    Projection(const Projection& orig);
    GLuint programID;
    GLint renderSizeLocation;
    GLint inData1Location;
    GLint inData2Location;
    GLint invertEdgesLocation;
    void setEdgeInversion(bool b1, bool b2, bool b3, bool b4);
    void setInputs(GLuint tex1ID, GLuint tex2ID);
    void setOutput(GLuint texID);
public:
    Projection(int vertex_shader);
    ~Projection() = default;
    void apply(GLuint& vTextureID, GLuint pressureTextureID, GLuint& tmpTextureID,
            int width, int height,
            bool invert1X, bool invert1Y, bool invert2X, bool invert2Y);
};

#endif /* PROJECTION_HPP */

