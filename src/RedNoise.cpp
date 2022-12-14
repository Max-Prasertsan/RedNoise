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
#define time 0;

//#define ini_cam_position vec3(0.0, 0.0, 4.0)
#define ini_focal_length 2.00

glm::vec3 cam_position = glm::vec3(0.0, 0.0, 4.0);
glm::mat3 cameraOrMat = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
bool orbit;


void draw_Red(DrawingWindow &window) {
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


void draw_Line(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow &window){
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


std::vector<CanvasPoint> interpolateTexture(CanvasPoint from, CanvasPoint to, float numberOfValues) {
	std::vector<CanvasPoint> steps;
	float stepSizeX = (to.x - from.x) / (numberOfValues - 1);
	float stepSizeY = (to.y - from.y) / (numberOfValues - 1);
	float stepSizeTextureX = (to.texturePoint.x - from.texturePoint.x) / (numberOfValues - 1);
	float stepSizeTextureY = (to.texturePoint.y - from.texturePoint.y) / (numberOfValues - 1);

	for (int i = 0; i < numberOfValues; i++) {
		CanvasPoint n = CanvasPoint(from.x + i * stepSizeX, from.y + i * stepSizeY);
		n.texturePoint.x = from.texturePoint.x + i * stepSizeTextureX;
		n.texturePoint.y = from.texturePoint.y + i * stepSizeTextureY;
		steps.push_back(n);
	}

	return steps;
}

// DOES NOT WORK, DO NOT RUN.
void draw_TextureLine(CanvasPoint from, CanvasPoint to, TextureMap texture, DrawingWindow &window){
    std::cout << "New Line" << std::endl;

    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));

    std::vector<CanvasPoint> canvas_line = interpolateTexture(from, to, numberOfSteps);
	int size_canvas = canvas_line.size();
	if (size_canvas == 1) {
		uint32_t colourNum = texture.pixels[from.texturePoint.x + from.texturePoint.y * texture.width];
		std::cout << "Set Texture" << std::endl;
		window.setPixelColour(round(from.x), round(from.y), colourNum);
		return;
	}

    std::cout << "Draw From: " << from << " t.x: " << from.texturePoint.x << " t.y: " << from.texturePoint.y << std::endl;
	std::cout << "Draw To: " << to << " t.x: " << to.texturePoint.x << " t.y: " << to.texturePoint.y << std::endl;
	for (auto c: canvas_line) {
	    std::cout << "Draw: " << c << " t.x: " << c.texturePoint.x << " t.y: " << c.texturePoint.y << std::endl;
		float x = c.texturePoint.x;
		float y = c.texturePoint.y;
		uint32_t colourNum = texture.pixels.at(x + y * texture.width);
		window.setPixelColour(round(c.x), round(c.y), colourNum);
	}
}


void draw_LineDepth(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow &window, float** depthBuff){
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float depthDiff = to.depth - from.depth;

    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));

    float xStepSize = xDiff/numberOfSteps;
    float yStepSize = yDiff/numberOfSteps;

    float depthStepSize = depthDiff / numberOfSteps;

    uint32_t colour = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8 ) + int(c.blue);

    for (float i = 0.0; i < numberOfSteps; i++ ){
        float x = from.x + (xStepSize*i);
        float y = from.y + (yStepSize*i);
        float d = from.depth + (depthStepSize * i);

        float depth_inverse = 1 / d;
        if (depth_inverse > depthBuff[int(y)][int(x)])
        {
            depthBuff[int(y)][int(x)] = depth_inverse;
            window.setPixelColour(x, y, colour);
        }
        //window.setPixelColour(round(x), round(y), colour);
    }
}


void draw_Triangle(CanvasTriangle tri, Colour colour, DrawingWindow &window){
    CanvasPoint a = tri[0];
    CanvasPoint b = tri[1];
    CanvasPoint c = tri[2];

    draw_Line(a, b, colour, window);
    draw_Line(b, c, colour, window);
    draw_Line(c, a, colour, window);
}


