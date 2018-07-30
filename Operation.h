#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Operation {
public:
	virtual void operate(glm::mat4 & matrix) { std::cerr << "Operation::operate\n"; }
	
	enum OperationType
	{
		TRANSLATE,
		SCALE,
		ROTATE
	};

	OperationType type;
};

class OpTranslate : public Operation {
public:
	OpTranslate(glm::vec3);
	OpTranslate(float x, float y, float z);
	void operate(glm::mat4 & matrix) override;
	
protected:
	glm::vec3 position;
};

class OpScale : public Operation {
public:
	OpScale(glm::vec3);
	OpScale(float x, float y, float z);
	void operate(glm::mat4 & matrix) override;
	
protected:
	glm::vec3 scale;
};

class OpRotate : public Operation {
public:
	OpRotate(glm::vec3, float w);
	OpRotate(float x, float y, float z, float w);
	void operate(glm::mat4 & matrix) override;
	
protected:
	glm::vec3 axis; // rotating axis
	float rotation; // radian
};

#endif