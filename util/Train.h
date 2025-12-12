#ifndef TRAIN_H
#define TRAIN_H

#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>
#include <vector>

using namespace graphics;

class Train : public VisualAsset
{
private:
    Brush brush;

public:
    Train(float posX, float posY, std::vector Passengers) : VisualAsset(posX, posY)
    {

	brush.fill_color[3] = {0.6f, 0.4f, 0.3f};
	void draw() override {

	drawRect(posX, posY, brush);







	}














	 //TODO: REMOVE/ADD PASSENGER FROM/TO TRAIN'S VECTOR (FUNCTION)
	};








};
#endif // TRAIN_H