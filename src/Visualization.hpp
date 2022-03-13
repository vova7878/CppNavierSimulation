#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

class Visualization final {
private:
    Visualization(const Visualization& orig);
    GLuint programID;
    GLint renderSizeLocation;
    GLint inData1Location;
    GLint inData2Location;
    void setInputs(GLuint tex1ID, GLuint tex2ID);
    void setOutput(GLuint texID);
public:
    Visualization(int vertex_shader);
    ~Visualization() = default;
    void apply(GLuint cTextureID, GLuint vTextureID, GLuint displayTextureID,
            int width, int height);
};

#endif /* VISUALIZATION_HPP */

