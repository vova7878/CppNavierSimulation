#ifndef DIFFUSION_HPP
#define DIFFUSION_HPP

class Diffusion final {
private:
    Diffusion(const Diffusion& orig);
    GLuint programID;
    GLint renderSizeLocation;
    GLint inData1Location;
    GLint inData2Location;
    GLint invertEdgesLocation;
    GLint diffValueLocation;
    void setEdgeInversion(bool b1, bool b2, bool b3, bool b4);
    void setInputs(GLuint tex1ID, GLuint tex2ID);
    void setOutput(GLuint texID);
public:
    Diffusion(int vertex_shader);
    ~Diffusion() = default;
    void apply(GLuint& dataTextureID, GLuint& tmp1TextureID, GLuint& tmp2TextureID,
            int width, int height, float diffValue,
            bool invert1X, bool invert1Y, bool invert2X, bool invert2Y);
};

#endif /* DIFFUSION_HPP */

