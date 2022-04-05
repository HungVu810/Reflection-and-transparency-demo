#include "../include/light_model.h"

light_model::light_model() : model(){};

light_model::light_model(const light_model &light) : model(*this){
	outer_cone = light.getOuterConeAngle();
	inner_cone = light.getInnerConeAngle();
};

light_model& light_model::operator=(const light_model &light){
	model::operator=(light);
	outer_cone = light.getOuterConeAngle();
	inner_cone = light.getInnerConeAngle();
	return *this;
};

void light_model::assignOuterConeAngle(float radian){
	// make assertion on radian
	outer_cone = radian;
};
void light_model::assignInnerConeAngle(float radian){
	// make assertion on radian
	inner_cone = radian;
};

float light_model::getOuterConeAngle() const{
	return outer_cone;
};

float light_model::getInnerConeAngle() const{
	return inner_cone;
};
