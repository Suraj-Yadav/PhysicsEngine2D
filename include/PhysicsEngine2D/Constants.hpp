#ifndef CONSTANTS_H
#define CONSTANTS_H
enum ShapeType {
	BASESHAPE = 0b1,		   // 1
	DYNAMICSHAPE = 0b10001,	   // 17
	LINE = 0b10010,			   // 18
	PARTICLE = 0b100010001,	   // 273
	RIGIDSHAPE = 0b100010010,  // 274
	BALL = 0b1000100100001,	   // 4385
	BOX = 0b1000100100010,	   // 4386
};
inline const char* getShapeTypeName(const ShapeType& type) {
	switch (type) {
		case BASESHAPE:
			return "BASESHAPE";
		case DYNAMICSHAPE:
			return "DYNAMICSHAPE";
		case LINE:
			return "LINE";
		case PARTICLE:
			return "PARTICLE";
		case RIGIDSHAPE:
			return "RIGIDSHAPE";
		case BALL:
			return "BALL";
		case BOX:
			return "BOX";
	}
	return "";
}
#endif	// CONSTANTS_H
