#include <vector>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "disp.h"
#include "prog.h"
#include "util.h"

std::vector<std::string> split(std::string buff, char delim) {
	std::vector<std::string> tok;

	std::stringstream s(buff);
	std::string seg;
	while (std::getline(s, seg, delim)) {
		tok.push_back(seg);
	}

	return tok;
}

std::vector<GLfloat> rdVtc(std::string fName) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = util::rdVec(fName + ".obj");

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = split(buff[l], ' ');

		if (tok[0] == "v") {
			for (int i = 1; i < 1 + 3; i++) {
				_.push_back(std::stof(tok[i]));
			}
		}
	}

	return _;
}

int main() {
	Disp disp("asdf", 800, 600);

	glm::vec3 scale = glm::vec3(1, 1, 1);

	// data
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// position
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	std::vector<GLfloat> vtc = rdVtc("c_shotgun");

	glBufferData(GL_ARRAY_BUFFER, vtc.size() * sizeof (GLfloat), &vtc[0], GL_STATIC_DRAW);

	// matrix
	glm::mat4
		model = glm::mat4(1.0),
		view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)),
		proj = glm::perspective(glm::radians(45.0), 800.0 / 600.0, 0.1, 100.0);

	// shader
	Prog prog("dir", "dir");

	/// attribute
	GLint attrPos = glGetAttribLocation(prog._id, "pos");
	glVertexAttribPointer(attrPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(attrPos);

	/// uniform
	GLint
		uniModel = glGetUniformLocation(prog._id, "model"),
	 	uniView = glGetUniformLocation(prog._id, "view"),
	 	uniProj = glGetUniformLocation(prog._id, "proj");

	// initialize
	prog.use();

	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	Prog progOutline("outline", "outline");

	/// attribute
	GLint attrPosOutline = glGetAttribLocation(prog._id, "pos");
	glVertexAttribPointer(attrPosOutline, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(attrPosOutline);

	/// uniform
	GLint
		uniModelOutline = glGetUniformLocation(progOutline._id, "model"),
	 	uniViewOutline = glGetUniformLocation(progOutline._id, "view"),
	 	uniProjOutline = glGetUniformLocation(progOutline._id, "proj");

	progOutline.use();

	glUniformMatrix4fv(uniProjOutline, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniViewOutline, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniModelOutline, 1, GL_FALSE, glm::value_ptr(model));

	SDL_Event e;
	unsigned int t = 0;
	while (disp.open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				SDL_Quit();
			}

			if (e.type == SDL_MOUSEWHEEL) {
				if (e.wheel.y > 0) {
					if (glm::all(glm::lessThan(scale, glm::vec3(5.0, 5.0, 5.0)))) {
						scale += glm::vec3(0.1, 0.1, 0.1);
					}
				}

				if (e.wheel.y < 0) {
					if (glm::all(glm::greaterThan(scale, glm::vec3(0.5, 0.5, 0.5)))) {
						scale -= glm::vec3(0.1, 0.1, 0.1);
					}
				}
			}
		}

		disp.clear(42 / 255.0, 39 / 255.0, 37 / 255.0, 1);

		model = glm::mat4(1.0);
		model = glm::rotate(model, (GLfloat) (t * (M_PI / 100.0)), glm::vec3(0, 1, 0));
		model = glm::scale(model, scale);

		glDisable(GL_DEPTH_TEST);

		progOutline.use();

		glUniformMatrix4fv(uniModelOutline, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 2 * 3);

		progOutline.unUse();

		glEnable(GL_DEPTH_TEST);

		prog.use();

		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 2 * 3);

		prog.unUse();

		disp.update();

		t++;
	}
}
