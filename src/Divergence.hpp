#ifndef DIVERGENCE_HPP
#define DIVERGENCE_HPP

class Divergence final {
private:
    Divergence(const Divergence& orig);
    GLuint programID;
    GLint renderSizeLocation;
    GLint inDataLocation;
    void setInput(GLuint texID);
    void setOutput(GLuint texID);
public:
    Divergence(int vertex_shader);
    ~Divergence() = default;
    void apply(GLuint vTextureID, GLuint outTextureID,
            int width, int height);
};

#endif /* DIVERGENCE_HPP */

