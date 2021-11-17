#include <buffer.hpp>

Buffer::Buffer()
{
    VAO = 0;
    VBO = 0;
    mat = glm::mat4(1.0f);
}

Buffer::~Buffer()
{
    deleteVBO();
}

void Buffer::deleteVBO()
{
    // glDeleteBuffers(1, &VBO);
    // glDeleteVertexArrays(1, &VAO);
}
void Buffer::loadVBO()
{
    // printf("%d\n", chunk.Vertices.size());
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &pos[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}