void draw_MultiTri(DrawingWindow &window){
    Colour c(rand()%256, rand()%256, rand()%256);

    CanvasPoint p1(rand()%320, rand()%240);
   	CanvasPoint p2(rand()%320, rand()%240);
   	CanvasPoint p3(rand()%320, rand()%240);
    CanvasTriangle tri(p1, p2, p3);

    draw_Triangle(tri, c, window);
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
    glm::vec3 stepSize = (to - from)/(numberOfValues-1);
    for (float i = 0; i < numberOfValues; ++i){
        result.push_back(from + i*stepSize);
    }
    return result;
}


std::vector<CanvasPoint> interpolateCanvasPointWithDepth(CanvasPoint from, CanvasPoint to, int numberOfValues) {
	std::vector<CanvasPoint> result;
	float stepSizeX = (to.x - from.x) / (numberOfValues - 1);
	float stepSizeY = (to.y - from.y) / (numberOfValues - 1);

	float stepSizeDepth = (to.depth - from.depth) / (numberOfValues - 1);

	for (int i = 0; i < numberOfValues; i++) {
		CanvasPoint n = CanvasPoint(from.x + i * stepSizeX, from.y + i * stepSizeY);
		n.depth = from.depth + i * stepSizeDepth;
		result.push_back(n);
	}
	return result;
}


void draw_Gradient(DrawingWindow &window) {
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


void draw_ColorGradient(DrawingWindow &window) {
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


void draw_LineScheme(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow& window, std::vector<float> &depth_scheme) {
	std::vector<CanvasPoint> canvas_line = interpolateCanvasPointWithDepth(from, to, to.x - from.x);

	if (canvas_line.size() == 1) {
		int x = from.x;
		int y = from.y;
		float depth_inverse = 1 / from.depth;

		if (x < 0 || y < 0 || x + y * WIDTH >= HEIGHT * WIDTH || from.depth == 0) {
			return;
		}

		if (depth_inverse > depth_scheme[x + y * WIDTH]) {
			depth_scheme[x + y * WIDTH] = depth_inverse;
			uint32_t colourNumeric = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);
			window.setPixelColour(round(x), round(y), colourNumeric);
		}
		return;
	}

	for (auto canvas : canvas_line) {
		int x = canvas.x;
		int y = canvas.y;

		if (x < 0 || y < 0 || x + y * WIDTH >= HEIGHT * WIDTH || canvas.depth == 0) continue;
		float depth_inverse = 1 / canvas.depth;

		if (depth_inverse > depth_scheme[x + y * WIDTH]) {
			depth_scheme[x + y * WIDTH] = depth_inverse;
			uint32_t colourNumeric = (255 << 24) + (int(c.red) << 16) + (int(c.green) << 8) + int(c.blue);
			window.setPixelColour(round(x), round(y), colourNumeric);
		}
	}
}


void fill_Triangle(CanvasTriangle tri, Colour c, DrawingWindow &window){
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
            draw_Line(CanvasPoint(x1, y1), CanvasPoint(x2, y2), c, window);
            //draw_Triangle(tri, Colour(255, 255, 255), window);
        }
    }
}


void fill_TriangleTexture(CanvasTriangle tri, DrawingWindow &window){
    TextureMap texture = TextureMap("texture.ppm");

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

            std::cout << "before texture line" << std::endl;
            draw_TextureLine(CanvasPoint(x1, y1), CanvasPoint(x2, y2), texture, window);
        }
    }
}

// DOES NOT WORK. DO NOT RUN --> CAUSE ERROR
void draw_TextureTriangle(DrawingWindow &window){
    CanvasPoint t = CanvasPoint(160, 10);
    t.texturePoint = TexturePoint(195, 5);

    CanvasPoint m = CanvasPoint(10, 150);
    m.texturePoint = TexturePoint(65, 330);

    CanvasPoint b = CanvasPoint(300, 230);
    b.texturePoint = TexturePoint(395, 380);

    CanvasTriangle tri = CanvasTriangle(t, m, b);

    fill_TriangleTexture(tri, window);
}


