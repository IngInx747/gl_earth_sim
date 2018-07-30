#include <Operation.h>

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



OpTranslate :: OpTranslate(glm::vec3 vec) : position(vec) { type = Operation::TRANSLATE; }

OpTranslate :: OpTranslate(float x, float y, float z) : position(x, y, z) { type = Operation::TRANSLATE; }

void OpTranslate :: operate(glm::mat4 & matrix) { matrix = glm::translate(matrix, position); }



OpScale :: OpScale(glm::vec3 vec) : scale(vec) { type = Operation::SCALE; }

OpScale :: OpScale(float x, float y, float z) : scale(x, y, z) { type = Operation::SCALE; }

void OpScale :: operate(glm::mat4 & matrix) { matrix = glm::scale(matrix, scale); }



OpRotate :: OpRotate(glm::vec3 vec, float w) : axis(vec), rotation(w) { type = Operation::ROTATE; }

OpRotate :: OpRotate(float x, float y, float z, float w) : axis(x, y, z), rotation(w) { type = Operation::ROTATE; }

void OpRotate :: operate(glm::mat4 & matrix) { matrix = glm::rotate(matrix, rotation, axis); }
