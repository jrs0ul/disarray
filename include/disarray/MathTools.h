#ifndef USEFULL_H
#define USEFULL_H
#include "Vectors.h"

#define CLAMP(x, lo, hi)    ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
int roundDouble2Int(double x);

bool CollisionCircleCircle(float x1, float y1, float radius1,
                             float x2, float y2, float radius2);

bool CollisionCircleRectangle(float cirleCenterX, float circleCenterY, float circleRadius,
                              float rectX, float rectY, float rectWidth, float rectHeight);
bool CollisionRectangleRectangle(float rect1X, float rect1Y, float rect1Width, float rect1Height,
                                 float rect2X, float rect2Y, float rect2Width, float rect2Height);

bool CollisionCircleLineSegment(float lineX1, float lineY1, float lineX2, float lineY2,
                                float circleCenterX, float circleCenterY, float circleRadius);

bool CollisionLineSegmentLineSegment(float line1X1, float line1Y1, float line1X2, float line1Y2,
                                     float line2X1, float line2Y1, float line2X2, float line2Y2,
                                     float* resR = nullptr, float* resS = nullptr);

Vector3D MakeVectorM(float speedx, float speedy, float _angle );

Vector3D Vec2Reflection(Vector3D& vector, Vector3D& plane);

Vector3D Lerp(Vector3D org, Vector3D dest, float fProgress);

#endif// USEFULL_H