void fill_TriangleDepth(CanvasTriangle tri, Colour c, DrawingWindow &window, float** depthBuff){
    // t --> top, m --> middle, b --> bottom
    CanvasPoint t = tri.vertices[0];
    CanvasPoint m = tri.vertices[1];
    CanvasPoint b = tri.vertices[2];

    // top - mid
    float x1Diff = m.x - t.x;
    float y1Diff = m.y - t.y;
    float depthDiff1 = m.depth - t.depth;

    // top - bottom
    float x2Diff = b.x - t.x;
    float y2Diff = b.y - t.y;
    float depthDiff2 = b.depth - t.depth;

    float step1 = std::max(abs(x1Diff), abs(y1Diff));
    float step2 = std::max(abs(x2Diff), abs(y2Diff));

    float depthStepSize1 = depthDiff1 / step1;
    float depthStepSize2 = depthDiff2 / step2;

    float x1step = x1Diff/step1;
    float x2step = x2Diff/step2;
    float y1step = y1Diff/step1;
    float y2step = y2Diff/step2;

    for(float i = 0.0; i < step1; i++){
        for(float j = 0.0; j < step2; j++){
            float x1 = t.x + (x1step*i);
            float y1 = t.y + (y1step*i);
            float d1 = t.depth + (depthStepSize1*i);

            float x2 = t.x + (x2step*j);
            float y2 = t.y + (y2step*j);
            float d2 = t.depth + (depthStepSize2*j);


            draw_LineDepth(CanvasPoint(x1, y1, d1), CanvasPoint(x2, y2, d2), c, window, depthBuff);
            //draw_Triangle(tri, Colour(255, 255, 255), window);
        }
    }
}


void draw_RandFillTri(DrawingWindow &window){
    Colour c(rand()%256, rand()%256, rand()%256);

    CanvasPoint p1(rand()%320, rand()%240);
    CanvasPoint p2(rand()%320, rand()%240);
    CanvasPoint p3(rand()%320, rand()%240);
    CanvasTriangle tri(p1, p2, p3);

    fill_Triangle(tri, c, window);
}


void fill_TriangleScheme(CanvasTriangle tri, Colour c, DrawingWindow &window, std::vector<float> &depth_scheme ){
    // t --> top, m --> middle, b --> bottom
    CanvasPoint t = tri.vertices[0];
    CanvasPoint m = tri.vertices[1];
    CanvasPoint b = tri.vertices[2];

    // top - mid
    float x1Diff = m.x - t.x;
    float y1Diff = m.y - t.y;
    float depthDiff1 = m.depth - t.depth;

    // top - bottom
    float x2Diff = b.x - t.x;
    float y2Diff = b.y - t.y;
    float depthDiff2 = b.depth - t.depth;

    float step1 = std::max(abs(x1Diff), abs(y1Diff));
    float step2 = std::max(abs(x2Diff), abs(y2Diff));

    float depthStepSize1 = depthDiff1 / step1;
    float depthStepSize2 = depthDiff2 / step2;

    float x1step = x1Diff/step1;
    float x2step = x2Diff/step2;
    float y1step = y1Diff/step1;
    float y2step = y2Diff/step2;

    for(float i = 0.0; i < step1; i++){
        for(float j = 0.0; j < step2; j++){
            float x1 = t.x + (x1step*i);
            float y1 = t.y + (y1step*i);
            float d1 = t.depth + (depthStepSize1*i);

            float x2 = t.x + (x2step*j);
            float y2 = t.y + (y2step*j);
            float d2 = t.depth + (depthStepSize2*j);

            draw_LineScheme(CanvasPoint(x1, y1, d1), CanvasPoint(x2, y2, d2), c, window, depth_scheme);
        }
    }
}


std::unordered_map<std::string, Colour> parse_Mtl(std::string filename){
    std::unordered_map<std::string, Colour> colours;
    std::string colour;

    std::ifstream colourFile(filename);
    std::string line;

    while(getline(colourFile, line))
        {
            if(line == "") continue;

            std::vector<std::string> parts = split(line, ' ');
            if(parts[0] == "newmtl")
            {
                colour = parts[1];
            }
            else if(parts[0] == "Kd")
            {
                Colour c(colour,
                         float(std::stof(parts[1]) * 255),
                         float(std::stof(parts[2]) * 255),
                         float(std::stof(parts[3]) * 255));
                colours[colour] = c;
            }
        }
        colourFile.close();
        return colours;
}


