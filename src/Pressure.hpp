#ifndef PRESSURE_HPP
#define PRESSURE_HPP

class Pressure final {
private:
    Pressure(const Pressure& orig);
    GLuint programID;
    GLint renderSizeLocation;
    GLint inData1Location;
    GLint inData2Location;
    void setInputs(GLuint tex1ID, GLuint tex2ID);
    void setOutput(GLuint texID);
public:
    Pressure(int vertex_shader);
    ~Pressure() = default;
    void apply(GLuint divTextureID, GLuint& outTextureID, GLuint& tmpTextureID,
            int width, int height);
};

#endif /* PRESSURE_HPP */

