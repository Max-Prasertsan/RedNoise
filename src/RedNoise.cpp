#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>


#define WIDTH 320
#define HEIGHT 240

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}


void drawLine(CanvasPoint from, CanvasPoint to, Colour colour, DrawingWindow &window){
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));

    float xStepSize = xDiff/numberOfSteps;
    float yStepSize = yDiff/numberOfSteps;

    uint32_t c = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8 ) + int(colour.blue);

    for (float i = 0.0; i < numberOfSteps; i++ ){
        float x = from.x + (xStepSize*i);
        float y = from.y + (yStepSize*i);

        window.setPixelColour(round(x), round(y), c);
    }
}


void drawTriangle(CanvasTriangle tri, Colour colour, DrawingWindow &window){
    CanvasPoint a = tri[0];
    CanvasPoint b = tri[1];
    CanvasPoint c = tri[2];

    drawLine(a, b, colour, window);
    drawLine(b, c, colour, window);
    drawLine(c, a, colour, window);
}


void fillTriangle(CanvasTriangle tri, Colour c, DrawingWindow &window){
    // t --> top, m --> middle, b --> bottom
    CanvasPoint t = tri.vertices[0];
    CanvasPoint m = tri.vertices[1];
    CanvasPoint b = tri.vertices[2];

    float x1Diff = m.x - t.x;
    float y1Diff = m.y - t.y;

    float x2Diff = b.x - t.x;
    float y2Diff = b.y - t.y;

    float step1 = std::max(abs(x1Diff), abs(y1Diff));
    float step2 = std::max(abs(x2Diff), abs(y2Diff));

    float x1step = x1Diff/step1;
    float x2step = x2Diff/step2;
    float y1step = y1Diff/step1;
    float y2step = y2Diff/step2;

    for(float i = 0.0; i < step1; i++){
        for(float j = 0.0; j < step2; j++){
            float x1 = t.x + (x1step*i);
            float y1 = t.y + (y1step*i);

            float x2 = t.x + (x2step*j);
            float y2 = t.y + (y2step*j);
            drawLine(CanvasPoint(round(x1), round(y1)), CanvasPoint(round(x2), round(y2)), c, window);
            drawTriangle(tri, Colour(255, 255, 255), window);
        }
    }
}

void drawFillTri(DrawingWindow &window){
    Colour c(rand()%256, rand()%256, rand()%256);

    CanvasPoint p1(rand()%320, rand()%240);
    CanvasPoint p2(rand()%320, rand()%240);
    CanvasPoint p3(rand()%320, rand()%240);
    CanvasTriangle tri(p1, p2, p3);

    fillTriangle(tri, c, window);
}


void drawMultiTri(DrawingWindow &window){
    Colour c(rand()%256, rand()%256, rand()%256);

    CanvasPoint p1(rand()%320, rand()%240);
   	CanvasPoint p2(rand()%320, rand()%240);
   	CanvasPoint p3(rand()%320, rand()%240);
    CanvasTriangle tri(p1, p2, p3);

    drawTriangle(tri, c, window);
}




std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){
    std::vector<float> v;
    float range = to - from;
    float spacing = range / (numberOfValues - 1);
    float num = from;

    for (int i = 0; i < numberOfValues; ++i){
        v.push_back(num);
        num = num + spacing;
    }
    return v;
}


void drawGradient(DrawingWindow &window) {
	window.clearPixels();
	std::vector<float> container;
	container = interpolateSingleFloats(255, 0, window.width);
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float color = container[x];
			uint32_t colour = (255 << 24) + (int(color) << 16) + (int(color) << 8) + int(color);
			window.setPixelColour(x, y, colour);
		}
	}
}


// triple interpolation
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues){
    std::vector<glm::vec3> result;
    glm::vec3 spacing = (to - from)/(numberOfValues-1);
    for (float i = 0; i < numberOfValues; ++i){
        result.push_back(from + i*spacing);
    }
    return result;
}


void drawColorGradient(DrawingWindow &window) {
    window.clearPixels();
    glm::vec3 t_Left(255, 0, 0);       //red
    glm::vec3 t_Right(0, 0, 255);      //blue
    glm::vec3 b_Left(255, 255, 0);    //green
    glm::vec3 b_Right(0, 255, 0); //yellow

    std::vector<glm::vec3> c_left = interpolateThreeElementValues(t_Left, b_Left, window.height);
    std::vector<glm::vec3> c_right = interpolateThreeElementValues(t_Right, b_Right, window.height);
    for (size_t y = 0; y < window.height; y++){
        std::vector<glm::vec3> colourPack = interpolateThreeElementValues(c_left[y], c_right[y], window.width);
        for(size_t x = 0; x < window.width; x++){
            uint32_t packed = (255 << 24) + (int(colourPack[x][0]) << 16) + (int(colourPack[x][1]) << 8) + int(colourPack[x][2]);
            window.setPixelColour(x, y, packed);
        }
    }
}


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_u)  drawMultiTri(window);
		else if (event.key.keysym.sym == SDLK_f) drawFillTri(window);
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}


int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	CanvasPoint from(0, 0);
	CanvasPoint to(window.width-1, window.height-1);
	Colour c(144, 250, 15);


	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		//drawGradient(window);
		drawColorGradient(window);
		//drawLine(from, to, c, window);

		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