// For 3d model file reading Func
std::vector<ModelTriangle> parse_Obj(std::string filename, float scale, std::unordered_map<std::string, Colour> colours){
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
            glm::vec3 vertex(std::stof(parts[1]) * scale,
                             std::stof(parts[2]) * scale,
                             std::stof(parts[3]) * scale);
            vertices.push_back(vertex);
        }

        if(parts[0] == "f")
        {
            Colour triColour = colours[colour];
            ModelTriangle triangle(vertices[std::stoi(parts[1]) - 1],
                                   vertices[std::stoi(parts[2]) - 1],
                                   vertices[std::stoi(parts[3]) - 1],
                                   triColour);
            triangles.push_back(triangle);
        }

        if (parts[0] == "usemtl")
        {
            colour = parts[1];
        }
    }
    file.close();
    return triangles;
}


CanvasPoint get_CanvasPoint(glm::vec3 cam_position, glm::vec3 vertex, float focal_length){
    CanvasPoint point = CanvasPoint();
    vertex -= cam_position;
    point.x = ((focal_length / vertex.z) * -vertex.x) * 150 + WIDTH / 2;
    point.y = ((focal_length / vertex.z) * vertex.y) * 150 + HEIGHT / 2;
    point.depth = -vertex.z;
    return point;
}


// Drawing the points given in the 3dfile.
void draw_Points(DrawingWindow &window){
    std::string obj_file = "./cornell-box.obj";
    float scale = 0.35;
    std::string mtl_file = "./cornell-box.mtl";

    std::unordered_map<std::string, Colour> colours = parse_Mtl(mtl_file);
    std::vector<ModelTriangle> t = parse_Obj( obj_file, scale, colours);
    uint32_t colourNum = (255 << 24) + (255 << 16) + (255 << 8) + 255;

    for(auto mt : t){
        for(int i = 0; i < 3; i ++){
            CanvasPoint current_point = get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[i], 2);
            window.setPixelColour(round(current_point.x), round(current_point.y), colourNum);
        }
    }
}


// draw the outline/frame of the models
void draw_Frame(DrawingWindow &window){
    std::string obj_file = "./cornell-box.obj";
    float scale = 0.35;
    std::string mtl_file = "./cornell-box.mtl";

    std::unordered_map<std::string, Colour> colours = parse_Mtl(mtl_file);
    std::vector<ModelTriangle> t = parse_Obj( obj_file, scale, colours);
    Colour c = Colour(255, 255, 255);

    for (auto mt : t)
    {
    	CanvasTriangle t = CanvasTriangle(get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[0], 2),
    	                                  get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[1], 2),
    	                                  get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[2], 2));
    	draw_Triangle(t, c, window);
    }
}


void draw_Rasterised(DrawingWindow &window){
    std::string obj_file = "./cornell-box.obj";
    float scale = 0.35;
    std::string mtl_file = "./cornell-box.mtl";

    std::unordered_map<std::string, Colour> colours = parse_Mtl(mtl_file);
    std::vector<ModelTriangle> t = parse_Obj( obj_file, scale, colours);

    for (auto mt:t)
    {
        Colour c = mt.colour;
    	CanvasTriangle t = CanvasTriangle(get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[0], 2),
    	                                  get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[1], 2),
    	                                  get_CanvasPoint(glm::vec3(0.0, 0.0, 4.0), mt.vertices[2], 2));
    	fill_Triangle(t, c, window);
    }
}


