#include <disarray/MathTools.h>
#include <disarray/Matrix.h>
 
#include <cmath>
#include <cstdio>
 
int roundDouble2Int(double x){
    return int(x > 0.0 ? x + 0.5 : x - 0.5);
}
//-----------------------------------------
bool CollisionCircleCircle(float x1, float y1, float radius1,
                             float x2, float y2, float radius2){
 
    float difx = (float)fabs(x1-x2);
    float dify = (float)fabs(y1-y2);
 
    float distance = (float)sqrt(difx*difx+dify*dify);
    if (distance < radius1 + radius2)
        return true;
 
 
    return false;
}
//-------------------------------------------------------
bool CollisionCircleRectangle(float circleCenterX, float circleCenterY, float circleRadius,
                              float rectX, float rectY, float rectWidth, float rectHeight){
    
    float tmpX = circleCenterX;
    float tmpY = circleCenterY;

    if (tmpX > rectX + rectWidth)  tmpX = rectX + rectWidth;
    if (tmpX < rectX)              tmpX = rectX;
    if (tmpY > rectY + rectHeight) tmpY = rectY + rectHeight;
    if (tmpY < rectY)              tmpY = rectY;


    float difx = (float) fabs (tmpX - circleCenterX);
    float dify = (float) fabs (tmpY - circleCenterY);
    float distance = (float) sqrt (difx * difx + dify * dify);

    if (distance < circleRadius) return true;

    return false; 
}

bool CollisionRectangleRectangle(float rect1X, float rect1Y, float rect1Width, float rect1Height,
                                 float rect2X, float rect2Y, float rect2Width, float rect2Height)
{
    if (rect1X + rect1Width >= rect2X &&     
        rect1X <= rect2X + rect2Width &&       
        rect1Y + rect1Height >= rect2Y &&       
        rect1Y <= rect2Y + rect2Height) 
    {       
        return true;
    }

    return false;
}



bool CollisionCircleLineSegment(float lineX1, float lineY1, float lineX2, float lineY2,
                                float circleCenterX, float circleCenterY, float circleRadius)
{
    Vector3D lineV1(lineX1, lineY1, 0);
    Vector3D lineV2(lineX2, lineY2, 0);
    Vector3D segmentVector = lineV2 - lineV1;
    Vector3D circlePos(circleCenterX, circleCenterY, 0);
    Vector3D pointVector = circlePos - lineV1;

    Vector3D segmentVectorNornalized = segmentVector;
    segmentVectorNornalized.normalize();

    float projection = pointVector * segmentVectorNornalized;

    Vector3D closestPointOnSegment;

    if (projection <= 0.f)
    {
        closestPointOnSegment = lineV1;
    }
    else if (projection >= segmentVector.length())
    {
        closestPointOnSegment = lineV2;
    }
    else
    {
        Vector3D projectionVector(segmentVectorNornalized.x * projection,
                                  segmentVectorNornalized.y * projection,
                                  0);
        closestPointOnSegment = projectionVector + lineV1;

    }


    Vector3D distanceVector = circlePos - closestPointOnSegment;
    float distance = distanceVector.length();

    return (distance <= circleRadius);
}

bool CollisionLineSegmentLineSegment(float line1X1, float line1Y1, float line1X2, float line1Y2,
                                     float line2X1, float line2Y1, float line2X2, float line2Y2,
                                     float* resR, float* resS)
{
    Vector3D a(line1X1, line1Y1, 0);
    Vector3D b(line1X2, line1Y2, 0);
    Vector3D c(line2X1, line2Y1, 0);
    Vector3D d(line2X2, line2Y2, 0);

    float denominator = ((b.x - a.x) * (d.y - c.y)) - ((b.y - a.y) * (d.x - c.x));

    if (denominator == 0)
    {
        return false;
    }



    float numerator1 = ((a.y - c.y) * (d.x - c.x)) - ((a.x - c.x) * (d.y - c.y));

    float numerator2 = ((a.y - c.y) * (b.x - a.x)) - ((a.x - c.x) * (b.y - a.y));

    if (numerator1 == 0 || numerator2 == 0)
    {
        return false;
    }

    float r = numerator1 / denominator;
    float s = numerator2 / denominator;


    if (resR)
    {
        *resR = r;
    }

    if (resS)
    {
        *resS = s;
    }

    return (r > 0 && r < 1) && (s > 0 && s < 1);
}


//-------------------------------------------------------------------
Vector3D MakeVectorM(float speedx, float speedy, float _angle ){
    Matrix rot;
    Vector3D pn(speedx, speedy, 0);
    MatrixRotationAxis(_angle, Vector3D(0,0,-1), rot);
    pn.transform(rot);

    return pn;
}
//-------------------------------------------------------------------
Vector3D Vec2Reflection(Vector3D& vector, Vector3D& plane){
 
    Vector3D perp(plane.y, -plane.x, 0);
 
    float dot1 = perp * perp;
    float dot2 = perp * vector;
    Vector3D v1(vector.v[0] * dot1, vector.v[1] * dot1, vector.v[2] * dot1);
    dot2 = dot2 * 2;
    Vector3D v2(perp.v[0] * dot2, perp.v[1] * dot2, perp.v[2] * dot2); 

    Vector3D reflection = v1 - v2;
    reflection.normalize();
 
    return reflection;
}

Vector3D Lerp(Vector3D org, Vector3D dest, float fProgress)
{

    Vector3D result;

    result.v[0] = org.v[0] + fProgress * (dest.v[0] - org.v[0]);
    result.v[1] = org.v[1] + fProgress * (dest.v[1] - org.v[1]);
    result.v[2] = org.v[2] + fProgress * (dest.v[2] - org.v[2]);

    return result;

}

