#include <CanvasTriangle.h>
#include <ModelTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include <unordered_map>



#define WIDTH 600
#define HEIGHT 600

#define ini_cam_position vec3(0.0, 0.0, 4.0)
#define ini_focal_length 2.00


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


void drawLine(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow &window){
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));

    float xStepSize = xDiff/numberOfSteps;
    float yStepSize = yDiff/numberOfSteps;

    uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8 ) + int(c.blue);

    for (float i = 0.0; i < numberOfSteps; i++ ){
        float x = from.x + (xStepSize*i);
        float y = from.y + (yStepSize*i);

        window.setPixelColour(round(x), round(y), colour);
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


// triple interpolation
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues){
    std::vector<glm::vec3> result;
    glm::vec3 spacing = (to - from)/(numberOfValues-1);
    for (float i = 0; i < numberOfValues; ++i){
        result.push_back(from + i*spacing);
    }
    return result;
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


void drawColorGradient(DrawingWindow &window) {
    window.clearPixels();
    glm::vec3 t_Left(255, 0, 0);      //red
    glm::vec3 t_Right(0, 0, 255);     //blue
    glm::vec3 b_Left(255, 255, 0);    //green
    glm::vec3 b_Right(0, 255, 0);     //yellow

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


// NOT DONE. NEED WORK
void textureMap(CanvasPoint p, CanvasTriangle t){
    TextureMap source("./texture.ppm");
    p.texturePoint = TexturePoint (source.height, source.width);

}


std::unordered_map<std::string, Colour> parse_mtl(std::string& filename){
    std::unordered_map<std::string, Colour> colours;
    std::string colour;

    std::ifstream file(filename);
    std::string line;

    while(getline(file, line))
        {
            if(line == "") continue;

            std::vector<std::string> parts = split(line, ' ');
            if(parts[0] == "newmtl")
            {
                colour = parts[1];
            }
            else if(parts[0] == "Kd")
            {
                Colour c(float(std::stof(parts[1]) * 255), float(std::stof(parts[2]) * 255), float(std::stof(parts[3]) * 255));
                colours[colour] = c;
            }
        }
        file.close();
        return colours;
}


// For 3d model file reading Func
std::vector<ModelTriangle> parse_Obj(std::string& filename, float scale, std::unordered_map<std::string, Colour> colours){
    std::vector<ModelTriangle> triangles;
    std::vector<glm::vec3> vertices;
    std::string colour;

    std::ifstream file(filename);
    std::string line;


    while(getline(file, line))
    {

        // If blank line skip to the next one.
        if(line == "") continue;

        // split each components of the line in to vector called 'parts' then compare the first part.
        std::vector<std::string> parts = split(line, ' ');
        if (parts[0] == "v")
        {
            glm::vec3 vertex(std::stof(parts[1]) * scale, std::stof(parts[2]) * scale, std::stof(parts[3]) * scale);
            vertices.push_back(vertex);
        }

        else if(parts[0] == "f")
        {
            ModelTriangle triangle(vertices[std::stoi(parts[1]) - 1], vertices[std::stoi(parts[2]) - 1], vertices[std::stoi(parts[3]) - 1], colours[colour]);
            triangles.push_back(triangle);
        }

        else if(parts[0] == "usemtl")
        {
            colour = parts[1];
        }

    }
    file.close();
    return triangles;
}


CanvasPoint getCanvasPoint(glm::vec3 cam_position, glm::vec3 vertex, float focal_length){
    CanvasPoint point = CanvasPoint();
    vertex -= cam_position;
    point.x = ((focal_length / vertex.z) * -vertex.x) * 150 + WIDTH / 2;
    point.y = ((focal_length / vertex.z) * vertex.y) * 150 + HEIGHT / 2;
    point.depth = -vertex.z;
    return point;
}


// Drawing the models or "Objects" given in the 3dfile.
void draw_obj(DrawingWindow &window){
    std::vector<ModelTriangle> t = parse_Obj("./cornell-box.obj", 0.35, parse_mtl("./cornell-box.mtl"));
    uint32_t colourNum = (255 << 24) + (255 << 16) + (255 << 8) + 255;
    for(auto mt : t){
        for(int i = 0; i < 3; i ++){
            CanvasPoint currentPoint = getCanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[i], 2);
            window.setPixelColour(round(currentPoint.x), round(currentPoint.y), colourNum);
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

	//CanvasPoint from(0, 0);
	//CanvasPoint to(window.width-1, window.height-1);
	//Colour c(144, 250, 15);

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		//drawGradient(window);
		//drawColorGradient(window);
		//drawLine(from, to, c, window);
        draw_obj(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