void draw_RasterisedDepth(DrawingWindow &window){
    std::string obj_file = "./cornell-box.obj";
    float scale = 0.35;
    std::string mtl_file = "./cornell-box.mtl";

    std::unordered_map<std::string, Colour> colours = parse_Mtl(mtl_file);
    std::vector<ModelTriangle> t = parse_Obj( obj_file, scale, colours);

    // initialise the depth 2d array as a pointers
    float** depthBuff = new float*[HEIGHT];
    for (int i = 0; i < HEIGHT; i++)
    {
        depthBuff[i] = new float[WIDTH];
        for (int j = 0; j < WIDTH; j++)
        {
            depthBuff[i][j] = 0;
        }
    }

    for (auto mt:t)
        {
            Colour c = mt.colour;
        	CanvasTriangle t = CanvasTriangle(get_CanvasPoint(cam_position, mt.vertices[0], 2),
        	                                  get_CanvasPoint(cam_position, mt.vertices[1], 2),
        	                                  get_CanvasPoint(cam_position, mt.vertices[2], 2));
        	fill_TriangleDepth(t, c, window, depthBuff);
        }
    std::cout << "render" << std::endl;
}


CanvasPoint get_CanvasPointByMat(glm::vec3 cam_position ,glm::mat3 cam_rotate, glm::vec3 vertex, float focal_length, int scale) {
	//cam_vertex --> camera to vertex
	glm::vec3 cam_vertex = vertex - cam_position;
	vertex = cam_vertex * cam_rotate;
	vertex.x = ((focal_length / vertex.z) * -vertex.x) * scale + WIDTH / 2;
	vertex.y = ((focal_length / vertex.z) * vertex.y) * scale + HEIGHT / 2;
	vertex.z = -vertex.z;
	CanvasPoint point = CanvasPoint(vertex.x, vertex.y, vertex.z);
	return point;
}


void draw_RasterisedDepthByCam(DrawingWindow& window, glm::vec3 cam_position, glm::mat3 cam_or) {
    std::string obj_file = "./cornell-box.obj";
    float scale = 0.35;
    std::string mtl_file = "./cornell-box.mtl";

    std::unordered_map<std::string, Colour> colours = parse_Mtl(mtl_file);
    std::vector<ModelTriangle> t = parse_Obj( obj_file, scale, colours);

    // initialise the depth 2d array as a pointers
    float** depthBuff = new float*[HEIGHT];
    for (int i = 0; i < HEIGHT; i++)
    {
        depthBuff[i] = new float[WIDTH];
        for (int j = 0; j < WIDTH; j++)
        {
            depthBuff[i][j] = 0;
        }
    }

	for (auto mt : t) {
		Colour c = mt.colour;

		CanvasTriangle t = CanvasTriangle(get_CanvasPointByMat(cam_position, cam_or, mt.vertices[0], 2, 150),
		                                  get_CanvasPointByMat(cam_position, cam_or, mt.vertices[1], 2, 150),
		                                  get_CanvasPointByMat(cam_position, cam_or, mt.vertices[2], 2, 150));

		fill_TriangleDepth(t, c, window, depthBuff);
	}
}


void move_CameraLeft(DrawingWindow& window, glm::vec3 &cam_position) {
	cam_position.x += 1;
	window.clearPixels();
	//std::cout << "Left" << std::endl;
}


void move_CameraRight(DrawingWindow& window, glm::vec3 &cam_position) {
	cam_position.x -= 1;
	window.clearPixels();
	//std::cout << "Right" << std::endl;
}


void move_CameraUp(DrawingWindow& window, glm::vec3 &cam_position) {
	cam_position.y -= 1;
	window.clearPixels();
	//std::cout << "Up" << std::endl;

}


void move_CameraDown(DrawingWindow& window, glm::vec3 &cam_position) {
	cam_position.y += 1;
	window.clearPixels();
	//std::cout << "Down" << std::endl;
}


void rotate_Up(DrawingWindow& window, glm::vec3& cam_position) {
	float degree = 0.01;
	glm::mat3 rotateMat = glm::mat3(1, 0, 0, 0, cos(degree), sin(degree), 0, -sin(degree), cos(degree));
	cam_position = rotateMat * cam_position;
}


