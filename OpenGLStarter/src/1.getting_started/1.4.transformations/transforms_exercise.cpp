#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

int t_exercises_main()
{
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f); // basically vec = (1,0,0)
	glm::mat4 trans = glm::mat4(1.0f); // initialize to an identity matrix (weird I know)
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f)); // trans = (1,1,0)
	trans = glm::rotate(trans, glm::radians(9.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	vec = trans * vec;

	std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;

	return 0;
}