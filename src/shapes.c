#include "shapes.h"

shape_t shape_init_square(void)
{
    shape_t s = SHAPE_INIT();

    GLfloat vertices[] = { -0.5f, 0.5f, 0.0f,
                           0.5f, 0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f,
                           -0.5f, -0.5f, 0.0f };
    s.v_len = 4;

    GLuint elements[] = { 0, 1, 2, 2, 3, 0 };
    s.e_len = sizeof(elements);

    //Init VAO
    glGenVertexArrays(1, &s.vao);
    glBindVertexArray(s.vao);

    //Init VBO
    glGenBuffers(1, &s.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);

    //Init EBO
    glGenBuffers(1, &s.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glEnableVertexAttribArray(VERT_ATTR_POS);
    glVertexAttribPointer(VERT_ATTR_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0); //Unbind VAO
    return s;
}

shape_t shape_init_triangle(void)
{
    shape_t s = SHAPE_INIT();

    GLfloat vertices[] = { 0.0f, 0.5f, 0.0f,
                           -1.0f, -1.0f, 0.0f,
                           1.0f, -1.0f, 0.0f };
    s.v_len = 3;

    //Init VAO
    GLuint vao = 0;
    glGenVertexArrays(1, &s.vao);
    glBindVertexArray(s.vao);

    //Init VBO
    glGenBuffers(1, &s.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(VERT_ATTR_POS);
    glVertexAttribPointer(VERT_ATTR_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0); //Unbind VAO
    return s;
}

void shape_draw(shape_t *s)
{
    glBindVertexArray(s->vao);

    if(s->ebo) {
        glDrawElements(GL_TRIANGLES, s->e_len, GL_UNSIGNED_INT, 0);
        return;
    }

    glDrawArrays(GL_TRIANGLES, 0, s->v_len);
}