void rotate_AntiClock(DrawingWindow& window, glm::vec3& cam_position) {
	float degree = 0.01;
	glm::mat3 rotateMat = glm::mat3(cos(degree), 0, sin(degree), 0, 1, 0, -sin(degree), 0, cos(degree));
	cam_position = cam_position * rotateMat;
}


void look_At(glm::vec3& cam_position, glm::mat3& cameraOrMat) {
	glm::vec3 forward = glm::normalize(cam_position - glm::vec3(0, 0, 0));
	glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	cameraOrMat[0] = right;
	cameraOrMat[1] = up;
	cameraOrMat[2] = forward;
}


void rotate_UpMat(DrawingWindow& window, glm::vec3& cam_position, glm::mat3& cameraOrMat) {
	float degree = 0.01;
	glm::mat3 rotateMat = glm::mat3(1, 0, 0, 0, cos(degree), sin(degree), 0, -sin(degree), cos(degree));
	cam_position = rotateMat * cam_position;
	look_At(cam_position, cameraOrMat);
}


void rotate_AntiClockMat(DrawingWindow& window, glm::vec3& cam_position, glm::mat3& cameraOrMat) {
	float degree = 0.001;
	glm::mat3 rotateMat = glm::mat3(cos(degree), 0, sin(degree), 0, 1, 0, -sin(degree), 0, cos(degree));
	cam_position = rotateMat * cam_position;
	look_At(cam_position, cameraOrMat);
}


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) move_CameraLeft(window, cam_position);
		else if (event.key.keysym.sym == SDLK_RIGHT) move_CameraRight(window, cam_position);
		else if (event.key.keysym.sym == SDLK_UP) move_CameraUp(window, cam_position);
		else if (event.key.keysym.sym == SDLK_DOWN) move_CameraDown(window, cam_position);
        else if (event.key.keysym.sym == SDLK_q) rotate_Up(window, cam_position);
        else if (event.key.keysym.sym == SDLK_w) rotate_AntiClock(window, cam_position);

        else if (event.key.keysym.sym == SDLK_a) rotate_UpMat(window, cam_position, cameraOrMat);
        else if (event.key.keysym.sym == SDLK_s) rotate_AntiClockMat(window, cam_position, cameraOrMat);

        else if (event.key.keysym.sym == SDLK_d) orbit = orbit ? false:true;

		//else if (event.key.keysym.sym == SDLK_y)  draw_MultiTri(window);
		//else if (event.key.keysym.sym == SDLK_u) draw_RandFillTri(window);
		//else if (event.key.keysym.sym == SDLK_i) draw_RasterisedDepthByCam(window, cam_position, cameraOrMat);
		//else if (event.key.keysym.sym == SDLK_c) window.clearPixels();

	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}


void animation(DrawingWindow &window){

}


void draw(DrawingWindow &window){
    window.clearPixels();
    //draw_Red(window);
    //drawGradient(window);
    //draw_TextureTriangle(window);
    //drawColorGradient(window);
    //drawLine(from, to, c, window);
    //draw_Point(window);
    //draw_Frame(window);
    //draw_Rasterised(window);
    //draw_RasterisedDepth(window);
    draw_RasterisedDepthByCam(window, cam_position, cameraOrMat);

    if (orbit == true) {
        rotate_AntiClockMat(window, cam_position, cameraOrMat);
    }
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	int padding = 1;

	//CanvasPoint from(0, 0);
	//CanvasPoint to(window.width-1, window.height-1);
	//Colour c(144, 250, 15);

	while (true)
	{

		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
        draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();

        if (padding < 10){
            window.savePPM("0000" + std::to_string(padding) + ".ppm");
        }
        else if (padding < 100){
            window.savePPM("000" + std::to_string(padding) + ".ppm");
        }
        else if (padding < 1000){
            window.savePPM("00" + std::to_string(padding) + ".ppm");
        }
        else if (padding < 10000){
            window.savePPM("0" + std::to_string(padding) + ".ppm");
        }
        else if (padding > 10000){
            window.savePPM(std::to_string(padding) + ".ppm");
        }
		//window.savePPM(std::to_string(padding) + ".ppm");
		padding = padding + 1;
	}
}
